#pragma once
#include <memory>
class Renderer
{
private:
	std::unique_ptr<class Shader> shader;
public:
	Renderer(int width, int height);
	~Renderer();
	void draw(const class RenderObject & obj);
	void draw(const std::unique_ptr<class RenderObject> & obj);
	void clear();
};

