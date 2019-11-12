#pragma once
#include <glm.hpp>
class RenderObject
{
protected:
	glm::mat4 model;
public:
	virtual glm::mat4 getTransform() const { return model; }
	virtual void setTransform(const glm::mat4 & m) { model = m; }
	virtual void setPos(glm::vec3 pos);
	virtual void rotate(glm::vec3 axis, float radians);
	virtual void scale(glm::vec3 scalars);
	virtual void draw() const = 0;
};

class Cube : public RenderObject {
private:
	unsigned int vao, vbo;
	const static float vertices[];
public:
	Cube();
	void draw() const override;
	~Cube();
};

