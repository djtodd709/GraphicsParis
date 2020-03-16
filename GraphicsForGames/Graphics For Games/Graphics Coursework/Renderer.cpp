#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera = new Camera();
	camera->SetPosition(Vector3(100, 30, 100));
	camera->setFreeLook(false);
	camera->setFreeMove(false);
	freeCam = false;

	sceneShader = new Shader(CWSHADERDIR"SceneVertex.glsl", CWSHADERDIR"SceneFragment.glsl");
	sceneShaderBump = new Shader(CWSHADERDIR"SceneVertexBump.glsl", CWSHADERDIR"SceneFragmentBump.glsl");
	skyboxShader = new Shader(CWSHADERDIR"skyboxVertex.glsl", CWSHADERDIR"skyboxFragment.glsl");
	md5Shader = new Shader(CWSHADERDIR"skeletonVertex.glsl", CWSHADERDIR"skeletonFragment.glsl");
	reflectShader = new Shader(CWSHADERDIR"SceneVertex.glsl", CWSHADERDIR"reflectFragment.glsl");
	displaceShader = new Shader(CWSHADERDIR"DisplacementVertex.glsl", CWSHADERDIR"DisplacementFragment.glsl");

	grassShader = new Shader(CWSHADERDIR"GrassVertex.glsl", CWSHADERDIR"GrassFragment.glsl", CWSHADERDIR"GrassGeom.glsl",
		CWSHADERDIR"GrassTessC.glsl", CWSHADERDIR"GrassTessE.glsl");
	lightShader = new Shader(CWSHADERDIR"LightVertex.glsl", CWSHADERDIR"LightFragment.glsl");

	extractBlueShader = new Shader(CWSHADERDIR"ProcessVertex.glsl", CWSHADERDIR"ExtractBlueFragment.glsl");
	extractShader = new Shader(CWSHADERDIR"ProcessVertex.glsl", CWSHADERDIR"ExtractLightFragment.glsl");
	portalShader = new Shader(CWSHADERDIR"ProcessVertex.glsl", CWSHADERDIR"PortalFragment.glsl");
	processShader = new Shader(CWSHADERDIR"ProcessVertex.glsl", CWSHADERDIR"ProcessFragment.glsl");
	presentShader = new Shader(CWSHADERDIR"TexturedVertex.glsl", CWSHADERDIR"TexturedFragment.glsl");

	skyQuad = Mesh::GenerateQuad();

	bool meshes = LoadMeshes();

	eiffelScene = true;

	cubeMap = SOIL_load_OGL_cubemap(CWTEXTUREDIR"ColdNightLeft.jpg", CWTEXTUREDIR"ColdNightRight.jpg", CWTEXTUREDIR"ColdNightUp.jpg",
		CWTEXTUREDIR"ColdNightDown.jpg", CWTEXTUREDIR"ColdNightFront.jpg", CWTEXTUREDIR"ColdNightBack.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	brightCubeMap = SOIL_load_OGL_cubemap(CWTEXTUREDIR"BlueSunsetLeft.jpg", CWTEXTUREDIR"BlueSunsetRight.jpg", CWTEXTUREDIR"BlueSunsetUp.jpg",
		CWTEXTUREDIR"BlueSunsetDown.jpg", CWTEXTUREDIR"BlueSunsetFront.jpg", CWTEXTUREDIR"BlueSunsetBack.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!sceneShader->LinkProgram() || !sceneShaderBump->LinkProgram() || !extractShader->LinkProgram() ||
		!processShader->LinkProgram() || !presentShader->LinkProgram() || !skyboxShader->LinkProgram() ||
		!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !displaceShader->LinkProgram() ||
		!portalShader->LinkProgram() || !md5Shader->LinkProgram() || !extractBlueShader->LinkProgram() ||
		!grassShader->LinkProgram() || !cubeMap || !brightCubeMap || !meshes) {
		return;
	}

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	glGenTextures(1, &portalDepthTex);
	glBindTexture(GL_TEXTURE_2D, portalDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);


	for (int i = 0; i < 5; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);
	glGenFramebuffers(1, &extractFBO);
	glGenFramebuffers(1, &portalFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, portalFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, portalDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, portalDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[3], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !portalDepthTex || !bufferColourTex[3]) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	eiffelTimer = 0.0f;
	grassTimer = 0.0f;


	moonLight = new DirLight(Vector3(-0.0f, -0.5f, -0.2f), Vector4(0.96f, 0.96f, 1.0f, 1.0f));
	eveLight = new DirLight(Vector3(-0.0f, -0.4f, 0.5f), Vector4(1.0f, 0.9f, 0.7f, 1.0f));

	root = new SceneNode();
	root->SetShader(sceneShader);
	root->SetTransform(Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f)));

	altRoot = new SceneNode();
	altRoot->SetShader(sceneShader);
	altRoot->SetTransform(Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f)));

	SceneNode* portal = new SceneNode(archway, Vector4(1, 1, 1, 1), sceneShader);
	portal->SetTransform(
		Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f)) *
		Matrix4::Translation(Vector3(50.0f, 0.2f, 0)) *
		Matrix4::Rotation(-90.0f, Vector3(0, 1, 0)));
	root->AddChild(portal);
	portal->SetBoundingRadius(100.0f);

	Portal* portalZone = new Portal();
	portal->AddChild(portalZone);
	portalNode = portalZone;
	portalNode->SetShader(lightShader);

	SceneNode* portal2 = new SceneNode(archway, Vector4(1, 1, 1, 1), sceneShader);
	portal2->SetTransform(
		Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f)) *
		Matrix4::Translation(Vector3(50.0f, 0.2f, 0)) *
		Matrix4::Rotation(90.0f, Vector3(0, 1, 0)));
	altRoot->AddChild(portal2);
	portal2->SetBoundingRadius(100.0f);

	Portal* portalZone2 = new Portal();
	portal2->AddChild(portalZone2);
	altPortalNode = portalZone2;
	altPortalNode->SetShader(lightShader);
	altPortalNode->SetActive(false);
	
	InitBuildings();

	SceneNode* ground = new SceneNode();
	ground->SetTransform(
		Matrix4::Rotation(90.0f, Vector3(1, 0, 0)) *
		Matrix4::Scale(Vector3(2500.0f, 2500.0f, 2500.0f)));
	root->AddChild(ground);
	ground->SetBoundingRadius(10000.0f);

	SceneNode* pavement = new SceneNode();
	pavement->SetTransform(
		Matrix4::Translation(Vector3(0.0f, 10.0f, 0.0f))*
		Matrix4::Scale(Vector3(30.0f, 30.0f, 30.0f)));
	root->AddChild(pavement);
	pavement->SetBoundingRadius(10000.0f);


	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			SceneNode* groundC = new SceneNode(floor, Vector4(0.4f, 0.4f, 0.4f, 1));
			ground->AddChild(groundC);
			groundC->SetTransform(Matrix4::Translation(Vector3(2.0f*i, 2.0f*j, 0.0f)));


			SceneNode* pavement2 = new SceneNode(paveMesh, Vector4(0.6f, 0.6f, 0.6f, 1));
			pavement->AddChild(pavement2);
			pavement2->SetTransform(Matrix4::Translation(Vector3(168.025f * i, 0.0f, 168.025f * j)));
			pavement2->SetShader(sceneShaderBump);
		}
	}

	grass = new SceneNode(heightMap,Vector4(0.46f,0.57f,0.29f,1.0f));

	grass->SetTransform(Matrix4::Scale(Vector3(200.0f, 200.0f, 200.0f))*
		Matrix4::Translation(Vector3(25.0f, 0.1f, 0.0f)) *
		Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f)));
	altRoot->AddChild(grass);
	grass->SetBoundingRadius(100000.0f);
	grass->SetShader(grassShader);
	grass->GetMesh()->SetPatches();
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	grass->SetUniformInt("shadowTex", 2);
	grass->SetUniformInt("heightMap", 0);
	grass->SetUniformFloat("rise", 0.0f);
	grass->SetUniformInt("normalMap", 1);
	grass->SetUniformInt("grassTex", 3);




	portalStand = new SceneNode(stand, Vector4(0.9f, 0.85f, 0.8f, 1));
	portalStand->SetTransform(
		Matrix4::Scale(Vector3(300.0f, 300.0f, 300.0f)) *
		Matrix4::Translation(Vector3(7.82f, 0.8f, 0))*
		Matrix4::Scale(Vector3(0.25f, 2.0f, 0.25f)));
	altRoot->AddChild(portalStand);
	portalStand->SetBoundingRadius(1000.0f);
	portalStand->SetActive(false);

	louvreRoot = new SceneNode(louvreShell, Vector4(0.4f, 0.4f, 0.4f, 1));
	louvreRoot->SetTransform(
		Matrix4::Scale(Vector3(300.0f, 300.0f, 300.0f)) *
		Matrix4::Translation(Vector3(15.0f, 0.6f, 0)));
	altRoot->AddChild(louvreRoot);
	louvreRoot->SetBoundingRadius(10000.0f);

	SceneNode* louvreStand = new SceneNode(stand, Vector4(0.9f, 0.85f, 0.8f, 1));
	louvreRoot->AddChild(louvreStand);

	louvreInner = new SceneNode(louvreGlass, Vector4(0.85f, 0.85f, 0.95f, 0.3f));
	louvreRoot->AddChild(louvreInner);

	waterNode = new SceneNode(waterMesh);
	altRoot->AddChild(waterNode);
	waterNode->SetTransform(
		louvreRoot->GetTransform()*
		Matrix4::Rotation(90.0f,Vector3(1,0,0))*
		Matrix4::Translation(Vector3(0.0f, 0.0f, 0.95f))*
		Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f)));
	waterNode->SetBoundingRadius(10000.0f);
	waterNode->SetShader(reflectShader);

	pointLights = new Light[49];
	

	for (int i = 0; i < 4; ++i) {
		SceneNode* lampNode = new SceneNode(lampost, Vector4(0.2f, 0.2f, 0.2f, 1));
		lampNode->SetTransform(
			Matrix4::Translation(Vector3(1500.0f + 300.0f * i, 0.0f, 350.0f))
		);
		root->AddChild(lampNode);
		lampNode->SetBoundingRadius(100.0f);

		SceneNode* lampNode2 = new SceneNode(lampost, Vector4(0.2f, 0.2f, 0.2f, 1));
		lampNode2->SetTransform(
			Matrix4::Translation(Vector3(1500.0f + 300.0f * i, 0.0f, -350.0f))
		);
		root->AddChild(lampNode2);
		lampNode2->SetBoundingRadius(100.0f);

		
		SceneNode* lampLight = new SceneNode(sphere, Vector4(1.0f, 0.92f, 0.52f, 1));
		root->AddChild(lampLight);
		lampLight->SetTransform(
			Matrix4::Translation(Vector3(1500.0f + 300.0f * i, 94.0f, 350.0f))*
			Matrix4::Scale(Vector3(3.5f, 3.5f, 3.5f))
		);
		lampLight->SetShader(lightShader);

		SceneNode* lampLight2 = new SceneNode(sphere, Vector4(1.0f, 0.92f, 0.52f, 1));
		root->AddChild(lampLight2);
		lampLight2->SetTransform(
			Matrix4::Translation(Vector3(1500.0f + 300.0f * i, 94.0f, -350.0f)) *
			Matrix4::Scale(Vector3(3.5f, 3.5f, 3.5f))
		);
		lampLight2->SetShader(lightShader);

		Light& l = pointLights[2*i];
		l.SetPosition(Vector3(750.0f + 150.0f * i, 50.0f, 175.0f));
		l.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l.SetRadius(150.0f);

		Light& l2 = pointLights[(2 * i)+1];
		l2.SetPosition(Vector3(750.0f + 150.0f * i, 50.0f, -175.0f));
		l2.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l2.SetRadius(150.0f);
	}

	for (int i = -1; i <= 1; i+=2) {
		Light& l = pointLights[9 + i];
		l.SetPosition(Vector3(200.0f * i, 60.0f, 0.0f));
		l.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l.SetRadius(100.0f);

		Light& l2 = pointLights[10 + i];
		l2.SetPosition(Vector3(0.0f, 60.0f, 200.0f*i));
		l2.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l2.SetRadius(100.0f);


		Light& l3 = pointLights[13 + i];
		l3.SetPosition(Vector3(140.0f * i, 120.0f, 0.0f));
		l3.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l3.SetRadius(100.0f);

		Light& l4 = pointLights[14 + i];
		l4.SetPosition(Vector3(0.0f, 120.0f, 140.0f * i));
		l4.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l4.SetRadius(100.0f);


		Light& l5 = pointLights[17 + i];
		l5.SetPosition(Vector3(95.0f * i, 221.0f, 0.0f));
		l5.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l5.SetRadius(70.0f);

		Light& l6 = pointLights[18 + i];
		l6.SetPosition(Vector3(0.0f, 221.0f, 95.0f * i));
		l6.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l6.SetRadius(70.0f);


		Light& l7 = pointLights[21 + i];
		l7.SetPosition(Vector3(34.0f * i, 278.0f, 0.0f));
		l7.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l7.SetRadius(70.0f);

		Light& l8 = pointLights[22 + i];
		l8.SetPosition(Vector3(0.0f, 278.0f, 34.0f * i));
		l8.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l8.SetRadius(70.0f);


		Light& l9 = pointLights[25 + i];
		l9.SetPosition(Vector3(25.0f * i, 366.0f, 0.0f));
		l9.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l9.SetRadius(70.0f);

		Light& l10 = pointLights[26 + i];
		l10.SetPosition(Vector3(0.0f, 366.0f, 25.0f * i));
		l10.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l10.SetRadius(70.0f);


		Light& l11 = pointLights[29 + i];
		l11.SetPosition(Vector3(20.0f * i, 445.0f, 0.0f));
		l11.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l11.SetRadius(70.0f);

		Light& l12 = pointLights[30 + i];
		l12.SetPosition(Vector3(0.0f, 445.0f, 20.0f * i));
		l12.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l12.SetRadius(70.0f);


		Light& l13 = pointLights[33 + i];
		l13.SetPosition(Vector3(16.0f * i, 521.0f, 0.0f));
		l13.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l13.SetRadius(70.0f);

		Light& l14 = pointLights[34 + i];
		l14.SetPosition(Vector3(0.0f, 521.0f, 16.0f * i));
		l14.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l14.SetRadius(70.0f);


		Light& l15 = pointLights[37 + i];
		l15.SetPosition(Vector3(13.0f * i, 581.0f, 0.0f));
		l15.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l15.SetRadius(70.0f);

		Light& l16 = pointLights[38 + i];
		l16.SetPosition(Vector3(0.0f, 581.0f, 13.0f * i));
		l16.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l16.SetRadius(70.0f);


		Light& l17 = pointLights[41 + i];
		l17.SetPosition(Vector3(11.0f * i, 650.0f, 0.0f));
		l17.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l17.SetRadius(70.0f);

		Light& l18 = pointLights[42 + i];
		l18.SetPosition(Vector3(0.0f, 650.0f, 11.0f * i));
		l18.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l18.SetRadius(70.0f);


		Light& l19 = pointLights[45 + i];
		l19.SetPosition(Vector3(13.0f * i, 715.0f, 0.0f));
		l19.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l19.SetRadius(70.0f);

		Light& l20 = pointLights[46 + i];
		l20.SetPosition(Vector3(0.0f, 715.0f, 13.0f * i));
		l20.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
		l20.SetRadius(70.0f);
	}

	Light& ltop = pointLights[48];
	ltop.SetPosition(Vector3(0.0f, 770.0f, 0.0f));
	ltop.SetColour(Vector4(0.9f, 0.6f, 0.3f, 1.0f));
	ltop.SetRadius(200.0f);
	
	eiffelLights = new SceneNode(towerLights, Vector4(1.0f, 0.92f, 0.52f, 1));
	root->AddChild(eiffelLights);
	eiffelLights->SetTransform(
		Matrix4::Rotation(45,Vector3(0.0f, 1.0f, 0.0f))*
		Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f))
	);
	eiffelLights->SetShader(lightShader);
	eiffelLights->SetBoundingRadius(1000.0f);
	eiffelLights->SetActive(false);

	lightsOn = false;
	
	for (int i = 0; i < 5; ++i) {
		MD5Node* womanNode = new MD5Node(*womanData);
		womanNode->PlayAnim(CWMESHDIR"Woman/idle.md5anim");
		womanNode->SetShader(md5Shader);
		float r = 0.5f + (float)(rand() % 129) / 128.0f;
		float g = 0.5f + (float)(rand() % 129) / 128.0f;
		float b = 0.5f + (float)(rand() % 129) / 128.0f;
		womanNode->SetUniformVec4("baseColour", Vector4(r, g, b, 1));
		womanNode->SetTransform(
			Matrix4::Rotation((float)(rand() % 3600)/10.0f, Vector3(0, 1, 0)) *
			Matrix4::Translation(Vector3(500.0f + (float)(rand() % 300), 0, 0)) *
			Matrix4::Scale(Vector3(0.3f, 0.3f, 0.3f)) *
			Matrix4::Rotation((float)(rand() % 360), Vector3(0, 1, 0)));
		root->AddChild(womanNode);
		womanNode->SetBoundingRadius(30.0f);
	}

	eifT = new EiffelTower();

	root->AddChild(eifT);


	root->SetUniformVec3("lightDir", moonLight->GetDirection());
	root->SetUniformVec4("lightColour", moonLight->GetColour());

	altRoot->SetUniformVec3("lightDir", eveLight->GetDirection());
	altRoot->SetUniformVec4("lightColour", eveLight->GetColour());


	for (int i = 0; i < 49; i++) {
		Light& l = pointLights[i];
		positions[i] = l.GetPosition();
		colours[i] = l.GetColour();
		radii[i] = l.GetRadius();
		if (i < 8) {
			radiiToUse[i] = radii[i];
		}
		else {
			radiiToUse[i] = 0.0f;
		}
	}
	


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	activeRoot = root;
	inactiveRoot = altRoot;
	activePortalNode = portalNode;

	CreatePath();


	
	

	timePassed = 0.0f;

	init = true;
	firstTime = true;
}

