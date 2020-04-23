// [1]: taken from https://github.com/GameTechDev/ASSAO
// and GPU ZEN - Advanced Rendering Techniques, Chapter Scalable Adaptive SSAO, p. 194, Filip Strugar

#define SSAO_QUALITY_PRESET_LOW         0
#define SSAO_PRESET_QUALITY_MEDIUM      1
#define SSAO_PRESET_QUALITY_HIGH        2
#define SSAO_PRESET_QUALITY_ADAPTIVE    3

struct Samples
{
    float x,y,z; 
};

layout(std430, binding = 1) readonly buffer SamplesBuffer { Samples samples[]; };
uniform int sample_cnt;
uniform float radius;
uniform int current_buffer;
uniform uint quality_preset;

uniform sampler2D normal_tx2D;
uniform sampler2D depth_tx2D;
uniform sampler2D noise_tx2D;

layout(RG8) writeonly uniform image2D ao_edge_tx2D;

uniform mat4 view_mx;
uniform mat4 proj_mx;
uniform mat4 inv_view_mx;
uniform mat4 inv_proj_mx;


vec3 depthToViewPos(float depth, vec2 uv) {
    float z = depth * 2.0 - 1.0;

    vec4 cs_pos = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 vs_pos = inv_proj_mx * cs_pos;

    // Perspective division
    vs_pos /= vs_pos.w;
    
    return vs_pos.xyz;
}


// see [1] on top
// packing/unpacking for edges; 2 bits per edge mean 4 gradient values (0, 0.33, 0.66, 1) for smoother transitions!
float PackEdges( vec4 edgesLRTB )
{
    // int4 edgesLRTBi = int4( saturate( edgesLRTB ) * 3.0 + 0.5 );
    // return ( (edgesLRTBi.x << 6) + (edgesLRTBi.y << 4) + (edgesLRTBi.z << 2) + (edgesLRTBi.w << 0) ) / 255.0;

    // optimized, should be same as above
    edgesLRTB = round( clamp( edgesLRTB, 0.0, 1.0 ) * 3.05 );
    return dot( edgesLRTB, float4( 64.0 / 255.0, 16.0 / 255.0, 4.0 / 255.0, 1.0 / 255.0 ) ) ;
}


// see [1] on top
vec4 depthBasedEdgeDetection(float center_z, float left_z, float right_z, float top_z, float bottom_z) {
    vec4 edgesLRTB = vec4(left_z, right_z, top_z, bottom_z) - vec4(center_z);
    vec4 edgesLRTBSlopeAdjusted = edgesLRTB + edgesLRTB.yxwz;
    edgesLRTB = min(abs(edgesLRTB), abs(edgesLRTBSlopeAdjusted));

    // 0 means edge, 1 means no edge (free to blur across)
    return clamp(vec4(1.3) - edgesLRTB / (center_z * 0.040), 0.0, 1.0);
}


layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;


