uniform sampler2D src_tx2D;
uniform int current_buffer;

layout(RGBA16) writeonly uniform image2D tgt_tx2D;

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
    vec4 depth = texture(src_tx2D, pixel_coords_norm);
    float isEdge = texture(src_tx2D, pixel_coords_norm).y;

    vec4 result = vec4(0.0);

    // only blur if there is no edge
    // otherwise spatially separate surface might be blurred
    // which results in degrading sharpness and other issues
    if(isEdge > 0) {
        for (int x = -2; x < 2; ++x) 
        {
            for (int y = -2; y < 2; ++y) 
            {
                ivec2 offset = ivec2(x, y);
                result += texelFetch(src_tx2D, pixel_coords + offset,0).rgba;
            }
        }
        result = result / 16.0; // / (4.0 * 4.0)
    } else {
        result = depth;
    }

    switch(current_buffer) {
        case 0: pixel_coords = pixel_coords * 2 + ivec2(0, 0); break;   // bottom left
        case 1: pixel_coords = pixel_coords * 2 + ivec2(1, 0); break;   // bottom right
        case 2: pixel_coords = pixel_coords * 2 + ivec2(0, 1); break;   // top left
        case 3: pixel_coords = pixel_coords * 2 + ivec2(1, 1); break;   // top right
        default: break;
    }

    imageStore(tgt_tx2D, pixel_coords, result );
}