void Renderer::CreatePath() {
	path = SplineTrack(Vector3(1000.0f, 60.0f, 0.0f), Vector3(800.0f, 100.0f, 0.0f));
	tPath = SplineTrack(Vector3(0.0f, 100.0f, 0.0f), Vector3(0.0f, 200.0f, 0.0f));

	//around tower
	path.addPoint(Vector3(0.0f, 500.0f, 1000.0f));
	tPath.addPoint(Vector3(0.0f, 300.0f, 0.0f));
	path.addPoint(Vector3(-1000.0f, 500.0f, 0.0f));
	tPath.addPoint(Vector3(0.0f, 400.0f, 0.0f));
	path.addPoint(Vector3(0.0f, 500.0f, -1000.0f));
	tPath.addPoint(Vector3(0.0f, 500.0f, 0.0f));
	path.addPoint(Vector3(1000.0f, 500.0f, 0.0f));
	tPath.addPoint(Vector3(0.0f, 300.0f, 0.0f));

	//under arch
	path.addPoint(Vector3(300.0f, 100.0f, 300.0f));
	tPath.addPoint(Vector3(0.0f, 50.0f, 0.0f));
	path.addPoint(Vector3(0.0f, 50.0f, 0.0f));
	tPath.addPoint(Vector3(-300.0f, 100.0f, -300.0f));
	path.addPoint(Vector3(-300.0f, 100.0f, -300.0f));
	tPath.addPoint(Vector3(0.0f, 500.0f, -1000.0f));
	path.addPoint(Vector3(0.0f, 500.0f, -1000.0f));
	tPath.addPoint(Vector3(0.0f, 500.0f, 0.0f));
	path.addPoint(Vector3(1000.0f, 500.0f, 0.0f));
	tPath.addPoint(Vector3(0.0f, 500.0f, 0.0f));

	//through portal
	path.addPoint(Vector3(1000.0f, 100.0f, 0.0f));
	tPath.addPoint(Vector3(1000.0f, 100.0f, 50.0f));
	path.addPoint(Vector3(1300.0f, 70.0f, 0.0f));
	tPath.addPoint(Vector3(2000.0f, 50.0f, 0.0f));
	path.addPoint(Vector3(1600.0f, 200.0f, 0.0f));
	tPath.addPoint(Vector3(2200.0f, 50.0f, 0.0f));

	path.addPoint(Vector3(1800.0f, 200.0f, 600.0f));
	tPath.addPoint(Vector3(2200.0f, 50.0f, 0.0f));

	//around louvre
	path.addPoint(Vector3(2100.0f, 400.0f, 1000.0f));
	tPath.addPoint(Vector3(2200.0f, 150.0f, 0.0f));
	path.addPoint(Vector3(2900.0f, 400.0f, 400.0f));
	tPath.addPoint(Vector3(2200.0f, 150.0f, 0.0f));
	path.addPoint(Vector3(2900.0f, 400.0f, -400.0f));
	tPath.addPoint(Vector3(2200.0f, 150.0f, 0.0f));
	path.addPoint(Vector3(2200.0f, 400.0f, -1000.0f));
	tPath.addPoint(Vector3(2200.0f, 150.0f, 0.0f));
	path.addPoint(Vector3(1700.0f, 300.0f, 0.0f));
	tPath.addPoint(Vector3(1000.0f, 50.0f, 0.0f));
	path.addPoint(Vector3(1300.0f, 60.0f, 0.0f));
	tPath.addPoint(Vector3(1000.0f, 50.0f, 0.0f));
	path.addPoint(Vector3(1000.0f, 50.0f, 0.0f));
	tPath.addPoint(Vector3(900.0f, 50.0f, 0.0f));


	path.setComplete(true);
	tPath.setComplete(true);
}

