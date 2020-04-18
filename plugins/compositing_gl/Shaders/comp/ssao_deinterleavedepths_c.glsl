uniform sampler2D depth_tx2D;

layout(R16) writeonly uniform image2D tgt_tx2D_depth0;
layout(R16) writeonly uniform image2D tgt_tx2D_depth1;
layout(R16) writeonly uniform image2D tgt_tx2D_depth2;
layout(R16) writeonly uniform image2D tgt_tx2D_depth3;

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


layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// see https://learnopengl.com/Advanced-Lighting/SSAO
void main()
{
    uvec3 gID = gl_GlobalInvocationID.xyz;
    ivec2 pixel_coords = ivec2(gID.xy);
    ivec2 depth_resolution = textureSize (depth_tx2D, 0);

    if (pixel_coords.x >= depth_resolution.x || pixel_coords.y >= depth_resolution.y) {
        return;
    }

    vec2 pixel_coords_norm = (vec2(pixel_coords) + vec2(0.5)) / vec2(depth_resolution);
    float depth = texture(depth_tx2D, pixel_coords_norm).r;
    float view_depth = depthToViewPos(depth, pixel_coords_norm).r;

    // de-interleave original depth buffer into four half x half depth buffers
    // for better cache using
    if(pixel_coords.x % 2 == 0 && pixel_coords.y % 2 == 0) {
        imageStore(tgt_tx2D_depth0, pixel_coords / 2, vec4(view_depth));                   // bottom left
    } else if(pixel_coords.x % 2 == 1 && pixel_coords.y % 2 == 0) {
        imageStore(tgt_tx2D_depth1, (pixel_coords - ivec2(1, 0)) / 2, vec4(view_depth));   // bottom right
    } else if(pixel_coords.x % 2 == 0 && pixel_coords.y % 2 == 1) {
        imageStore(tgt_tx2D_depth2, (pixel_coords - ivec2(0, 1)) / 2, vec4(view_depth));   // top left
    } else {
        imageStore(tgt_tx2D_depth3, (pixel_coords - ivec2(1)) / 2, vec4(view_depth));      // top right
    }
}