uniform sampler2D src_tx2D;

layout(RGBA16) writeonly uniform image2D tgt_tx2D;

uniform mat4 inv_view_mx;
uniform mat4 inv_proj_mx;

float fac(int n) {
	if( n <= 1 ) return 1.f;

	float sum = 1;
	while(n != 0) {
		sum *= n--;
	}

	return sum;
}

float binco(int n, int i) {
	if( n < 0 || i < 0 ) return -1;
	if( n == i || i == 0 ) return 1;

	float sum = fac(n) / (fac(i) * fac(n - i));

	return sum;
}

float B(int n, int i, float t) {
	return binco(n, i) * pow(t, i) * pow(1 - t, n - i);
}

vec3 derivativeU(int m, int n, float u, float v, vec3 b[9]) {

	vec3 sum = vec3(0.f);

	for(int j = 0; j <= n; ++j) {
		for(int i = 0; i < m; ++i) {
			vec3 delta = b[(i + 1) + 3 * j] - b[i + 3 * j];
			sum += delta * B(m-1, i, u) * B(n, j, v);
		}
	}

	return float(m) * sum;
}

vec3 derivativeV(int m, int n, float u, float v, vec3 b[9]) {

	vec3 sum = vec3(0.f);

	for(int i = 0; i <= m; ++i) {
		for(int j = 0; j < n; ++j) {
			vec3 delta = b[i + 3 * (j + 1)] - b[i + 3 * j];
			sum += delta * B(n-1, j, v) * B(m, i, u);
		}
	}

	return float(n) * sum;
}

vec3 smoothNormal(vec3 b[9]) {
  vec3 dU = derivativeU(2, 2, 0.5f, 0.5f, b);
  vec3 dV = derivativeV(2, 2, 0.5f, 0.5f, b);
  vec3 n = normalize(cross(dU, dV));

  return n;
}

vec4 depthToWorldPos(float depth, vec2 uv) {
    float z = depth * 2.0 - 1.0;

    vec4 cs_pos = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 vs_pos = inv_proj_mx * cs_pos;

    // Perspective division
    vs_pos /= vs_pos.w;

    vec4 ws_pos = inv_view_mx * vs_pos;

    return ws_pos;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

void main()
{
    uvec3 gID = gl_GlobalInvocationID.xyz;
    ivec2 pixel_coords = ivec2(gID.xy);
    ivec2 tgt_resolution = imageSize(tgt_tx2D);

    if (pixel_coords.x >= tgt_resolution.x || pixel_coords.y >= tgt_resolution.y) {
        return;
    }

    vec2 pixel_coords_norm = (vec2(pixel_coords) + vec2(0.5)) / vec2(tgt_resolution);
    vec2 pixel_offset = vec2(1.0) / vec2(tgt_resolution);

    float depth_center = texture(src_tx2D, pixel_coords_norm).r;


    if ((depth_center > 0.f) && (depth_center < 1.f)){
        // depths of neighbor fragments
        vec4 depth_c = depthToWorldPos(depth_center, pixel_coords_norm);

        vec2 pixel_coords_norm_t = pixel_coords_norm + vec2(0.0, pixel_offset.y);
        vec4 depth_t = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_t).x, pixel_coords_norm_t);

        vec2 pixel_coords_norm_b = pixel_coords_norm - vec2(0.0, pixel_offset.y);
        vec4 depth_b = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_b).x, pixel_coords_norm_b);

        vec2 pixel_coords_norm_l = pixel_coords_norm - vec2(pixel_offset.x, 0.0);
        vec4 depth_l = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_l).x, pixel_coords_norm_l);

        vec2 pixel_coords_norm_r = pixel_coords_norm + vec2(pixel_offset.x, 0.0);
        vec4 depth_r = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_r).x, pixel_coords_norm_r);

        vec2 pixel_coords_norm_tl = pixel_coords_norm + vec2(-pixel_offset.x, pixel_offset.y);
        vec4 depth_tl = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_tl).x, pixel_coords_norm_tl);

        vec2 pixel_coords_norm_tr = pixel_coords_norm + pixel_offset;
        vec4 depth_tr = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_tr).x, pixel_coords_norm_tr);

        vec2 pixel_coords_norm_bl = pixel_coords_norm - pixel_offset;
        vec4 depth_bl = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_bl).x, pixel_coords_norm_bl);

        vec2 pixel_coords_norm_br = pixel_coords_norm + vec2(pixel_offset.x, -pixel_offset.y);
        vec4 depth_br = depthToWorldPos(texture(src_tx2D, pixel_coords_norm_br).x, pixel_coords_norm_br);

        //vec3 arr[9] = {depth_bl.xyz, depth_b.xyz, depth_br.xyz, depth_l.xyz, depth_c.xyz, depth_r.xyz, depth_tl.xyz, depth_t.xyz, depth_tr.xyz};

        // background handling
        if (depth_bl.w < 0.5) { depth_bl = depth_c; }
        if (depth_tl.w < 0.5) { depth_tl = depth_c; }
        if (depth_br.w < 0.5) { depth_br = depth_c; }
        if (depth_tr.w < 0.5) { depth_tr = depth_c; }
        if (depth_l.w < 0.5) { depth_l = depth_c; }
        if (depth_b.w < 0.5) { depth_b = depth_c; }
        if (depth_t.w < 0.5) { depth_t = depth_c; }
        if (depth_r.w < 0.5) { depth_r = depth_c; }

        // evaluate at (0.5, 0.5)
        vec4 b0 = (depth_bl + 2.0 * depth_l + depth_tl) * 0.25;
        vec4 b1 = (depth_b + 2.0 * depth_c + depth_t) * 0.25;
        vec4 b2 = (depth_br + 2.0 * depth_r + depth_tr) * 0.25;
        vec3 dx = 0.5 * (b0.xyz + b1.xyz) - 0.5 * (b1.xyz + b2.xyz);
        b0 = (depth_bl + 2.0 * depth_b + depth_br) * 0.25;
        b1 = (depth_l + 2.0 * depth_c + depth_r) * 0.25;
        b2 = (depth_tl + 2.0 * depth_t + depth_tr) * 0.25;
        vec3 dy = 0.5 * (b0.xyz + b1.xyz) - 0.5 * (b1.xyz + b2.xyz);

        vec3 normal = normalize(cross(dx, dy));
        //vec3 normal = smoothNormal(arr);

        imageStore(tgt_tx2D, pixel_coords, vec4(normal,1.0) );
    }
    else{
        imageStore(tgt_tx2D, pixel_coords, vec4(0.0) );
    }
}
