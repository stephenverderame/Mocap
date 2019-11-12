#include "Shader.h"
#include <fstream>
#include <sstream>

void Shader::load(const char * vertexCode, const char * fragCode, const char * geomCode, FILE * msgOut)
{
	unsigned int v, f, g;
	int success;
	char infoLog[512];
	v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, &vertexCode, NULL);
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(v, 512, NULL, infoLog);
		fprintf(msgOut, "%s\n", infoLog);
	}
	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f, 1, &fragCode, NULL);
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(f, 512, NULL, infoLog);
		fprintf(msgOut, "%s\n", infoLog);
	}
	if (geomCode != nullptr) {
		g = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(g, 1, &geomCode, NULL);
		glCompileShader(g);
		glGetShaderiv(g, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(g, 512, NULL, infoLog);
			fprintf(msgOut, "%s\n", infoLog);
		}
	}
	program = glCreateProgram();
	glAttachShader(program, v);
	glAttachShader(program, f);
	if (geomCode != nullptr) glAttachShader(program, g);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(msgOut, "%s\n", infoLog);
	}
	glDeleteShader(v);
	glDeleteShader(f);
	if (geomCode != nullptr) glDeleteShader(g);
}

Shader::Shader()
{
}

Shader::Shader(const char * vertPath, const char * fragPath, const char * geomPath)
{
	std::ifstream in;
	in.open(vertPath);
	std::stringstream sv, sf, sg;
	sv << in.rdbuf();
	in.close();
	in.open(fragPath);
	sf << in.rdbuf();
	in.close();
	if (geomPath != nullptr) {
		in.open(geomPath);
		sg << in.rdbuf();
		in.close();
	}
	load(sv.str().c_str(), sf.str().c_str(), geomPath == nullptr ? nullptr : sg.str().c_str());
}

Shader::~Shader()
{
	glDeleteProgram(program);
}

Rect::Rect()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

Rect::Rect(float x, float y, float width, float height) : Rect()
{
	model = glm::translate(model, glm::vec3(x, y, 0));
	model = glm::scale(model, glm::vec3(width, height, 1.0));
}

Rect::~Rect()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Rect::draw(const Shader & s)
{
	glBindVertexArray(vao);
	s.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