void Renderer::InitBuildings(){
	for (int i = 1; i < 12; ++i) {
		SceneNode* offset = new SceneNode(building, Vector4(1, 1, 1, 1));
		offset->SetTransform(
			Matrix4::Rotation(30.0f * (i - 6) + 180.0f, Vector3(0, 1, 0)) *
			Matrix4::Scale(Vector3(400.0f, 400.0f, 400.0f)) *
			Matrix4::Translation(Vector3(3.4f, 0, 0)) *
			Matrix4::Rotation(90.0f, Vector3(0, 1, 0)));
		root->AddChild(offset);
		offset->SetBoundingRadius(280.0f);
	}

	for (int j = 2; j < 7; ++j) {
		int numBuildings = 10 + j * 2;
		for (int i = 1; i < numBuildings; ++i) {
			SceneNode* offset = new SceneNode(lowQbuilding, Vector4(1, 1, 1, 1));
			offset->SetTransform(
				Matrix4::Rotation((360.0f / (float)numBuildings) * i, Vector3(0, 1, 0)) *
				Matrix4::Scale(Vector3(400.0f, 400.0f, 400.0f)) *
				Matrix4::Translation(Vector3(2.4f + 1.0f * j, 0, 0)) *
				Matrix4::Rotation(90.0f, Vector3(0, 1, 0)));
			root->AddChild(offset);
			offset->SetBoundingRadius(280.0f);
		}
	}

	portalBuilding = new SceneNode(building, Vector4(1, 1, 1, 1));
	portalBuilding->SetTransform(
		Matrix4::Scale(Vector3(400.0f, 400.0f, 400.0f)) *
		Matrix4::Translation(Vector3(6.6f, 0, 0)) *
		Matrix4::Rotation(-90.0f, Vector3(0, 1, 0)));
	root->AddChild(portalBuilding);
	portalBuilding->SetBoundingRadius(280.0f);
	portalBuilding->SetActive(true);

	for (int i = 1; i <= 2; ++i) {
		SceneNode* extraBuilding = new SceneNode(lowQbuilding, Vector4(1, 1, 1, 1));
		portalBuilding->AddChild(extraBuilding);
		extraBuilding->SetTransform(
			Matrix4::Rotation(90.0f, Vector3(0, 1, 0)) *
			Matrix4::Translation(Vector3(1.0f*i, 0, 0)) *
			Matrix4::Rotation(-90.0f, Vector3(0, 1, 0)));
		extraBuilding->SetBoundingRadius(280.0f);
	}
	
	outerBuidlings = new SceneNode();
	root->AddChild(outerBuidlings);
	for (int j = 7; j < 15; ++j) {
		int numBuildings = 4 + j;
		for (int i = 0; i < numBuildings; ++i) {
			SceneNode* offset = new SceneNode(ulowQbuilding, Vector4(1, 1, 1, 1));
			offset->SetTransform(
				Matrix4::Rotation((360.0f / (float)numBuildings)*i + (j%2)*50.0f, Vector3(0, 1, 0)) *
				Matrix4::Scale(Vector3(400.0f, 400.0f, 400.0f)) *
				Matrix4::Translation(Vector3(2.4f + 1.0f * j, 0, 0)) *
				Matrix4::Rotation(90.0f, Vector3(0, 1, 0)));
			outerBuidlings->AddChild(offset);
			offset->SetBoundingRadius(500.0f);
		}
	}
	outerBuidlings->SetActive(true);
}

