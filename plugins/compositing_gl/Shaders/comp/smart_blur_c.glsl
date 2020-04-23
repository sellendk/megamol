// [1]: taken from https://github.com/GameTechDev/ASSAO
// and GPU ZEN - Advanced Rendering Techniques, Chapter Scalable Adaptive SSAO, p. 194, Filip Strugar

#define SSAO_QUALITY_PRESET_LOW         0
#define SSAO_PRESET_QUALITY_MEDIUM      1
#define SSAO_PRESET_QUALITY_HIGH        2
#define SSAO_PRESET_QUALITY_ADAPTIVE    3

#define SSAO_NORMAL_BASED_EDGES_THRESHOLD 0.5

uniform sampler2D src_tx2D;
uniform sampler2D normal_tx2D;

uniform int current_buffer;
uniform uint quality_preset;

layout(RGBA16) writeonly uniform image2D tgt_tx2D;

// see [1] on top
vec4 UnpackEdges( float packed_value )
{
    uint packed_val = (uint)(packed_value * 255.5);
    vec4 edgesLRTB = vec4(0.0);
    edgesLRTB.x = float((packed_val >> 6) & 0x03) / 3.0; // there's really no need for mask (as it's an 8 bit input) 
    edgesLRTB.y = float((packed_val >> 4) & 0x03) / 3.0; // but I'll leave it in so it doesn't cause any trouble in the future
    edgesLRTB.z = float((packed_val >> 2) & 0x03) / 3.0;
    edgesLRTB.w = float((packed_val >> 0) & 0x03) / 3.0;

    return clamp( edgesLRTB /*+ Inv_Sharpness*/, 0.0, 1.0 ); // TODO add uniform for (inv)sharpness
}

void AddSample( float ssao_value, float edge_value, inout float sum, inout float sum_weight )
{
    float weight = edge_value;

    sum += (weight * ssao_value);
    sum_weight += weight;
}

float SumElements(vec4 v) {
    return v.x + v.y + v.z + v.w;
}

float SumElements(vec3 v) {
    return v.x + v.y + v.z;
}


layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// see http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA
void main()
{
    uvec3 gID = gl_GlobalInvocationID.xyz;
    ivec2 pixel_coords = ivec2(gID.xy);
    ivec2 src_resolution = textureSize (src_tx2D, 0);

    if (pixel_coords.x >= src_resolution.x || pixel_coords.y >= src_resolution.y) {
        return;
    }

    vec2 pixel_coords_norm = (vec2(pixel_coords) + vec2(0.5)) / vec2(src_resolution);
    vec2 texel_size = vec2(0.5) / vec2(src_resolution);
    
    // only blur if there is no edge
    // otherwise spatially separate surface might be blurred
    // which results in degrading sharpness and other issues
    float packed_edge = texture(src_tx2D, pixel_coords_norm).y;
    vec4 unpacked_edges = UnpackEdges(packed_edge);

    // -------------------------------------------------------------------------------
    // Depth and Normal Based Blur
    // -------------------------------------------------------------------------------
    if(quality_preset >= SSAO_PRESET_QUALITY_HIGH) {
        vec3 normalC = texelFetch(normal_tx2D, pixel_coords, 0).xyz;
        vec3 normalL = texelOffset(normal_tx2D, pixel_coords, ivec2(-2, 0)).xyz;
        vec3 normalR = texelOffset(normal_tx2D, pixel_coords, ivec2(0, 2)).xyz;
        vec3 normalT = texelOffset(normal_tx2D, pixel_coords, ivec2(2, 0)).xyz;
        vec3 normalB = texelOffset(normal_tx2D, pixel_coords, ivec2(-2, 0)).xyz;

        const float dotThreshold = SSAO_NORMAL_BASED_EDGES_THRESHOLD;

        vec4 normal_edgesLRTB;
        normal_edgesLRTB.x = clamp( dot( normalC, normalL ) + dotThreshold, 0.0, 1.0);
        normal_edgesLRTB.y = clamp( dot( normalC, normalR ) + dotThreshold, 0.0, 1.0);
        normal_edgesLRTB.z = clamp( dot( normalC, normalT ) + dotThreshold, 0.0, 1.0);
        normal_edgesLRTB.w = clamp( dot( normalC, normalB ) + dotThreshold, 0.0, 1.0);

        edgesLRTB *= normal_edgesLRTB;
    }
    // -------------------------------------------------------------------------------
    // Depth and Normal Based Blur end
    // -------------------------------------------------------------------------------


    // -------------------------------------------------------------------------------
    // Smart Depth Based Blur
    // -------------------------------------------------------------------------------
    // get all x - components in a (5 x 5) neihgborhood around the current pixel
    if(quality_preset >= SSAO_PRESET_QUALITY_MEDIUM) {
        vec4 ssaoCUL = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(-1, 0), 0);   // 0
        vec4 ssaoBR = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(0, -1), 0);    // 1
    
        // fetch all ssaoValues around current pixel
        //      sCUL.x      sCUL.y
        //      sCUL.w  sCUL.z/sBR.x  sBR.y
        //                  sBR.w     sBR.z
        float ssao_value     = ssaoCUL.z;    // center
        float ssao_valueL    = ssaoCUL.w;    // left
        float ssao_valueT    = ssaoCUL.y;    // top
        float ssao_valueR    = ssaoBR.y;     // right
        float ssao_valueB    = ssaoBR.w;     // bottom
        
        float sum_weight = 0.5;
        float sum = ssaoValue * sum_weight;
    
        // sum up all weights and all weighted ssaoValues
        AddSample(ssao_valueL, unpacked_edges.x, sum, sum_weight);
        AddSample(ssao_valueR, unpacked_edges.y, sum, sum_weight);
        AddSample(ssao_valueT, unpacked_edges.z, sum, sum_weight);
        AddSample(ssao_valueB, unpacked_edges.w, sum, sum_weight);
    
        float ssao_avg = sum / sum_weight;
    
        ssao_value = ssao_avg;
    }
    // -------------------------------------------------------------------------------
    // Smart Depth Based Blur end
    // -------------------------------------------------------------------------------

    
    // -------------------------------------------------------------------------------
    // Simple Blur
    // -------------------------------------------------------------------------------
    float simple_blur_value = 0.0;

    if(quality_preset == SSAO_QUALITY_PRESET_LOW) {

        // sum over all previously fetched ssao values of its direct neighbors
        simple_blur_value = (ssaoCUL.y + ssaoCUL.z + ssaoCUL.w 
                            + ssaOBR.y + ssaoBR.w);
        simple_blur_value *= 0.2;

        ssao_value = simple_blur_value;
    }

