in vec2 Position;
in vec2 Texcoord;

#ifndef VSLayer
out int layer;
out vec2 uv_in;
#else
out vec2 uv;
flat out int slice;
#endif



void main() {
#ifdef VSLayer
    gl_Layer = gl_InstanceID;
    uv = Texcoord;
    slice = gl_InstanceID;
#else
    layer = gl_InstanceID;
    uv_in = Texcoord;
#endif
    gl_Position = vec4(Position, 0., 1.);
}
