/*
#include "Context.h"
#include "Shader.h"
#include <memory>
int overlaymain() {
	Context ctx;
	std::unique_ptr<VRTex> renderTexture;
	std::unique_ptr<Overlay> vrOverlay;
	std::unique_ptr<Shader> fboShader;
	std::unique_ptr<Rect> fboRect;
	glViewport(0, 0, 1920, 1080);
	if (ctx.isVrEnabled()) {
		printf("Vr!\n");
		renderTexture = std::make_unique<VRTex>(1920, 1080);
		vrOverlay = std::make_unique<Overlay>("TestOverlay");
		vrOverlay->show();
		fboShader = std::make_unique<Shader>("fbovert.glsl", "fbofrag.glsl");
		fboShader->use();
		fboShader->setMat4("projection", glm::ortho(0.0f, 1920.f, 1080.f, 0.0f, -1.0f, 0.0f));
		fboShader->setInt("tex", 0);
		fboRect = std::make_unique<Rect>(0, 0, 1920, 1080);
	}
	glViewport(0, 0, 1920, 1080);
	Shader shader("vertex.glsl", "fragment.glsl");
	shader.use();
	shader.setMat4("projection", glm::ortho(0.0f, 1920.f, 1080.f, 0.0f, -1.0f, 0.0f));
	Rect r(0, 0, 400, 400);
	while (true) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.f, 0.f, 1.f, 0.0f);
		r.draw(shader);
		if (ctx.isVrEnabled()) {
//			vrOverlay->clear();
			vrOverlay->render(renderTexture->getRenderedTexture());

			//for displaying onto the screen
/*			renderTexture->unbind();
			fboShader->use();
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.f, 1.f, 0.f, 1.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, renderTexture->getRenderedTexture());
			fboRect->draw(*fboShader);
			renderTexture->bind();
			shader.use();*/
/*

		}
		ctx.updateScreen();
	}
}*/