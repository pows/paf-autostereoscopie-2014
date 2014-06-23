uniform sampler2D ntex;
uniform sampler2D dtex;
//uniform sampler2D cloudtex;

uniform vec3 direction;
uniform vec3 col;
uniform mat4 invproj;
//uniform int hasclouds;
//uniform vec2 wind;

#ifdef UBO_DISABLED
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 InverseViewMatrix;
uniform mat4 InverseProjectionMatrix;
uniform vec2 screen;
#else
layout (std140) uniform MatrixesData
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 InverseViewMatrix;
    mat4 InverseProjectionMatrix;
    mat4 ShadowViewProjMatrixes[4];
    vec2 screen;
};
#endif

out vec4 Diff;
out vec4 Spec;

vec3 DecodeNormal(vec2 n);
vec3 getSpecular(vec3 normal, vec3 eyedir, vec3 lightdir, vec3 color, float roughness);
vec4 getPosFromUVDepth(vec3 uvDepth, mat4 InverseProjectionMatrix);

void main() {
    vec2 uv = gl_FragCoord.xy / screen;
	float z = texture(dtex, uv).x;
	vec4 xpos = getPosFromUVDepth(vec3(uv, z), InverseProjectionMatrix);

	if (z < 0.03)
	{
		// Skyboxes are fully lit
		Diff = vec4(1.0);
		Spec = vec4(1.0);
		return;
	}

	vec3 norm = normalize(DecodeNormal(2. * texture(ntex, uv).xy - 1.));
    float roughness = texture(ntex, uv).z;
    vec3 eyedir = -normalize(xpos.xyz);

	// Normalized on the cpu
    vec3 L = direction;

    float NdotL = max(0., dot(norm, L));

    vec3 Specular = getSpecular(norm, eyedir, L, col, roughness) * NdotL;

	vec3 outcol = NdotL * col;

/*	if (hasclouds == 1)
	{
		vec2 cloudcoord = (xpos.xz * 0.00833333) + wind;
		float cloud = texture(cloudtex, cloudcoord).x;
		//float cloud = step(0.5, cloudcoord.x) * step(0.5, cloudcoord.y);

		outcol *= cloud;
	}*/

	Diff = vec4(NdotL * col, 1.);
	Spec = vec4(Specular, 1.);
}
