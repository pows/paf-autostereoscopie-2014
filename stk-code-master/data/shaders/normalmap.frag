uniform sampler2D normalMap;
uniform sampler2D DiffuseForAlpha;

in vec3 tangent;
in vec3 bitangent;
in vec2 uv;
out vec3 EncodedNormal;

vec2 EncodeNormal(vec3 n);

void main()
{
    // normal in Tangent Space
    vec3 TS_normal = 2.0 * texture(normalMap, uv).rgb - 1.0;
    float alpha = texture(DiffuseForAlpha, uv).a;
    // Because of interpolation, we need to renormalize
    vec3 Frag_tangent = normalize(tangent);
    vec3 Frag_normal = normalize(cross(Frag_tangent, bitangent));
    vec3 Frag_bitangent = cross(Frag_normal, Frag_tangent);

    vec3 FragmentNormal = TS_normal.x * Frag_tangent + TS_normal.y * Frag_bitangent - TS_normal.z * Frag_normal;
    EncodedNormal.xy = 0.5 * EncodeNormal(normalize(FragmentNormal)) + 0.5;
    EncodedNormal.z = exp2(10. * (1. - alpha) + 1.);
}
