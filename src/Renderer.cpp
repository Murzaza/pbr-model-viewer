#include "Renderer.hpp"
#include "util.hpp"

#include <glad/glad.h> 
#include <glm/glm.hpp>


void drawBox(GLuint cubeVAO, Texture& texture, Shader& shader, float scale);
void setupBox(GLuint& cubeVAO, GLuint& cubeVBO);

Renderer::Renderer()
{
	/* DEBUG CUBE */
	std::string vert = "simple.vert";
	std::string frag = "simple.frag";
	std::string tex = "container2.png";
    shader.add(GL_VERTEX_SHADER, vert);
    shader.add(GL_FRAGMENT_SHADER, frag);
	shader.link();
	texture.load(tex);

    setupBox(cubeVAO, cubeVBO);
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
}

void Renderer::render()
{
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Get our view and projection matrices
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera->_zoom), (float)scrnWidth / scrnHeight, 0.1f, 10000.0f);

	shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m = glm::scale(m, glm::vec3(3.0f));
    //drawBox(cubeVAO, texture, shader, 1.0f);
	model->draw(m, view, projection, camera->_position);

	skybox->draw(view, projection);
}

void drawBox(GLuint cubeVAO, Texture& texture, Shader& shader, float scale)
{
	shader.use();
	glm::mat4 model = glm::mat4(0.5f);
	glm::vec3 scaleV = glm::vec3(scale);
	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture._id);
	shader.setInt("material", 0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, scaleV);
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

void setupBox(GLuint& cubeVAO, GLuint& cubeVBO)
{

    // TODO: Move to Mesh/Model
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
#if 0
	/* TRIANGLE */
	glBufferData(GL_ARRAY_BUFFER, sizeof(TRI_VERTS), &TRI_VERTS, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
#endif
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), &CUBE_VERTS, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
    // TODO: Move to Mesh/Model
}
