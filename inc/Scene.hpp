#pragma once
#include <glm/glm.hpp>
#include <string>

#include "Skybox.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"

typedef struct Light
{
    glm::vec3 color;
    glm::vec3 pos;
} Light;

class Scene
{
public:
    Scene();
    ~Scene();

    void render(glm::mat4& view, glm::mat4& proj, glm::vec3& camPos);
    void setLightColor(size_t idx, glm::vec3 color);
    void setLightPosition(size_t idx, glm::vec3 pos);
    void loadModel(std::string filepath);
    bool isModelLoaded();
    void loadSkybox(std::string filepath) { return; /* NOT IMPLEMENTED */ }

    void setModelMatrix(glm::mat4 matrix) { _modelMatrix = matrix; };
    glm::mat4 getModelMatrix() { return _modelMatrix; }
    void resetModelMatrix() { _modelMatrix = glm::mat4(1.0f); }

    void resizeFramebuffer();

private:
    Light _lights[4];
    Skybox *_skybox;
    Model *_model;
    glm::mat4 _modelMatrix;

    /* For rendering framebuffer */
    Framebuffer *_framebuffer;
    unsigned int _frameVao;
    unsigned int _frameVbo;
    Shader *_screenShader;
};