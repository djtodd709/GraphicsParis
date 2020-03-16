#pragma once
#include "OGLRenderer.h"

#define SHADER_VERTEX 0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2
#define SHADER_TESSCONTROL 3
#define SHADER_TESSEVAL 4

using namespace std;

class Shader
{
public:
	Shader(string vertex, string fragment, string geometry = "", string control = "", string eval = "");
	~Shader(void);

	GLuint GetProgram() { return program; }
	bool LinkProgram();

	bool GetLoadFailed() { return loadFailed; }
protected:
	void SetDefaultAttributes();
	bool LoadShaderFile(string from, string& into);
	GLuint GenerateShader(string from, GLenum type);

	GLuint objects[5];
	GLuint program;

	bool loadFailed;
};

