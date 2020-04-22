// [1]: taken from https://github.com/GameTechDev/ASSAO
// and GPU ZEN - Advanced Rendering Techniques, Chapter Scalable Adaptive SSAO, p. 194, Filip Strugar

uniform sampler2D src_tx2D;
uniform int current_buffer;

layout(RGBA16) writeonly uniform image2D tgt_tx2D;

// see [1] on top
vec4 UnpackEdges( float packedValue )
{
    uint packedVal = (uint)(packedValue * 255.5);
    vec4 edgesLRTB = vec4(0.0);
    edgesLRTB.x = float((packedVal >> 6) & 0x03) / 3.0; // there's really no need for mask (as it's an 8 bit input) 
    edgesLRTB.y = float((packedVal >> 4) & 0x03) / 3.0; // but I'll leave it in so it doesn't cause any trouble in the future
    edgesLRTB.z = float((packedVal >> 2) & 0x03) / 3.0;
    edgesLRTB.w = float((packedVal >> 0) & 0x03) / 3.0;

    return clamp( edgesLRTB /*+ InvSharpness*/, 0.0, 1.0 ); // TODO add uniform for (inv)sharpness
}

void AddSample( float ssaoValue, float edgeValue, inout float sum, inout float sumWeight )
{
    float weight = edgeValue;

    sum += (weight * ssaoValue);
    sumWeight += weight;
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
    
    float packedEdge = texture(src_tx2D, pixel_coords_norm).y;
    vec4 unpackedEdges = UnpackEdges(packedEdge);

    // get all x - components
    vec4 ssaoCUL = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(-1, 0), 0);
    vec4 ssaoBR = textureGatherOffset(srx_tx2D, pixel_coords_norm, ivec2(0, -1), 0);

    // fetch all ssaoValues around current pixel
    //      sCUL.x      sCUL.y
    //      sCUL.w  sCUL.z/sBR.x  sBR.y
    //                  sBR.w     sBR.z
    float ssaoValue     = ssaoCUL.z;    // center
    float ssaoValueL    = ssaoCUL.w;    // left
    float ssaoValueT    = ssaoCUL.y;    // top
    float ssaoValueR    = ssaoBR.y;     // right
    float ssaoValueB    = ssaoBR.w;     // bottom
    
    float sumWeight = 0.5;
    float sum = ssaoValue * sumWeight;

    // sum up all weights and all weighted ssaoValues
    AddSample(ssaoValueL, unpackedEdges.x, sum, sumWeight);
    AddSample(ssaoValueR, unpackedEdges.y, sum, sumWeight);
    AddSample(ssaoValueT, unpackedEdges.z, sum, sumWeight);
    AddSample(ssaoValueB, unpackedEdges.w, sum, sumWeight);

    float ssaoAvg = sum / sumWeight;

    ssaoValue = ssaoAvg;

    // only blur if there is no edge
    // otherwise spatially separate surface might be blurred
    // which results in degrading sharpness and other issues

    // simple blur
    // vec4 result = vec4(0.0);
    // for (int x = -2; x < 2; ++x) 
    // {
    //     for (int y = -2; y < 2; ++y) 
    //     {
    //         ivec2 offset = ivec2(x, y);
    //         result += texelFetch(src_tx2D, pixel_coords + offset,0).rgba;
    //     }
    // }
    // result = result / 16.0; // / (4.0 * 4.0)
    

    switch(current_buffer) {
        case 0: pixel_coords = pixel_coords * 2 + ivec2(0, 0); break;   // bottom left
        case 1: pixel_coords = pixel_coords * 2 + ivec2(1, 0); break;   // bottom right
        case 2: pixel_coords = pixel_coords * 2 + ivec2(0, 1); break;   // top left
        case 3: pixel_coords = pixel_coords * 2 + ivec2(1, 1); break;   // top right
        default: break;
    }

    imageStore(tgt_tx2D, pixel_coords, vec4(ssaoValue, packedEdge, 1.0, 1.0) );
}