Renderer::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	delete root;
	delete floor;
	delete camera;

	delete stand;

	delete moonLight;
	delete eveLight;
	delete sceneShader;
	delete sceneShaderBump;
	delete skyboxShader;
	delete reflectShader;
	delete displaceShader;
	delete skyQuad;

	Shader* lightShader;
	Shader* extractShader;
	Shader* extractBlueShader;
	Shader* portalShader;

	delete processShader;
	delete presentShader;
	delete grassShader;

	delete waterMesh;
	delete paveMesh;

	delete lampost;
	delete building;
	delete lowQbuilding;
	delete ulowQbuilding;

	delete womanData;

	delete louvreGlass;
	delete louvreShell;

	delete quad;
	delete sphere;
	delete[] pointLights;

	delete towerLights;

	currentShader = 0;


	glDeleteTextures(5, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &extractFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

void Renderer::UpdateScene(float msec) {
	timePassed += msec;
	eiffelTimer += msec;
	grassTimer += msec;
	if (firstTime) {
		firstTime = false;
		timePassed = 0.0f;
		RestartEiffel();
		RestartLouvre(false);
	}
	float scaledTime = timePassed * 0.000017f;
	if (!freeCam) {
		camera->SetPosition(path.pointAt(scaledTime));
		camera->LookAt(tPath.pointAt(scaledTime));
	}
	
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->SetUniformVec3("cameraPos", camera->GetPosition());
	altRoot->SetUniformVec3("cameraPos", camera->GetPosition());

	Vector3 pos = camera->GetPosition();
	if (abs(pos.z) < 15.0f && abs(pos.y - 50.0f) < 50.0f) {
		if (pos.x < 1250.0f && pos.x > 1200.0f) {
			SetScene(true);
		}
		else if (pos.x > 1250.0f && pos.x < 1300.0f) {
			SetScene(false);
		}
	}


	frames++;
	acc += msec;
	if (acc >= 1000.0f) {
		cout << "FPS: " << frames << endl;
		acc = 0.0f;
		frames = 0;
	}

	root->Update(msec);
	altRoot->Update(msec);

	if (eiffelTimer > 28000.0f && !lightsOn) {
		lightsOn = true;
		eiffelLights->SetActive(true);
		for (int i = 8; i < 49; ++i) {
			radiiToUse[i] = radii[i];
		}
	}

	float grassScaleTime = min(max(grassTimer-34000.0f,0.0f)/4000.0f,1.0f);
	float standScaleTime = min(max(grassTimer - 38000.0f, 0.0f) / 4000.0f, 1.0f);
	float louvreScaleTime = min(max(grassTimer - 42000.0f, 0.0f) / 2000.0f, 1.0f);

	
	grass->SetTransform(Matrix4::Scale(Vector3(200.0f, 200.0f, 200.0f))*
		Matrix4::Translation(Vector3(25.0f, 0.1f-4.5f*grassScaleTime, 0.0f)) *
		Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f)));
	grass->SetUniformFloat("rise", grassScaleTime);
	grass->SetUniformFloat("time", timePassed * 0.0005f);


	louvreRoot->SetTransform(
		Matrix4::Scale(Vector3(300.0f, 300.0f, 300.0f)) *
		Matrix4::Translation(Vector3(15.0f, -1.0f+1.6f*standScaleTime, 0)));

	louvreRoot->SetModelScale(Vector3(1.0f, 0.01f + 1.0f * louvreScaleTime, 1.0f));
	louvreInner->SetModelScale(Vector3(1.0f, 0.01f+1.0f*louvreScaleTime, 1.0f));
}

