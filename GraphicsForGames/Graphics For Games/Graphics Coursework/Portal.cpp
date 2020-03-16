#include "Portal.h"

Portal::Portal(void) {
	OBJMesh* m = new OBJMesh(CWMESHDIR"Arch/Portal.obj");
	portalMesh = m;

	mesh = portalMesh;
	colour = Vector4(0, 0, 1, 1);
}

Portal::~Portal() {
	delete portalMesh;
}