#version 410

in VS_OUT{
	vec3 normals;
	vec2 texcoord;
}fs_in;

layout(location = 0) out vec4 fragColor;
uniform sampler2D s;

void main()
{
	fragColor = texture(s, fs_in.texcoord);
}