void Renderer::SetScene(bool b) {
	eiffelScene = b;

	if (eiffelScene) {
		activePortalNode = portalNode;
		activeRoot = root;
		inactiveRoot = altRoot;
		portalBuilding->SetActive(true);
		outerBuidlings->SetActive(true);
		portalStand->SetActive(false);
		altPortalNode->SetActive(false);
		portalNode->SetActive(true);
	}
	else {
		activePortalNode = altPortalNode;
		activeRoot = altRoot;
		inactiveRoot = root;
		portalBuilding->SetActive(false);
		outerBuidlings->SetActive(false);
		portalStand->SetActive(true);
		portalNode->SetActive(false);
		altPortalNode->SetActive(true);
	}
}

void Renderer::GoToScene(int i) {
	SetCameraFree(true);
	switch (i) {
	case 1:
		camera->SetPosition(Vector3(820.0f,20.0f,140.0f));
		camera->LookAt(Vector3(0,350.0f,180.0f));
		SetScene(true);
		break;
	case 2:
		camera->SetPosition(Vector3(1900.0f, 400.0f, 600.0f));
		camera->LookAt(Vector3(2200.0f,100.0f,-200.0f));
		SetScene(false);
		break;
	case 3:
		camera->SetPosition(Vector3(400.0f,20.0f,0.0f));
		camera->LookAt(Vector3(1000.0f, 20.0f, 0.0f));
		SetScene(true);
		break;
	case 4:
		camera->SetPosition(Vector3(400.0f,300.0f,200.0f));
		camera->LookAt(Vector3(100.0f,50.0f,-300.0f));
		SetScene(true);
		break;
	case 5:
		camera->SetPosition(Vector3(-900.0f,300.0f,900.0f));
		camera->LookAt(Vector3(0.0f,300.0f,0.0f));
		SetScene(true);
		break;
	case 6:
		camera->SetPosition(Vector3(1200.0f, 25.0f, 600.0f));
		camera->LookAt(Vector3(2000.0f, 100.0f, 0.0f));
		SetScene(false);
		break;
	}
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (!(from->GetActive())) {
		return;
	}
	if (frameFrustum.InsideFrustrum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes(bool savePortal) {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}
	if (savePortal) {
		glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[4], 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Matrix4 tempProj = projMatrix;
		Matrix4 tempView = viewMatrix;
		Matrix4 tempMod = modelMatrix;
		Matrix4 tempTex = textureMatrix;
		SetCurrentShader(extractBlueShader);
		projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		viewMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		textureMatrix.ToIdentity();
		UpdateShaderMatrices();

		glDisable(GL_DEPTH_TEST);


		quad->SetTexture(bufferColourTex[0]);
		quad->Draw();

		glEnable(GL_DEPTH_TEST);

		projMatrix = tempProj;
		viewMatrix = tempView;
		modelMatrix = tempMod;
		textureMatrix = tempTex;
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	}
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetShader()) {
		SetCurrentShader(n->GetShader());
		UpdateShaderMatrices();
		if (eiffelScene) {
			glUniform3fv(glGetUniformLocation(n->GetShader()->GetProgram(), "pLightPos"), 49, (float*)&positions);
			glUniform4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "pLightColour"), 49, (float*)&colours);
			glUniform1fv(glGetUniformLocation(n->GetShader()->GetProgram(), "pLightRadius"), 49, radiiToUse);
		}
		else {
			glUniform1fv(glGetUniformLocation(n->GetShader()->GetProgram(), "pLightRadius"), 49, zeroes);
		}
	}
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		n->SetUniformMat4("modelMatrix", transform);
		n->SetUniformVec4("nodeColour", n->GetColour());
		n->SetUniformInt("useTexture", (int)n->GetMesh()->GetTexture());

		n->Draw(*this);
	}
}

