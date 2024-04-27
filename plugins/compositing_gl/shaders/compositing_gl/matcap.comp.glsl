#version 430

uniform sampler2D normal_tx2D;
uniform sampler2D matcap_tx2D;

layout(OUTFORMAT) writeonly uniform image2D tgt_tx2D;

uniform mat4 view_mx;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

void main() {
    uvec3 gID = gl_GlobalInvocationID.xyz;
    ivec2 pixel_coords = ivec2(gID.xy);
    ivec2 tgt_resolution = imageSize (tgt_tx2D);

    if (pixel_coords.x >= tgt_resolution.x || pixel_coords.y >= tgt_resolution.y) {
        return;
    }

    vec2 pixel_coords_norm = (vec2(pixel_coords) + vec2(0.5)) / vec2(tgt_resolution);

    vec3 normal = texture(normal_tx2D, pixel_coords_norm).rgb;

    vec2 uv_normal = normalize(vec3(transpose(inverse(mat3(view_mx))) * normal)).xy;
    uv_normal = uv_normal * vec2(0.5) + vec2(0.5);
    vec4 retval = texture(matcap_tx2D, uv_normal);

    imageStore(tgt_tx2D, pixel_coords , retval);
}
