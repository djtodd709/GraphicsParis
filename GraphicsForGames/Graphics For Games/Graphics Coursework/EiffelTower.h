#pragma once
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/OGLRenderer.h"

#define START_DELAY 4.0f
#define BEAM_DELAY 10.0f
#define CROSS_DELAY 12.0f
#define ARCH_DELAY 15.0f
#define STAND_DELAY 18.0f

#define FINISH_DELAY 35.0f

class EiffelTower : public SceneNode {
public:
	EiffelTower(void);
	~EiffelTower(void);
	virtual void Update(float msec);

	void RestartBuild();

protected:
	Shader* towerShader;

	Vector3 upDir = Vector3(0.0f, 0.1f, 0.0f);
	Vector3 sideDir = Vector3(-0.2f, 0.0f, 0.2f);

	float timeElapsed;
	bool firstTime;

	bool towerStages[5] = { false, false, false, false, false };

	Mesh* full;
	Mesh* poles;
	Mesh* beams;
	Mesh* crosses;
	Mesh* arches;
	Mesh* stand;
	SceneNode* polesRoot;
	SceneNode* beamsRoot;
	SceneNode* crossesRoot;
	SceneNode* archesRoot;
	SceneNode* standRoot;

	SceneNode* polesNode[4];
	SceneNode* beamsNode[4];
	SceneNode* crossesNode[4];
	SceneNode* archesNode[4];
	SceneNode* standNode[4];

	void CreateMeshes() {

		GLuint t = SOIL_load_OGL_texture(CWTEXTUREDIR"Metal.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
		GLuint b = SOIL_load_OGL_texture(CWTEXTUREDIR"MetalNormal.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

		glBindTexture(GL_TEXTURE_2D, t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glBindTexture(GL_TEXTURE_2D, b);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(CWMESHDIR"Eiffel/eiffelfull.obj");
		m->SetTexture(t);
		full = m;

		m = new OBJMesh();
		m->LoadOBJMesh(CWMESHDIR"Eiffel/eiffelPoles.obj");
		m->SetTexture(t);
		poles = m;

		m = new OBJMesh();
		m->LoadOBJMesh(CWMESHDIR"Eiffel/eiffelBeams.obj");
		m->SetTexture(t);
		beams = m;

		m = new OBJMesh();
		m->LoadOBJMesh(CWMESHDIR"Eiffel/eiffelCrosses.obj");
		m->SetTexture(t);
		crosses = m;

		m = new OBJMesh();
		m->LoadOBJMesh(CWMESHDIR"Eiffel/eiffelArches.obj");
		m->SetTexture(t);
		arches = m;

		m = new OBJMesh();
		m->LoadOBJMesh(CWMESHDIR"Eiffel/eiffelfull.obj");
		m->SetTexture(t);
		stand = m;
	}
	void DeleteMeshes() {
		delete full;
		delete poles;
		delete beams;
		delete crosses;
		delete arches;
	}
};