void Renderer::RestartEiffel() {
	eiffelTimer = 0.0f;
	eifT->RestartBuild();
	lightsOn = false;
	eiffelLights->SetActive(false);
	for (int i = 8; i < 49; ++i) {
		radiiToUse[i] = 0.0f;
	}
}

void Renderer::RestartLouvre(bool instant) {
	grassTimer = 0.0f;
	if (instant) {
		grassTimer = 32000.0f;
	}
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	if (eiffelScene) {
		viewMatrix = Matrix4::BuildViewMatrix(Vector3(0.0f, 3000.0f, 3000.0f), Vector3(0, 0, 0));
		altRoot->SetActive(false);
	}
	else {
		viewMatrix = Matrix4::BuildViewMatrix(Vector3(2000.0f, 3000.0f, -2000.0f), Vector3(2000.0f, 0, 0));
		root->SetActive(false);
	}
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();

	DrawPhysicals(true);

	root->SetActive(true);
	altRoot->SetActive(true);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
	root->SetUniformInt("shadowTex", 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	DrawPhysicals(false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(0);
}

void Renderer::DrawPhysicals(bool shadows) {
	
	if (!shadows) {
		glBindFramebuffer(GL_FRAMEBUFFER, portalFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	BuildNodeLists(inactiveRoot);
	SortNodeLists();

	DrawSkybox(!eiffelScene);

	inactiveRoot->SetUniformInt("diffuseTex", 0);

	DrawNodes(false);

	ClearNodeLists();

	if (!shadows) {
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	DrawSkybox(eiffelScene);

	BuildNodeLists(activeRoot);
	SortNodeLists();

	activeRoot->SetUniformInt("diffuseTex", 0);

	DrawNodes(shadows? false:true);
	ClearNodeLists();
	glDisable(GL_STENCIL_TEST);
}

void Renderer::DrawScene() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawShadowScene();
	

	

	float portalDist = Vector3::Distance(camera->GetPosition(),portalNode->GetWorldTransform().GetPositionVector());

	float shadowRatio = max(min((portalDist - 20.0f) / 100.0f, 1.0f), 0.0f);


	root->SetUniformFloat("shadowR", shadowRatio);
	altRoot->SetUniformFloat("shadowR", shadowRatio);

	DrawCombinedScene();

	glUseProgram(0);
	
}

void Renderer::DrawPortal() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 9);

	for (int i = 0; i < 7; ++i) {
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "passNum"), -1.0f);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[3]);
		quad->Draw();

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[3], 0);

		quad->SetTexture(bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, extractFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(portalShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	float portalDist = Vector3::Distance(camera->GetPosition(), portalNode->GetWorldTransform().GetPositionVector());

	float shadowRatio = max(min((portalDist) / 100.0f, 1.0f), 0.0f);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "whiteout"), shadowRatio);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[4]);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "portalSpaceTex"), 5);

	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, waterMesh->GetDispMap());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "displacementMap"), 6);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), timePassed*0.00001f);

	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[3]);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "portalTex"), 7);

	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	quad->SetTexture(bufferColourTex[1]);
	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawBloom() {
	glBindFramebuffer(GL_FRAMEBUFFER, extractFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(extractShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);


	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[2], 0);

	quad->SetTexture(bufferColourTex[1]);
	quad->Draw();

	SetCurrentShader(processShader);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 9);

	for (int i = 0; i < POST_PASSES; ++i) {
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "passNum"), -1.0f);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[2]);
		quad->Draw();

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[2], 0);

		quad->SetTexture(bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);

}

