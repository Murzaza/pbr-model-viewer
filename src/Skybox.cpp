#include "Skybox.hpp"

#include <stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

Skybox::Skybox()
{
	glGenTextures(1, &_id);
	setupVAO();
}

Skybox::Skybox(std::vector<std::string> cubemap, bool flip)
{
	glGenTextures(1, &_id);
	setupVAO();
	load(cubemap, flip);
}

Skybox::~Skybox()
{
	glDeleteTextures(1, &_id);
	if (_shader != nullptr)
		delete _shader;
}

void Skybox::setShader(Shader* shader)
{
	_shader = shader;
	_shader->use();
	_shader->setInt("skybox", 0);
}

void Skybox::load(std::vector<std::string> cubemap, bool flip)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

	int width, height, comp;
	stbi_set_flip_vertically_on_load(flip);
	for (unsigned int i = 0; i < cubemap.size(); i++)
	{
		std::string path = PROJECT_SOURCE_DIR "/textures/";
		path += cubemap[i];

		fprintf(stderr, "Loading cubemap: %s\n", path.c_str());
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &comp, 0);
		if (data)
		{
			GLenum format = GL_RGB;
			if (comp == 1)
				format = GL_RED;
			else if (comp == 3)
				format = GL_RGB;
			else if (comp == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			// TODO: We can handle this a lot better.
			fprintf(stderr, "Unable to load cubemap item %s\n", path.c_str());
		}

		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::setupVAO()
{
	glGenVertexArrays(1, &_skyboxVAO);
	glGenBuffers(1, &_skyboxVBO);
	glBindVertexArray(_skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTS), &SKYBOX_VERTS, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

void Skybox::draw(glm::mat4& cam_view, glm::mat4& proj)
{
	glDepthFunc(GL_LEQUAL);
	_shader->use();
	_shader->setInt("skybox", 0);
	glm::mat4 view = glm::mat4(glm::mat3(cam_view));
	_shader->setMat4("view", view);
	_shader->setMat4("projection", proj);
	glBindVertexArray(_skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}