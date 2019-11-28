#include "Control.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad\glad.h>


Control::Control(std::initializer_list<const char *> textures, unsigned int x, unsigned int y, unsigned int w, unsigned int h) : x(x), y(y), w(w), h(h)
{
	unsigned int i = 0;
	activeTexture = 0;
	glGenTextures(textures.size(), this->textures);
	numTextures = textures.size();
	for (auto it : textures) {
		glBindTexture(GL_TEXTURE_2D, this->textures[i++]);
		int x, y, channels;
		stbi_set_flip_vertically_on_load(true);
		GLubyte * img = stbi_load(it, &x, &y, &channels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(img);
	}
}


Control::~Control()
{
	glDeleteTextures(numTextures, textures);
}

dims Control::getDimensions()
{
	return { x, y, w, h };
}
void Control::bindTarget()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[activeTexture]);
}