void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0 + 9);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);


	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 9);

	for (int i = 0; i < POST_PASSES; ++i) {
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "passNum"), (float) i);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[0]);
		quad->Draw();
		
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		quad->SetTexture(bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);

}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	SetCurrentShader(presentShader);
	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[2]);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bloomTex"), 8);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();
	glUseProgram(0);
}

void Renderer::RenderScene() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	Matrix4 tempProj = projMatrix;
	Matrix4 tempView = viewMatrix;
	Matrix4 tempMod = modelMatrix;
	Matrix4 tempTex = textureMatrix;
	DrawScene();
	DrawPortal();
	DrawBloom();
	DrawPostProcess();
	PresentScene();
	projMatrix = tempProj;
	viewMatrix = tempView;
	modelMatrix = tempMod;
	textureMatrix = tempTex;
	SwapBuffers();
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawSkybox(bool b) {
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, b ? cubeMap : brightCubeMap);

	SetCurrentShader(skyboxShader);
	

	UpdateShaderMatrices();
	skyQuad->Draw();

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Renderer::SetCameraFree(bool b) {
	freeCam = b;
	camera->setFreeLook(b);
	camera->setFreeMove(b);
	if (!b) {
		firstTime = true;
		SetScene(true);
	}
}

bool Renderer::LoadMeshes() {
	OBJMesh* m = new OBJMesh();
	m->LoadOBJMesh(CWMESHDIR"Building/russianMEDfull.obj");
	m->SetTexture(SOIL_load_OGL_texture(CWTEXTUREDIR"Russian.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	building = m;

	glBindTexture(GL_TEXTURE_2D, building->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	m = new OBJMesh();
	m->LoadOBJMesh(CWMESHDIR"Building/russianLOW.obj");
	m->SetTexture(building->GetTexture());
	lowQbuilding = m;

	m = new OBJMesh();
	m->LoadOBJMesh(CWMESHDIR"Building/ULOWx3.obj");
	m->SetTexture(SOIL_load_OGL_texture(CWTEXTUREDIR"Russian.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	ulowQbuilding = m;

	m = new OBJMesh(CWMESHDIR"Louvre/LouvreShell.obj");
	louvreShell = m;

	m = new OBJMesh(CWMESHDIR"Louvre/LouvreGlass.obj");
	louvreGlass = m;

	m = new OBJMesh(CWMESHDIR"Eiffel/eiffelLights.obj");
	towerLights = m;

	m = new OBJMesh(CWMESHDIR"Arch/Arch.obj");
	m->SetTexture(SOIL_load_OGL_texture(CWTEXTUREDIR"Stone.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	archway = m;

	glBindTexture(GL_TEXTURE_2D, archway->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);


	floor = Mesh::GenerateQuad(50.0f);
	floor->SetTexture(SOIL_load_OGL_texture(CWTEXTUREDIR"grass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	glBindTexture(GL_TEXTURE_2D, floor->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	m = new OBJMesh(CWMESHDIR"Pavement/pavement.obj");
	m->SetTexture(SOIL_load_OGL_texture(CWTEXTUREDIR"pavement/pavement.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	m->SetBumpMap(SOIL_load_OGL_texture(CWTEXTUREDIR"pavement/pavementNormals.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	paveMesh = m;

	glBindTexture(GL_TEXTURE_2D, paveMesh->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, paveMesh->GetBumpMap());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	waterMesh = Mesh::GenerateQuad();
	waterMesh->SetDispMap(SOIL_load_OGL_texture(TEXTUREDIR"waterdisp.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	glBindTexture(GL_TEXTURE_2D, waterMesh->GetDispMap());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	m = new OBJMesh(CWMESHDIR"terrain.obj");
	m->SetTexture(SOIL_load_OGL_texture(CWTEXTUREDIR"heightmap.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	m->SetBumpMap(SOIL_load_OGL_texture(CWTEXTUREDIR"terrainNormal.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	m->SetDispMap(SOIL_load_OGL_texture(CWTEXTUREDIR"GrassTex.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap = m;

	womanData = new MD5FileData(CWMESHDIR"Woman/IdleWoman.md5mesh");
	womanData->AddAnim(CWMESHDIR"Woman/idle.md5anim");

	m = new OBJMesh(CWMESHDIR"lamp/lampost.obj");
	lampost = m;

	m = new OBJMesh(CWMESHDIR"stand.obj");
	stand = m;

	m = new OBJMesh(MESHDIR"ico.obj");
	sphere = m;

	quad = Mesh::GenerateQuad();

	return true;
}