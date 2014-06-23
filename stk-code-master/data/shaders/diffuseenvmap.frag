uniform float blueLmn[9];
uniform float greenLmn[9];
uniform float redLmn[9];
uniform sampler2D ntex;
uniform mat4 TransposeViewMatrix;
layout (std140) uniform MatrixesData
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 InverseViewMatrix;
    mat4 InverseProjectionMatrix;
    mat4 ShadowViewProjMatrixes[4];
    vec2 screen;
};

out vec4 Diff;

vec3 DecodeNormal(vec2 n);

mat4 getMatrix(float L[9])
{
    float c1 = 0.429043, c2 = 0.511664, c3 = 0.743125, c4 = 0.886227, c5 = 0.247708;

    return mat4(
        c1 * L[8] /*L22*/, c1 * L[4] /*L2-2*/, c1 * L[7] /*L21*/, c2 * L[3] /*L11*/,
        c1 * L[4], - c1 * L[8], c1 * L[5] /*L2-1*/, c2 * L[1] /*L1-1*/,
        c1 * L[7], c1 * L[5], c3 * L[6] /*L20*/, c2 * L[2] /*L10*/,
        c2 * L[3], c2 * L[1], c2 * L[2], c4 * L[0] /*L00*/ - c5 * L[6]
    );
}

void main(void)
{
    vec2 uv = gl_FragCoord.xy / screen;
    vec3 normal = normalize(DecodeNormal(2. * texture(ntex, uv).xy - 1.));
    // Convert normal in world space (where SH coordinates were computed)
    vec4 extendednormal = TransposeViewMatrix * vec4(normal, 1.);
    extendednormal.w = 1.;
    mat4 rmat = getMatrix(redLmn);
    mat4 gmat = getMatrix(greenLmn);
    mat4 bmat = getMatrix(blueLmn);

    float r = dot(extendednormal, rmat * extendednormal);
    float g = dot(extendednormal, gmat * extendednormal);
    float b = dot(extendednormal, bmat * extendednormal);

    Diff = max(0.25 * vec4(r, g, b, .1), vec4(0.));
}