// see https://learnopengl.com/Advanced-Lighting/SSAO
void main()
{
    uvec3 gID = gl_GlobalInvocationID.xyz;
    ivec2 pixel_coords = ivec2(gID.xy);
    ivec2 normal_coords = pixel_coords;

    // scale up the coordinates from half x half to full resoltion
    switch(current_buffer) {
        case 0: normal_coords = normal_coords * 2 + ivec2(0, 0); break; // bottom left
        case 1: normal_coords = normal_coords * 2 + ivec2(1, 0); break; // bottom right
        case 2: normal_coords = normal_coords * 2 + ivec2(0, 1); break; // top left
        case 3: normal_coords = normal_coords * 2 + ivec2(1, 1); break; // top right
        default: break;
    }

    ivec2 tgt_resolution = imageSize (ao_edge_tx2D);
    ivec2 normal_resolution = textureSize (normal_tx2D, 0);

    if (pixel_coords.x >= tgt_resolution.x || pixel_coords.y >= tgt_resolution.y) {
        return;
    }
    if (normal_coords.x >= normal_resolution.x || normal_coords.y >= normal_resolution.y) {
        return;
    }


    vec2 pixel_coords_norm = (vec2(pixel_coords) + vec2(0.5)) / vec2(tgt_resolution);
    vec2 normal_coords_norm = (vec2(normal_coords) + vec2(0.5)) / vec2(normal_resolution);

    // tile noise texture over screen based on screen dimensions divided by noise size
    vec2 noise_scale = vec2(tgt_resolution.x / 4.0, tgt_resolution.y / 4.0);
    
    vec3 normal    = texture(normal_tx2D, normal_coords_norm).rgb;
    // TODO use mip maps during sampling
    // is this automatically happening here?
    float depth    = texture(depth_tx2D, pixel_coords_norm).r;
    vec3 rand_vec  = texture(noise_tx2D, normal_coords_norm * noise_scale).xyz;

    vec3 view_pos = depthToViewPos(depth, pixel_coords_norm);

    normal = transpose(mat3(inv_view_mx)) * normal; // transform normal to view space
    vec3 tangent    = normalize(rand_vec - normal * dot(rand_vec, normal));
    vec3 bitangent  = cross(normal, tangent);
    mat3 tangent_mx = mat3(tangent, bitangent, normal);
    
    float bias = 0.0001;

    float occlusion = 0.0;

    vec3 sample_vs_pos;
    vec3 frag_vs_pos;

    if(depth > 0.0)
    {
        for(int i = 0; i < sample_cnt; ++i)
        {
            // get sample position
            sample_vs_pos = tangent_mx * vec3(samples[i].x, samples[i].y, samples[i].z); // From tangent to view-space
            sample_vs_pos = view_pos + sample_vs_pos * radius;

            vec4 offset = vec4(sample_vs_pos, 1.0);
            offset      = proj_mx * offset;       // from view to clip-space
            offset.xyz /= offset.w;               // perspective divide
            offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

            if(offset.x > 1.0 || offset.x < 0.0 || offset.y > 1.0 || offset.y < 0.0f)
                continue;

            float sample_depth = texture(depth_tx2D, offset.xy).r;

            if(sample_depth < 0.0001)
                continue;

            frag_vs_pos = depthToViewPos(sample_depth, offset.xy);

            float range_check = smoothstep(0.0, 1.0, radius / abs(length(view_pos) - length(frag_vs_pos)));
            occlusion += (length(frag_vs_pos) <= length(sample_vs_pos) - bias ? 1.0 : 0.0) * range_check;
        }
    }
    
    occlusion = 1.0 - (occlusion / sample_cnt);

    // get z-values of neighbors of current pixel
    // caution: border pixels may cause artifacts
    // TODO better border handling
    // ASSAO extends image to have the actual border not at the screen border
    float packed_edges = 1.0;

    if(quality_preset >= SSAO_PRESET_QUALITY_MEDIUM) {
        float lz = textureOffset(depth_tx2D, pixel_coords, ivec2(-1, 0)).r;
        float rz = textureOffset(depth_tx2D, pixel_coords, ivec2(1, 0)).r;
        float tz = textureOffset(depth_tx2D, pixel_coords, ivec2(0, 1)).r;
        float bz = textureOffset(depth_tx2D, pixel_coords, ivec2(0, -1)).r;

        // transform neighbour dephs to viewspace
        lz = depthToViewPos(lz, pcn_lz).r;
        rz = depthToViewPos(rz, pcn_rz).r;
        tz = depthToViewPos(tz, pcn_tz).r;
        bz = depthToViewPos(bz, pcn_bz).r;

        vec4 edgesLRTB = depthBasedEdgeDetection(view_pos.z, lz, rz, tz, bz);

        packed_edges = PackEdges(edgesLRTB);
    }


    imageStore(ao_edge_tx2D, pixel_coords, vec4(occlusion, packed_edges, 1.0, 1.0));
}   