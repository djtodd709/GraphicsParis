#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour, Shader* shader) {
	this -> mesh = mesh;
	this -> colour = colour;
	this -> shader = shader;
	parent = NULL;
	active = true;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
}

SceneNode ::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s -> parent = this;
	s-> boundingRadius = boundingRadius;
	if (!s->GetShader()) {
		s->SetShader(shader);
		s->uniformInts = uniformInts;
		s->uniformFloats = uniformFloats;
		s->uniformVec2s = uniformVec2s;
		s->uniformVec3s = uniformVec3s;
		s->uniformVec4s = uniformVec4s;
		s->uniformMat4s = uniformMat4s;

		s->intNames = intNames;
		s->floatNames = floatNames;
		s->vec2Names = vec2Names;
		s->vec3Names = vec3Names;
		s->vec4Names = vec4Names;
		s->mat4Names = mat4Names;
	}
}

void SceneNode::SetUniformInt(string name, int value) {
	for (int i = 0; i < uniformInts.size(); i++) {
		if (intNames[i].compare(name) == 0) {
			uniformInts[i] = value;
			for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
				(*i)->SetUniformInt(name, value);
			}
			return;
		}
	}
	uniformInts.push_back(value);
	intNames.push_back(name);
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->SetUniformInt(name, value);
	}
}

void SceneNode::SetUniformFloat(string name, float value) {
	for (int i = 0; i < uniformFloats.size(); i++) {
		if (floatNames[i].compare(name) == 0) {
			uniformFloats[i] = value;
			for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
				(*i)->SetUniformFloat(name, value);
			}
			return;
		}
	}
	uniformFloats.push_back(value);
	floatNames.push_back(name);
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->SetUniformFloat(name, value);
	}
}

void SceneNode::SetUniformVec2(string name, Vector2 value) {
	for (int i = 0; i < uniformVec2s.size(); i++) {
		if (vec2Names[i].compare(name) == 0) {
			uniformVec2s[i] = value;
			for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
				(*i)->SetUniformVec2(name, value);
			}
			return;
		}
	}
	uniformVec2s.push_back(value);
	vec2Names.push_back(name);
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->SetUniformVec2(name, value);
	}
}

void SceneNode::SetUniformVec3(string name, Vector3 value) {
	for (int i = 0; i < uniformVec3s.size(); i++) {
		if (vec3Names[i].compare(name) == 0) {
			uniformVec3s[i] = value;
			for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
				(*i)->SetUniformVec3(name, value);
			}
			return;
		}
	}
	uniformVec3s.push_back(value);
	vec3Names.push_back(name);
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->SetUniformVec3(name, value);
	}
}

void SceneNode::SetUniformVec4(string name, Vector4 value) {
	for (int i = 0; i < uniformVec4s.size(); i++) {
		if (vec4Names[i].compare(name) == 0) {
			uniformVec4s[i] = value;
			for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
				(*i)->SetUniformVec4(name, value);
			}
			return;
		}
	}
	uniformVec4s.push_back(value);
	vec4Names.push_back(name);
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->SetUniformVec4(name, value);
	}
}

void SceneNode::SetUniformMat4(string name, Matrix4 value) {
	for (int i = 0; i < uniformMat4s.size(); i++) {
		if (mat4Names[i].compare(name) == 0) {
			uniformMat4s[i] = value;
			for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
				(*i)->SetUniformMat4(name, value);
			}
			return;
		}
	}
	uniformMat4s.push_back(value);
	mat4Names.push_back(name);
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->SetUniformMat4(name,value);
	}
}

void SceneNode::Draw(const OGLRenderer &r) {
	if (!active) {
		return;
	}
	if (shader) {
		for (int i = 0; i < uniformInts.size(); i++) {
			glUniform1i(glGetUniformLocation(shader->GetProgram(), intNames[i].c_str()), uniformInts[i]);
		}
		for (int i = 0; i < uniformFloats.size(); i++) {
			glUniform1f(glGetUniformLocation(shader->GetProgram(), floatNames[i].c_str()), uniformFloats[i]);
		}
		for (int i = 0; i < uniformVec2s.size(); i++) {
			glUniform2fv(glGetUniformLocation(shader->GetProgram(), vec2Names[i].c_str()), 1, (GLfloat*)&uniformVec2s[i]);
		}
		for (int i = 0; i < uniformVec3s.size(); i++) {
			glUniform3fv(glGetUniformLocation(shader->GetProgram(), vec3Names[i].c_str()), 1, (GLfloat*)&uniformVec3s[i]);
		}
		for (int i = 0; i < uniformVec4s.size(); i++) {
			glUniform4fv(glGetUniformLocation(shader->GetProgram(), vec4Names[i].c_str()), 1, (GLfloat*)&uniformVec4s[i]);
		}
		for (int i = 0; i < uniformMat4s.size(); i++) {
			glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), mat4Names[i].c_str()), 1, false, (GLfloat*)&uniformMat4s[i]);
		}
	}


	if (mesh) { mesh->Draw(); }
}


void SceneNode::Update(float msec) {
	if (parent) {
		worldTransform = parent -> worldTransform* transform;
	}
	else {
		worldTransform = transform;
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i) -> Update(msec);
	}
}
