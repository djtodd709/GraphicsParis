#pragma once
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/OGLRenderer.h"

class Portal : public SceneNode
{
public:
	Portal(void);
	~Portal(void);
protected:
	Mesh* portalMesh;
};

