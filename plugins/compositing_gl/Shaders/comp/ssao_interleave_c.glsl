uniform sampler2D inter0_tx2D;
uniform sampler2D inter1_tx2D;
uniform sampler2D inter2_tx2D;
uniform sampler2D inter3_tx2D;

layout(RGBA16) writeonly uniform image2D tgt_tx2D;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// see https://learnopengl.com/Advanced-Lighting/SSAO
void main()
{
    uvec3 gID = gl_GlobalInvocationID.xyz;
    ivec2 pixel_coords = ivec2(gID.xy);
    

    // interleave original the four half x half textures
    if(pixel_coords.x % 2 == 0 && pixel_coords.y % 2 == 0) {
        float occlusion = texture(inter0_tx2D, pixel_coords).r;
        imageStore(tgt_tx2D, pixel_coords * 2, vec4(vec3(occlusion), 1));                 // bottom left
    } else if(pixel_coords.x % 2 == 1 && pixel_coords.y % 2 == 0) {
        float occlusion = texture(inter1_tx2D, pixel_coords).r;
        imageStore(tgt_tx2D, pixel_coords * 2 + ivec2(1, 0), vec4(vec3(occlusion), 1));   // bottom right
    } else if(pixel_coords.x % 2 == 0 && pixel_coords.y % 2 == 1) {
        float occlusion = texture(inter2_tx2D, pixel_coords).r;
        imageStore(tgt_tx2D, pixel_coords * 2 + ivec2(0, 1), vec4(vec3(occlusion), 1));   // top left
    } else {
        float occlusion = texture(inter3_tx2D, pixel_coords).r;
        imageStore(tgt_tx2D, pixel_coords * 2 + ivec2(1), vec4(vec3(occlusion), 1));      // top right
    }
}