#ifndef RENDERER
#define RENDERER

#include "Shader.hpp"
#include "Texture.hpp"
#include "Skybox.hpp"
#include "Model.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void render(glm::mat4 view, glm::mat4 projection, glm::vec3 camPosition);

    //Access to switch post-process affect

private:
    GLuint cubeVAO, cubeVBO;
    Shader shader;
    Texture texture;
    Skybox skybox;
    Model model;
    //Framebuffer framebuffer
};
#endif