#ifdef false
    // alternative simple blurring
    // only '10' texture accesses instead of 25 in the alternative, more expensive simple blurring down below
    // |4.x|4.y|7.x|3.x|3.y|
    // |4.w|0.x|0.y|3.w|3.z|
    // |6.x|0.w|0.z|1.y|8.x|
    // |2.x|2.y|1.w|1.z|5.y|
    // |2.w|2.z|9.x|5.w|5.z|
    vec4  ssao_m2_m2 = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(-2, -2), 0);   // 2
    vec4  ssao_p1_p1 = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(1, 1), 0);     // 3
    vec4  ssao_m2_p1 = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(-2, 1), 0);    // 4
    vec4  ssao_p1_m2 = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(1, -2), 0);    // 5
    float ssao_m2_p0 = texelFetch(src_tx2D, pixel_coords + ivec2(-2, 0), 0).r;               // 6
    float ssao_m0_p2 = texelFetch(src_tx2D, pixel_coords + ivec2(0, 2), 0).r;                // 7
    float ssao_p2_p0 = texelFetch(src_tx2D, pixel_coords + ivec2(2, 0), 0).r;                // 8
    float ssao_p0_m2 = texelFetch(src_tx2D, pixel_coords + ivec2(0, -2), 0).r;               // 9
    simple_blur_value = SumElements(ssaoCUL) + SumElements(ssaoBR.yzw)
                      + SumElements(ssao_m2_m2) + SumElements(ssao_p1_p1)
                      + SumElements(ssao_m2_p1.xyw) + SumElements(ssao_p1_m2.yzw)
                      + ssao_m2_p0 + ssao_m0_p2 + ssao_p2_p0 + ssao_p0_m2;
    simple_blur_value /= 16.0; // / (4.0 + 4.0);
    ssao_value = simple_blur_value;

    // alternative, more expensive simple blurring
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            ivec2 offset = ivec2(x, y);
            simple_blur_value += texelFetch(src_tx2D, pixel_coords + offset,0).r;
        }
    }
    simple_blur_value /= 16.0; // / (4.0 * 4.0)

    ssao_value = simple_blur_value;
#endif
    // -------------------------------------------------------------------------------
    // Simple Blur end
    // -------------------------------------------------------------------------------
    

    switch(current_buffer) {
        case 0: pixel_coords = pixel_coords * 2 + ivec2(0, 0); break;   // bottom left
        case 1: pixel_coords = pixel_coords * 2 + ivec2(1, 0); break;   // bottom right
        case 2: pixel_coords = pixel_coords * 2 + ivec2(0, 1); break;   // top left
        case 3: pixel_coords = pixel_coords * 2 + ivec2(1, 1); break;   // top right
        default: break;
    }

    imageStore( tgt_tx2D, pixel_coords, vec4(ssao_value, packed_edge, 1.0, 1.0) );
}