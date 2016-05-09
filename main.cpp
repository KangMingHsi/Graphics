#include <glew.h>
#include <freeglut.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <tiny_obj_loader.h>
#include "fbxloader.h"
#include <IL/il.h>

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3

GLubyte timer_cnt = 0;
bool timer_enabled = true, isRotate = true, lookAround = false;
unsigned int timer_speed = 16, *indiceCount1, *indiceCount2, *indiceCount3;
long long shapeCount1, shapeCount2, shapeCount3;
int *materials_ID1, *materials_ID2, *materials_ID3,whichAnim = 0, state;
using namespace glm;

float rotation = 0.0f, viewportAspect, anim;
fbx_handles myFbx, myFbx1, myFbx2;
vec3 cameraPos, cameraFront, cameraUp;
vec3 recordPos, recordFront, recordUp;

mat4 mvp;
GLint um4mvp, umSampler, umMode;
GLuint *tex_array1, *vao1, *buffer1, *indice_buffer1;
GLuint *tex_array2, *vao2, *buffer2, *indice_buffer2;
GLuint *tex_array3, *vao3, *buffer3, *indice_buffer3;
GLfloat lastX = 600.0f, lastY = 600.0f, m_yaw = -90.0f, m_pitch = 0.0f;

void checkError(const char *functionName)
{
    GLenum error;
    while (( error = glGetError() ) != GL_NO_ERROR) {
        fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
    }
}

void watch(){
	switch(state){
	case 0:
		cameraPos = vec3(1341.0f, 495.0f, -32.4f);
		cameraFront = vec3(-1,0,0);
		state = 1;
		break;
	case 1:
		cameraPos.x -= 4.0f;
		if(cameraPos.x < 1243.0f)
			state = 2;
		break;
	case 2:
		if(cameraFront.z > -1.0f)
			cameraFront.z -= 0.1f;
		else{
			cameraFront.x += 0.1f;
			if(cameraFront.x > 0.0f){
				cameraFront = vec3(0,0,-1);
				state = 3;
			}
		}
		break;
	case 3:
		cameraPos.z -= 4.0f;
		if(cameraPos.z < -390.0f)
			state = 4;
		break;
	case 4:
		if(cameraFront.x > -1.0f)
			cameraFront.x -= 0.1f;
		else{
			cameraFront.z += 0.1f;
			if(cameraFront.z > 0.0f){
				cameraFront = vec3(-1,0,0);
				state = 5;
			}
		}
		break;
	case 5:
		cameraPos.x -= 4.0f;
		if(cameraPos.x < -1100.0f)
			state = 6;
		break;
	case 6:
		if(cameraFront.z < 1.0f)
			cameraFront.z += 0.1f;
		else{
			cameraFront.x += 0.1f;
			if(cameraFront.x > 0.0f){
				cameraFront = vec3(0,0,1);
				state = 7;
			}
		}
		break;
	case 7:
		cameraPos.z += 4.0f;
		if(cameraPos.z > 360.0f)
			state = 8;
		break;
	case 8:
		if(cameraFront.x < 1.0f)
			cameraFront.x += 0.1f;
		else{
			cameraFront.z -= 0.1f;
			if(cameraFront.z < 0.0f){
				cameraFront = vec3(1,0,0);
				state = 9;
			}
		}
		break;
	case 9:
		cameraPos.x += 4.0f;
		if(cameraPos.x > 1143.0f)
			state = 10;
		break;
	case 10:
		if(cameraFront.z > -1.0f)
			cameraFront.z -= 0.1f;
		else{
			cameraFront.x -= 0.1f;
			if(cameraFront.x < 0.0f){
				cameraFront = vec3(0,0,-1);
				state = 11;
			}
		}
	case 11:
		cameraPos.z -= 2.0f;
		if(cameraPos.z < -32.4f)
			state = 12;
		break;
	case 12:
		if(cameraFront.x > -1.0f)
			cameraFront.x -= 0.1f;
		else{
			cameraFront.z += 0.1f;
			if(cameraFront.z > 0.0f){
				cameraFront = vec3(-1,0,0);
				state = 13;
			}
		}
		break;
	case 13:
		cameraPos.x -= 4.0f;
		if(cameraPos.x < 700.0f)
			state = 14;
		break;
	case 14:
		if(cameraFront.y > -0.89f)
			cameraFront.y -= 0.1f;
		else{
			cameraFront.x += 0.1f;
			if(cameraFront.x > -0.11f){
				cameraFront = vec3(0,-0.9,-0.1);
				state = 15;
			}
		}
		break;
	case 15:
		cameraPos.y -= 4.0f;
		if(cameraPos.y < 24.0f)
			state = 16;
		break;
	case 16:
		if(cameraFront.x > -1.0f)
			cameraFront.x -= 0.1f;
		else{
			cameraFront.y += 0.1f;
			if(cameraFront.y > 0.0f){
				cameraFront = vec3(-1,0,0);
				state = 17;
			}
		}
		break;
	case 17:
		cameraPos.x -= 4.0f;
		if(cameraPos.x < -1100.0f){
			state = 18;
			rotation = 0.0f;
		}
		break;
	case 18:
		rotation += 1.0f;
		cameraPos = vec3(-74.0f * cos(radians(rotation)), cameraPos.y, -32.4f * sin(radians(rotation)));
		cameraFront = normalize(vec3(0.0f - cameraPos.x, 0.0f, 0.0f - cameraPos.z));
		 
		if(rotation > 360.0f){
			rotation = 0.0f;
			state = -1;
		}
		break;
	case -1:
		cameraPos =recordPos;
		cameraFront = recordFront;
		lookAround = false;
		break;
	}
}
// Print OpenGL context related information.
void dumpInfo(void)
{
    printf("Vendor: %s\n", glGetString (GL_VENDOR));
    printf("Renderer: %s\n", glGetString (GL_RENDERER));
    printf("Version: %s\n", glGetString (GL_VERSION));
    printf("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
}

char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

void freeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete errorLog;
	}
}

