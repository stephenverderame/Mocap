#pragma once
#include <memory>
class Renderer
{
private:
	std::unique_ptr<class Shader> shader;
	unsigned int fbos[2], texs[2], rbos[2];
	unsigned int w, h;
public:
	Renderer(int width, int height);
	~Renderer();
	void draw(const class RenderObject & obj);
	void draw(const std::unique_ptr<class RenderObject> & obj);
	void clear();
	void bindTarget();
};

