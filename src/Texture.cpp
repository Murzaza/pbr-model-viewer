#include "Texture.hpp";

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>

Texture::Texture()
{
	glGenTextures(1, &_id);
}

Texture::Texture(std::string file, bool flip)
{
	glGenTextures(1, &_id);
	load(file, flip);
}

Texture::~Texture()
{
	glDeleteTextures(1, &_id);
}

void Texture::load(std::string file, bool flip)
{
	std::string path = PROJECT_SOURCE_DIR "/textures/";
	path += file;

	int width, height, comp;
	stbi_set_flip_vertically_on_load(flip);
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

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else
	{
		fprintf(stderr, "Failed to load texture from %s", path.c_str());
	}

	stbi_image_free(data);
}
