#include "Scene.hpp"
#include "debug.hpp"
#include <string>

float frameCubeVerts[] = {
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
};

Scene::Scene()
{
    _lights[0] = { glm::vec3(300.0f, 150.0f, 0.0f), glm::vec3{-1.0f,  1.0f, 5.0f}};
    _lights[1] = { glm::vec3(0.0f, 150.0f, 300.0f), glm::vec3{ 1.0f,  1.0f, 5.0f}};
    _lights[2] = { glm::vec3(0.0f, 300.0f, 150.0f), glm::vec3{-1.0f, -1.0f, 5.0f}};
    _lights[3] = { glm::vec3(150.0f, 0.0f, 300.0f), glm::vec3{ 1.0f, -1.0f, 5.0f}};
    _model = new Model();
    resetModelMatrix();

    /* Setup frambuffer rendering */
    glGenVertexArrays(1, &_frameVao);
    glGenBuffers(1, &_frameVbo);
    glBindVertexArray(_frameVao);
    glBindBuffer(GL_ARRAY_BUFFER, _frameVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frameCubeVerts), &frameCubeVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    _screenShader = new Shader();
    std::string screenVert = "final.vert";
    std::string screenNoFX = "nofx.frag";
    _screenShader->add(GL_VERTEX_SHADER, screenVert);
    _screenShader->add(GL_FRAGMENT_SHADER, screenNoFX);
    _screenShader->link();

    _framebuffer = new Framebuffer();
}

Scene::~Scene()
{
    if (_model != nullptr) delete _model;
    if (_skybox != nullptr) delete _skybox;
    if (_screenShader != nullptr) delete _screenShader;
    if (_framebuffer != nullptr) delete _framebuffer;
}

void Scene::render(glm::mat4& view, glm::mat4& proj, glm::vec3& camPos)
{
    /* Calcualte env map textures */

    /* Render the scene */
    _framebuffer->setFramebuffer();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (_model->isLoaded())
    {
        _model->useShader();
        for (size_t i = 0; i < sizeof(_lights) / sizeof(_lights[0]); ++i)
        {
            _model->getShader()->setVec3("lights[" + std::to_string(i) + "].position", _lights[i].pos);
            _model->getShader()->setVec3("lights[" + std::to_string(i) + "].color", _lights[i].color);
        }
        _model->draw(_modelMatrix, view, proj, camPos);
    }
    // Render Skybox Env Map
    _framebuffer->unsetFramebuffer();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    _screenShader->use();
    glBindVertexArray(_frameVao);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE10);
    _screenShader->setInt("screenTexture", 10);
    glBindTexture(GL_TEXTURE_2D, _framebuffer->getFramebufferTexture());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Render framebuffer texture */

}

void Scene::setLightColor(size_t idx, glm::vec3 color)
{
    if (idx > 3) {
        DEBUG("idx %zu is out of range for size 4\n", idx);
        return;
    }

    _lights[idx].color = color;
}

void Scene::setLightPosition(size_t idx, glm::vec3 pos)
{
    if (idx > 3) {
        DEBUG("idx %zu is out of range for size 4\n", idx);
        return;
    }

    _lights[idx].pos = pos;
}

void Scene::loadModel(std::string filepath)
{
    // File existence check here
    DEBUG("Loading model %s\n", filepath.c_str());

    Model *model_holder = new Model();
    model_holder->load(filepath);

    // Check model was loaded properly
    DEBUG("Model successfully loaded!");
    delete _model;
    _model = model_holder;
}

bool Scene::isModelLoaded()
{
    return _model->isLoaded();
}

void Scene::resizeFramebuffer()
{
    if (_framebuffer != nullptr) delete _framebuffer;
    _framebuffer = new Framebuffer();
}
