#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/

const float CAM_SPEED = 0.05f;

void Camera::LookAt(Vector3 target) {
	Vector3 difference = position - target;
	if (difference.z == 0.0f) {
		yaw = 90.0f;
		if (difference.x < 0.0f) {
			yaw += 180;
		}
	}
	else if (difference.x == 0.0f) {
		yaw = 0.0f;
		if (difference.z < 0.0f) {
			yaw += 180;
		}
	}
	else {
		yaw = atan(difference.x / difference.z) * 180.0f / PI;

		if (difference.z < 0.0f) {
			yaw += 180;
		}
	}
	
	float xzlength = sqrt((difference.x * difference.x) + (difference.z * difference.z));

	pitch = -atan(difference.y / xzlength) * 180.0f / PI;
}

void Camera::UpdateMouse() {
	//Update the mouse by how much
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw	  -= (Window::GetMouse()->GetRelativePosition().x);
}

void Camera::UpdateCamera(float msec)	{
	if (freeLook) {
		UpdateMouse();
	}
	
	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch,90.0f);
	pitch = max(pitch,-90.0f);

	if(yaw <0) {
		yaw+= 360.0f;
	}
	if(yaw > 360.0f) {
		yaw -= 360.0f;
	}

	msec *= 5.0f;

	if (freeMove) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec * CAM_SPEED;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec * CAM_SPEED;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * CAM_SPEED;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * CAM_SPEED;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y += msec * CAM_SPEED;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y -= msec * CAM_SPEED;
		}
	}

}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1,0,0)) * 
			Matrix4::Rotation(-yaw, Vector3(0,1,0)) *
			Matrix4::Translation(-position);
};
