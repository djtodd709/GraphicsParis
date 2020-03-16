#pragma once
#include <string>
#include <iostream>
#include <fstream>

#include "../../nclgl/Mesh.h"

#define RAW_WIDTH 512
#define RAW_HEIGHT 512

#define HEIGHTMAP_X 1.0f
#define HEIGHTMAP_Z 1.0f
#define	HEIGHTMAP_Y 0.5f
#define HEIGHTMAP_TEX_X 1.0f/16.0f
#define HEIGHTMAP_TEX_Z 1.0f/16.0f

class HeightMap : public Mesh
{
public:
	HeightMap(std::string name);
	~HeightMap(void) {};
};

