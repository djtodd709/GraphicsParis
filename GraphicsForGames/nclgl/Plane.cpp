#include "Plane.h"

Plane::Plane(const Vector3& normal, float distance, bool normalize) {
	if (normalize) {
		float length = sqrt(Vector3::Dot(normal, normal));

		this->normal = normal / length;
		this->distance = distance / length;
	}
	else {
		this->normal;
		this->distance;
	}
}

bool Plane::SphereInPlane(const Vector3& position, float radius) const {
	if (Vector3::Dot(position, normal) + distance <= -radius) {
		return false;
	}
	return true;
}