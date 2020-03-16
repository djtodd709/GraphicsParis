#include "EiffelTower.h"

EiffelTower::EiffelTower(void) {
	SetBoundingRadius(3000.0f);

	timeElapsed = 0.0f;

	towerShader = new Shader(CWSHADERDIR"EiffelVertex.glsl", CWSHADERDIR"EiffelFragment.glsl");

	if (!towerShader->LinkProgram()) {
		cout << "Error: could not link tower shader!" << endl;
		return;
	}

	SetShader(towerShader);
	CreateMeshes();

	SetUniformVec3("fadedir", upDir);
	SetTransform(Matrix4::Rotation(45, Vector3(0, 1, 0)));

	polesRoot = new SceneNode();
	AddChild(polesRoot);
	beamsRoot = new SceneNode();
	AddChild(beamsRoot);
	beamsRoot->SetUniformVec3("fadedir", sideDir);
	crossesRoot = new SceneNode();
	AddChild(crossesRoot);
	archesRoot = new SceneNode();
	AddChild(archesRoot);
	archesRoot->SetUniformVec3("fadedir", sideDir);
	standRoot = new SceneNode();
	AddChild(standRoot);


	for (int i = 0; i < 4; ++i) {
		polesNode[i] = new SceneNode(poles,Vector4(1,0.9f,0.8f, 1));
		polesNode[i]->SetTransform(Matrix4::Rotation(90 * i, Vector3(0, 1, 0)));
		polesNode[i]->SetModelScale(Vector3(10.0f, 10.0f, 10.0f));
		polesRoot->AddChild(polesNode[i]);

		beamsNode[i] = new SceneNode(beams, Vector4(1, 0.9f, 0.8f, 1));
		beamsNode[i]->SetTransform(Matrix4::Rotation(90 * i, Vector3(0, 1, 0)));
		beamsNode[i]->SetModelScale(Vector3(9.99f, 9.99f, 9.99f));
		beamsRoot->AddChild(beamsNode[i]);

		crossesNode[i] = new SceneNode(crosses, Vector4(1, 0.9f, 0.8f, 1));
		crossesNode[i]->SetTransform(Matrix4::Rotation(90 * i, Vector3(0, 1, 0)));
		crossesNode[i]->SetModelScale(Vector3(9.98f, 9.98f, 9.98f));
		crossesRoot->AddChild(crossesNode[i]);

		archesNode[i] = new SceneNode(arches, Vector4(1, 0.9f, 0.8f, 1));
		archesNode[i]->SetTransform(Matrix4::Rotation(90 * i, Vector3(0, 1, 0)));
		archesNode[i]->SetModelScale(Vector3(9.97f, 9.97f, 9.97f));
		archesRoot->AddChild(archesNode[i]);

		standNode[i] = new SceneNode(stand, Vector4(1, 0.9f, 0.8f, 1));
		standNode[i]->SetTransform(Matrix4::Rotation(90 * i, Vector3(0, 1, 0)));
		standNode[i]->SetModelScale(Vector3(10.01f, 10.01f, 10.01f));
		standRoot->AddChild(standNode[i]);
	}
	
	beamsRoot->SetActive(false);
	crossesRoot->SetActive(false);
	archesRoot->SetActive(false);
	standRoot->SetActive(false);

	firstTime = true;
}

EiffelTower::~EiffelTower() {
	delete towerShader;
	DeleteMeshes();
}

void EiffelTower::RestartBuild() {
	timeElapsed = 0.0f;
	for (int i = 0; i < 5; ++i) {
		towerStages[i] = false;
	}
	polesRoot->SetActive(true);
	beamsRoot->SetActive(false);
	crossesRoot->SetActive(false);
	archesRoot->SetActive(false);
	standRoot->SetActive(false);
}

void EiffelTower::Update(float msec) {
	timeElapsed += msec * 0.001f;
	if (firstTime) {
		timeElapsed = 0.0f;
		firstTime = false;
	}

	polesRoot->SetUniformFloat("time", timeElapsed-START_DELAY);
	beamsRoot->SetUniformFloat("time", timeElapsed-BEAM_DELAY-7.0f);
	crossesRoot->SetUniformFloat("time", timeElapsed- CROSS_DELAY-2.0f);
	archesRoot->SetUniformFloat("time", timeElapsed- ARCH_DELAY - 5.0f);
	standRoot->SetUniformFloat("time", timeElapsed - STAND_DELAY);

	if (timeElapsed > BEAM_DELAY && !towerStages[0]) {
		towerStages[0] = true;
		beamsRoot->SetActive(true);
	}
	if (timeElapsed > CROSS_DELAY && !towerStages[1]) {
		towerStages[1] = true;
		crossesRoot->SetActive(true);
	}
	if (timeElapsed > ARCH_DELAY && !towerStages[2]) {
		towerStages[2] = true;
		archesRoot->SetActive(true);
	}
	if (timeElapsed > STAND_DELAY && !towerStages[3]) {
		towerStages[3] = true;
		standRoot->SetActive(true);
	}
	if (timeElapsed > FINISH_DELAY && !towerStages[4]) {
		towerStages[4] = true;
		beamsRoot->SetActive(false);
		crossesRoot->SetActive(false);
		archesRoot->SetActive(false);
		polesRoot->SetActive(false);
	}
	SceneNode::Update(msec);
}

