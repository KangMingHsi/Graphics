#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 inNormals;
layout(location = 2) in vec2 inTexcoord;

uniform mat4 um4mvp;
uniform int mode;

out VS_OUT{
	vec3 normals;
	vec2 texcoord;
}vs_out;

void main()
{
	if(mode < 1){
		gl_Position = um4mvp * vec4(iv3vertex, 1.0);
		vs_out.normals = inNormals;
	}else{
		gl_Position = um4mvp * vec4(vec3(iv3vertex.x, iv3vertex.z + 9.0f, iv3vertex.y), 1.0);
		vs_out.normals = vec3(inNormals.xzy);
	}
	vs_out.texcoord = inTexcoord;
}