#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/DirLight.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SplineTrack.h"
#include "EiffelTower.h"
#include "Portal.h"
#include <algorithm>

#define SHADOWSIZE 8192
#define POST_PASSES 20

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

	void SetScene(bool b);
	void RestartEiffel();
	void RestartLouvre(bool instant);
	void SetCameraFree(bool b);
	void GoToScene(int i);

protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes(bool savePortal);
	void DrawNode(SceneNode* n);
	bool LoadMeshes();

	void InitBuildings();

	void DrawSkybox(bool b);

	void DrawShadowScene();
	void DrawCombinedScene();
	void DrawPhysicals(bool shadows);

	void PresentScene();
	void DrawPortal();
	void DrawBloom();
	void DrawPostProcess();
	void DrawScene();

	void CreatePath();

	int frames = 0;
	float acc = 0.0f;

	GLuint shadowTex;
	GLuint shadowFBO;

	Light* pointLights;
	Mesh* sphere;
	Mesh* quad;
	Mesh* towerLights;

	Mesh* stand;

	bool freeCam;

	SplineTrack path;
	SplineTrack tPath;
	float timePassed;


	float eiffelTimer;
	float grassTimer;
	bool lightsOn;
	SceneNode* eiffelLights;

	bool firstTime;

	SceneNode* grass;
	SceneNode* louvreRoot;
	SceneNode* louvreInner;

	Vector3 positions[49];
	Vector4 colours[49];
	float radii[49];
	float radiiToUse[49];
	float zeroes[49] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	bool eiffelScene;

	SceneNode* altRoot;
	SceneNode* root;
	SceneNode* activeRoot;
	SceneNode* inactiveRoot;

	SceneNode* portalBuilding;
	SceneNode* outerBuidlings;

	SceneNode* portalStand;

	SceneNode* waterNode;

	Camera* camera;
	Mesh* skyQuad;

	Mesh* floor;
	Mesh* waterMesh;
	Mesh* paveMesh;

	Mesh* lampost;

	Mesh* heightMap;
	
	Mesh* building;
	Mesh* lowQbuilding;
	Mesh* ulowQbuilding;

	Shader* md5Shader;
	MD5FileData* womanData;

	EiffelTower* eifT;

	Portal* portalNode;
	Portal* altPortalNode;
	Portal* activePortalNode;

	Mesh* louvreShell;
	Mesh* louvreGlass;
	Mesh* archway;
	Mesh* portalMesh;

	DirLight* moonLight;
	DirLight* eveLight;

	Shader* sceneShader;
	Shader* sceneShaderBump;
	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* displaceShader;

	Shader* grassShader;

	Shader* lightShader;
	Shader* extractShader;
	Shader* extractBlueShader;
	Shader* portalShader;
	Shader* processShader;
	Shader* presentShader;

	GLuint cubeMap;
	GLuint brightCubeMap;

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint extractFBO;
	GLuint portalFBO;
	GLuint bufferColourTex[5];
	GLuint bufferDepthTex;
	GLuint portalDepthTex;
};

