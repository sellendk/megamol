void PSGenerateQ3( in vec4 inPos : SV_POSITION/*, in vec2 inUV : TEXCOORD0*/, out vec2 out0 : SV_Target0 )
{
    float   outShadowTerm;
    float   outWeight;
    vec4  outEdges;
    GenerateSSAOShadowsInternal( outShadowTerm, outEdges, outWeight, inPos.xy/*, inUV*/, 3, false);
    out0.x = outShadowTerm;
    out0.y = PackEdges( outEdges );
}