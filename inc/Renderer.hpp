#ifndef RENDERER
#define RENDERER

#include "World.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Skybox.hpp"
#include "Model.hpp"
#include "Scene.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer();
    ~Renderer();

    //void render();
    void render(Scene& scene);

    //Access to switch post-process affect

private:
    //Framebuffer framebuffer
};
#endif