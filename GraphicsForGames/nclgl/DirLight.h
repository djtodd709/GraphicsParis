#pragma once
#include "Vector4.h"
#include "Vector3.h"

class DirLight
{
public:
	DirLight() {}

	DirLight(Vector3 direction, Vector4 colour) {
		this->direction = direction;
		this->colour = colour;
	}

	~DirLight(void) {};

	Vector3 GetDirection() const { return direction; }
	void SetDirection(Vector3 val) { direction = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

protected:
	Vector3 direction;
	Vector4 colour;
};
