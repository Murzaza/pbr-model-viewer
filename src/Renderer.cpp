#include "Renderer.hpp"
#include "util.hpp"

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


void drawBox(GLuint cubeVAO, Texture& texture, Shader& shader, float scale);
void setupBox(GLuint& cubeVAO, GLuint& cubeVBO);

Renderer::Renderer()
{
	/* NOOP */
}

Renderer::~Renderer()
{
	/* NOOP */
}

#if 0
void Renderer::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Get our view and projection matrices
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera->_zoom), (float)scrnWidth / scrnHeight, 0.1f, 10000.0f);

	if (model->isLoaded()){
		glm::mat4 m = glm::mat4(1.0f); 
		m = m * *rotMatrix;
		model->draw(m, view, projection, camera->_position);
	}

	//skybox->draw(view, projection);
}
#endif

void Renderer::render(Scene& scene)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera->_zoom), (float)scrnWidth / scrnHeight, 0.1f, 10000.0f);

	scene.render(view, projection, camera->_position);	
}