void My_Init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    GLuint program = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");
	
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
	shaderLog(vertexShader);
    shaderLog(fragmentShader);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

	umMode = glGetUniformLocation(program, "mode");
	um4mvp = glGetUniformLocation(program, "um4mvp");
	umSampler = glGetUniformLocation(program, "s");
    glUseProgram(program);

	// TODO: glEnableVertexAttribArray Calls For Your Shader Program Here
	cameraPos = vec3(0.0f,0.0f, 50.0f);
	cameraFront = vec3(0.0f,0.0f, -1.0f);
	cameraUp = vec3(0.0f,1.0f, 0.0f);
}

std::vector<tinyobj::shape_t> shapes2;

void My_LoadModels()
{
	std::vector<tinyobj::shape_t> shapes1;
	std::vector<tinyobj::material_t> materials1;
	std::string err1;

	std::vector<tinyobj::material_t> materials2;
	std::string err2;

	std::vector<tinyobj::shape_t> shapes3;
	std::vector<tinyobj::material_t> materials3;
	std::string err3;
	
	bool ret2 = tinyobj::LoadObj(shapes2, materials2, err2, "sponza.obj");
	// TODO: If You Want to Load FBX, Use these. The Returned Values are The Same.
	 // Save this Object, You Will Need It to Retrieve Animations Later.
	bool ret1 = LoadFbx(myFbx1, shapes1, materials1, err1, "zombie_walk.FBX");
	bool ret3 = LoadFbx(myFbx2, shapes3, materials3, err3, "zombie_fury.FBX");
	
	if(ret3)
	{
		shapeCount3 = shapes3.size();
		tex_array3 = new GLuint[materials3.size()];
		buffer3 = new GLuint[shapeCount3*3];
		indice_buffer3 = new GLuint[shapeCount3];
		vao3 = new GLuint[shapeCount3];
		
		//vao = new GLuint[shapeCount];
		indiceCount3 = new unsigned int[shapeCount3];
		materials_ID3 = new int[shapeCount3];
		glGenTextures(materials3.size(), &tex_array3[0]);

		// For Each Material
		for(int i = 0; i < materials1.size(); i++)
		{
			// materials[i].diffuse_texname; // This is the Texture Path You Need
			//puts("ss");
			ILuint ilTexName;
			ilGenImages(1, &ilTexName);
			ilBindImage(ilTexName);
			if(ilLoadImage(materials1[i].diffuse_texname.c_str()))
			{
				unsigned char *data = new unsigned char[ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 3];
				int Width = ilGetInteger(IL_IMAGE_WIDTH);
				int Height = ilGetInteger(IL_IMAGE_HEIGHT);
				ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, data);
				// TODO: Generate an OpenGL Texture and use the [unsigned char *data] as Input Here.
				glBindTexture(GL_TEXTURE_2D, tex_array3[i]);
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				
				delete[] data;
				ilDeleteImages(1, &ilTexName);
			}
		}
		//printf("%d %d\n", materials3.size(), shapes3.size());
		glGenVertexArrays(shapeCount3, vao3);
		// For Each Shape (or Mesh, Object)
		for(int i = 0; i < shapes3.size(); i++)
		{
			glBindVertexArray(vao3[i]);

			glGenBuffers(1, &buffer3[3*i]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer3[3*i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes3[i].mesh.positions.size(), 
				shapes3[i].mesh.positions.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			glGenBuffers(1, &buffer3[3*i+1]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer3[3*i+1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes3[i].mesh.normals.size(), 
				shapes3[i].mesh.normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
			
			glGenBuffers(1, &buffer3[3*i+2]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer3[3*i+2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes3[i].mesh.texcoords.size(), 
				shapes3[i].mesh.texcoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			glGenBuffers(1, &indice_buffer3[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_buffer3[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*shapes3[i].mesh.indices.size(), 
				shapes3[i].mesh.indices.data(), GL_STATIC_DRAW);

			indiceCount3[i] = shapes3[i].mesh.indices.size();
			materials_ID3[i] = shapes3[i].mesh.material_ids[0];
		}
	}
	if(ret1)
	{
		shapeCount1 = shapes1.size();
		tex_array1 = new GLuint[materials1.size()];
		buffer1 = new GLuint[shapeCount1*3];
		indice_buffer1 = new GLuint[shapeCount1];
		vao1 = new GLuint[shapeCount1];
		
		//vao = new GLuint[shapeCount];
		indiceCount1 = new unsigned int[shapeCount1];
		materials_ID1 = new int[shapeCount1];
		glGenTextures(materials1.size(), &tex_array1[0]);

		// For Each Material
		for(int i = 0; i < materials1.size(); i++)
		{
			// materials[i].diffuse_texname; // This is the Texture Path You Need
			//puts("ss");
			ILuint ilTexName;
			ilGenImages(1, &ilTexName);
			ilBindImage(ilTexName);
			if(ilLoadImage(materials1[i].diffuse_texname.c_str()))
			{
				unsigned char *data = new unsigned char[ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 3];
				int Width = ilGetInteger(IL_IMAGE_WIDTH);
				int Height = ilGetInteger(IL_IMAGE_HEIGHT);
				ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, data);
				// TODO: Generate an OpenGL Texture and use the [unsigned char *data] as Input Here.
				glBindTexture(GL_TEXTURE_2D, tex_array1[i]);
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				
				delete[] data;
				ilDeleteImages(1, &ilTexName);
			}
		}
		//printf("%d %d\n", materials1.size(), shapes1.size());
		glGenVertexArrays(shapeCount1, vao1);
		// For Each Shape (or Mesh, Object)
		for(int i = 0; i < shapes1.size(); i++)
		{
			// shapes[i].mesh.positions; // VertexCount * 3 Floats, Load Them to a GL_ARRAY_BUFFER
			// shapes[i].mesh.normals; // VertexCount * 3 Floats, Load Them to a GL_ARRAY_BUFFER
			// shapes[i].mesh.texcoords; // VertexCount * 2 Floats, Load Them to a GL_ARRAY_BUFFER
			// shapes[i].mesh.indices; // TriangleCount * 3 Unsigned Integers, Load Them to a GL_ELEMENT_ARRAY_BUFFER
			// shapes[i].mesh.material_ids[0] // The Material ID of This Shape

			// TODO: 
			// 1. Generate and Bind a VAO
			// 2. Generate and Bind a Buffer for position/normal/texcoord
			// 3. Upload Data to The Buffers
			// 4. Generate and Bind a Buffer for indices (Will Be Saved In The VAO, You Can Restore Them By Binding The VAO)
			// 5. glVertexAttribPointer Calls (Will Be Saved In The VAO, You Can Restore Them By Binding The VAO)
			
			glBindVertexArray(vao1[i]);

			glGenBuffers(1, &buffer1[3*i]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer1[3*i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes1[i].mesh.positions.size(), 
				shapes1[i].mesh.positions.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			glGenBuffers(1, &buffer1[3*i+1]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer1[3*i+1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes1[i].mesh.normals.size(), 
				shapes1[i].mesh.normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
			
			glGenBuffers(1, &buffer1[3*i+2]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer1[3*i+2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes1[i].mesh.texcoords.size(), 
				shapes1[i].mesh.texcoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			glGenBuffers(1, &indice_buffer1[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_buffer1[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*shapes1[i].mesh.indices.size(), 
				shapes1[i].mesh.indices.data(), GL_STATIC_DRAW);

			indiceCount1[i] = shapes1[i].mesh.indices.size();
			materials_ID1[i] = shapes1[i].mesh.material_ids[0];
		}
	}

	if(ret2)
	{
		//puts("aa");
		
		shapeCount2 = shapes2.size();
		tex_array2 = new GLuint[materials2.size()];
		buffer2 = new GLuint[shapeCount2*3];
		indice_buffer2 = new GLuint[shapeCount2];
		vao2 = new GLuint[shapeCount2];
		
		//vao = new GLuint[shapeCount];
		indiceCount2 = new unsigned int[shapeCount2];
		materials_ID2 = new int[shapeCount2];
		glGenTextures(materials2.size(), &tex_array2[0]);

		// For Each Material
		for(int i = 0; i < materials2.size(); i++)
		{
			// materials[i].diffuse_texname; // This is the Texture Path You Need
			//puts("ss");
			ILuint ilTexName;
			ilGenImages(1, &ilTexName);
			ilBindImage(ilTexName);
			if(ilLoadImage(materials2[i].diffuse_texname.c_str()))
			{
				unsigned char *data = new unsigned char[ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 3];
				int Width = ilGetInteger(IL_IMAGE_WIDTH);
				int Height = ilGetInteger(IL_IMAGE_HEIGHT);
				ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, data);

				// TODO: Generate an OpenGL Texture and use the [unsigned char *data] as Input Here.
				glBindTexture(GL_TEXTURE_2D, tex_array2[i]);
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				
				delete[] data;
				ilDeleteImages(1, &ilTexName);
			}
		}
		//printf("%d %d\n", materials2.size(), shapes2.size());
		glGenVertexArrays(shapeCount2, vao2);
		// For Each Shape (or Mesh, Object)
		for(int i = 0; i < shapes2.size(); i++)
		{
			// shapes[i].mesh.positions; // VertexCount * 3 Floats, Load Them to a GL_ARRAY_BUFFER
			// shapes[i].mesh.normals; // VertexCount * 3 Floats, Load Them to a GL_ARRAY_BUFFER
			// shapes[i].mesh.texcoords; // VertexCount * 2 Floats, Load Them to a GL_ARRAY_BUFFER
			// shapes[i].mesh.indices; // TriangleCount * 3 Unsigned Integers, Load Them to a GL_ELEMENT_ARRAY_BUFFER
			// shapes[i].mesh.material_ids[0] // The Material ID of This Shape

			// TODO: 
			// 1. Generate and Bind a VAO
			// 2. Generate and Bind a Buffer for position/normal/texcoord
			// 3. Upload Data to The Buffers
			// 4. Generate and Bind a Buffer for indices (Will Be Saved In The VAO, You Can Restore Them By Binding The VAO)
			// 5. glVertexAttribPointer Calls (Will Be Saved In The VAO, You Can Restore Them By Binding The VAO)
			
			glBindVertexArray(vao2[i]);

			glGenBuffers(1, &buffer2[3*i]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer2[3*i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes2[i].mesh.positions.size(), 
				shapes2[i].mesh.positions.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			glGenBuffers(1, &buffer2[3*i+1]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer2[3*i+1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes2[i].mesh.normals.size(), 
				shapes2[i].mesh.normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
			
			glGenBuffers(1, &buffer2[3*i+2]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer2[3*i+2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes2[i].mesh.texcoords.size(), 
				shapes2[i].mesh.texcoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			glGenBuffers(1, &indice_buffer2[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_buffer2[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*shapes2[i].mesh.indices.size(), 
				shapes2[i].mesh.indices.data(), GL_STATIC_DRAW);

			indiceCount2[i] = shapes2[i].mesh.indices.size();
			materials_ID2[i] = shapes2[i].mesh.material_ids[0];
			//printf("~%d\n", shapes2[i].mesh.material_ids.size());
		}
	}
}

void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(lookAround){
		watch();
	}
	
	mvp = perspective(45.0f, viewportAspect, 0.1f, 3000.0f) * 
		lookAt(cameraPos, 
		cameraPos + cameraFront, cameraUp); 
	
	std::vector<tinyobj::shape_t> new_shapes;
	
	if(anim < 1) anim += 0.01f;
	else anim = 0.0f;
	
	if(whichAnim == 0) myFbx = myFbx1;
	else myFbx = myFbx2;

	glUniform1i(umMode,1);
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));

	GetFbxAnimation(myFbx, new_shapes, anim); // The Last Parameter is A Float in [0, 1], Specifying The Animation Location You Want to Retrieve
	if(whichAnim){
		for(int i = 0; i < new_shapes.size(); i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER,buffer1[3*i]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*new_shapes[i].mesh.positions.size(), new_shapes[i].mesh.positions.data());
		}

		for(int i = 0; i < shapeCount1; i++)
		{
			glBindVertexArray(vao1[i]);
			glBindTexture(GL_TEXTURE_2D, tex_array1[materials_ID1[i]]);
			glDrawElements(GL_TRIANGLES, indiceCount1[i], GL_UNSIGNED_INT, 0);
		}
	}else{
		for(int i = 0; i < new_shapes.size(); i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER,buffer3[3*i]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*new_shapes[i].mesh.positions.size(), new_shapes[i].mesh.positions.data());
		}
		for(int i = 0; i < shapeCount3; i++)
		{
			glBindVertexArray(vao3[i]);
			glBindTexture(GL_TEXTURE_2D, tex_array3[materials_ID3[i]]);
			glDrawElements(GL_TRIANGLES, indiceCount3[i], GL_UNSIGNED_INT, 0);
		}
	}
	glUniform1i(umMode,0);
	for(int i = 0; i < shapeCount2; i++)
	{
		glBindVertexArray(vao2[i]);
		int j = 0;
		while(j < shapes2[i].mesh.material_ids.size())
		{
			glBindTexture(GL_TEXTURE_2D, tex_array2[shapes2[i].mesh.material_ids[j]]);
			
			int count = 0;
			while(((j+count+1) < shapes2[i].mesh.material_ids.size()) && 
				shapes2[i].mesh.material_ids[j+count] == shapes2[i].mesh.material_ids[j+count+1]){
				count++;
			}
			glDrawElements(GL_TRIANGLES, (count+1)*3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)*j*3));
			
			j = j + count + 1;
			
		}
		
	}
	
	glGetError();
    glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	viewportAspect = (float)width/ (float)height;
}

void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if(timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}

void MotionMouse(int x, int y)
{
	if(!lookAround){
	   if(isRotate){
			lastX = x;
			lastY = y;
			isRotate = false;
		}
		GLfloat xoffset = lastX - x;
		GLfloat yoffset = lastY - y;
		lastX = x;
		lastY = y;
	
		GLfloat sensitivity = 1.0f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		m_yaw += xoffset;
		m_pitch += yoffset;

		if(m_pitch > 89.0f)
			m_pitch = 89.0f;
		if(m_pitch < -89.0f)
			m_pitch = -89.0f;

		vec3 front = vec3(cos(radians(m_yaw)) * cos(radians(m_pitch)), sin(-radians(m_pitch)), sin(radians(m_yaw)) * cos(radians(m_pitch)));
		cameraFront = normalize(cameraFront + front);
		//glutPostRedisplay();
	}	
}

void My_Mouse(int button, int state, int x, int y)
{
	if(state == GLUT_UP){
		isRotate = true;
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	//printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	if(!lookAround)
	{
		switch(key)
		{
		case 'M':
		case 'm':
			if(!lookAround){
				state = 0;
				recordPos = cameraPos;
				recordFront = cameraFront;
				recordUp = cameraUp;
				lookAround = true;
			}
			break;
		case 'O':
		case 'o':
			whichAnim = 0;
			anim = 0;
			break;
		case 'P':
		case 'p':
			anim = 0;
			whichAnim = 1;
			break;
		case 'W':
		case 'w':
			cameraPos += 2.0f * cameraFront;
			break;
		case 'S':
		case 's':
			cameraPos -= 2.0f * cameraFront;
			break;
		case 'A':
		case 'a':
			cameraPos -= normalize(cross(cameraFront,cameraUp)) * 2.0f;
			break;
		case 'D':
		case 'd':
			cameraPos += normalize(cross(cameraFront,cameraUp)) * 2.0f;
			break;
		case 'Z':
		case 'z':
			cameraPos += cameraUp * 2.0f;
			break;
		case 'X':
		case 'x':
			cameraPos -= cameraUp * 2.0f;
			break;
		default:
			break;
		}
		printf("(%f %f %f)\n", cameraPos.x,cameraPos.y, cameraPos.z);
	}
	glutPostRedisplay();
}

void My_SpecialKeys(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id)
{
	switch(id)
	{
	case MENU_TIMER_START:
		if(!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Assignment 02 103062212"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	glewInit();
	ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	dumpInfo();
	My_Init();
	My_LoadModels();
	////////////////////

	// Create a menu and bind it to mouse right button.
	////////////////////////////
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////////////////////////////

	// Register GLUT callback functions.
	///////////////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0); 
	glutMotionFunc(MotionMouse);
	///////////////////////////////

	// Enter main event loop.
	//////////////
	glutMainLoop();
	//////////////
	return 0;
}