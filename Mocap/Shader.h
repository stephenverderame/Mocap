#pragma once
#include <glad\glad.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <stdio.h>
class Shader
{
protected:
	unsigned int program;
protected:
	void load(const char * vertexCode, const char * fragCode, const char * geomCode = nullptr, FILE * msgOut = stdout);
public:
	Shader();
	Shader(const char * vertPath, const char * fragPath, const char * geomPath = nullptr);
//	Shader(int shaderType, int vertexId, int fragId, int geomId = -1);
//	Shader(StdResources & res, int vertexId, int fragId, int geomId = -1);
	~Shader();

	inline void use() const { glUseProgram(program); }
	inline void setBool(const char * name, bool value) const { glUniform1i(glGetUniformLocation(program, name), value); }
	inline void setInt(const char * name, int value) const { glUniform1i(glGetUniformLocation(program, name), value); }
	inline void setFloat(const char * name, float value) const { glUniform1f(glGetUniformLocation(program, name), value); }
	inline void setVec3(const char * name, glm::vec3 value) const { glUniform3f(glGetUniformLocation(program, name), value.x, value.y, value.z); }
	inline void setVec4(const char * name, glm::vec4 value) const { glUniform4f(glGetUniformLocation(program, name), value.x, value.y, value.z, value.w); }
	inline void setMat3(const char * name, glm::mat3 value) const { glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(value)); }
	inline void setMat4(const char * name, glm::mat4 value) const { glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(value)); }
	inline void setFArray(const char * name, const float * data, size_t length) const { glUniform1fv(glGetUniformLocation(program, name), length, data); }
	inline void setMArray(const char * name, const glm::mat4 * data, size_t length) const {
		glUniformMatrix4fv(glGetUniformLocation(program, name), length, GL_FALSE, glm::value_ptr(data[0]));
	}
};


