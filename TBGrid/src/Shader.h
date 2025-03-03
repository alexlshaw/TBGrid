#pragma once

#include <format>
#include <string>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"

using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

enum GLSLShaderType {
	VERTEX,
	FRAGMENT,
	GEOMETRY
};

class Shader
{
private:
	int vs;
	int fs;
	int gs;
	int tcs;
	int tes;
	GLuint handle;
	bool linked, validated;
	string shaderName;
	string logString;
	bool fileExists(const string& fileName);	//While files are handled in a bunch of places, this shader code is used in many applications, so we keep this self contained
public:
	Shader();
	~Shader();

	bool compileShaderFromFile(const char* fileName, GLSLShaderType type);
	bool compileShaderFromString(const string& source, GLSLShaderType type);
	bool compileShaderPair(const string& vertexFileName, const string& fragmentFileName);
	bool link();
	bool validate();
	bool linkAndValidate();
	void use() const;

	string getLog() const;
	GLuint getHandle() const;
	bool isLinked() const;

	void bindAttribLocation(GLuint location, const char* name) const;
	void bindFragDataLocation(GLuint location, const char* name) const;

	int getUniformLocation(const char* name) const;
	int getUniformBlockLocation(const char* name) const;
	void setUniform(GLint location, float x, float y);
	void setUniform(GLint location, float x, float y, float z);
	void setUniform(GLint location, const vec2& v);
	void setUniform(GLint location, const vec3& v);
	void setUniform(GLint location, const vec4& v);
	void setUniform(GLint location, const mat4& m);
	void setUniform(GLint location, const std::vector<mat4>& m);
	void setUniform(GLint location, const mat3& m);
	void setUniform(GLint location, float val);
	void setUniform(GLint location, int val);
	void setUniform(GLint location, bool val);

	void setUniform(const char* location, const vec3& v) const;
	void setUniform(const char* location, const vec4& v) const;
	void setUniform(const char* location, const float f) const;
	void setUniform(const char* location, const int i) const;
	void setUniform(const char* location, const mat3& m) const;
	void setUniform(const char* location, const mat4& m) const;
	void setUniform(const char* location, const std::vector<mat4>& m) const;

	void printActiveUniforms() const;
	void printActiveAttribs() const;
};