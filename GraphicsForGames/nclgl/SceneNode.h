#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "OGLRenderer.h"
#include <vector>

class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1), Shader* s = NULL);
	~SceneNode(void);

	void SetTransform(const Matrix4 &matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }
	
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }
	
	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }
	
	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* s) {
		shader = s;
		for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
			(*i)->SetShader(s);
		}
	}

	bool GetActive() const { return active; }
	void SetActive(bool b) {
		active = b;
	}

	void SetUniformInt(string name, int value);
	void SetUniformFloat(string name, float value);
	void SetUniformVec2(string name, Vector2 value);
	void SetUniformVec3(string name, Vector3 value);
	void SetUniformVec4(string name, Vector4 value);
	void SetUniformMat4(string name, Matrix4 value);

	vector<float> GetUniformFloats() {
		return uniformFloats;
	}
	
	void AddChild(SceneNode* s);
	
	virtual void Update(float msec);
	virtual void Draw(const OGLRenderer & r);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;

	Shader* shader;

	bool active;

	std::vector<int> uniformInts;
	std::vector<float> uniformFloats;
	std::vector<Vector2> uniformVec2s;
	std::vector<Vector3> uniformVec3s;
	std::vector<Vector4> uniformVec4s;
	std::vector<Matrix4> uniformMat4s;

	std::vector<string> intNames;
	std::vector<string> floatNames;
	std::vector<string> vec2Names;
	std::vector<string> vec3Names;
	std::vector<string> vec4Names;
	std::vector<string> mat4Names;

	std::vector<int> intNum;
	std::vector<int> floatNum;
	std::vector<int> vec2Num;
	std::vector<int> vec3Num;
	std::vector<int> vec4Num;
	std::vector<int> mat4Num;


	float distanceFromCamera;
	float boundingRadius;
};

