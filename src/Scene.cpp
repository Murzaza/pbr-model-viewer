#include "Scene.hpp"

Scene::Scene()
{
    _lights[0] = { glm::vec3(300.0f, 150.0f, 0.0f), glm::vec3{-1.0f,  1.0f, 5.0f}};
    _lights[1] = { glm::vec3(0.0f, 150.0f, 300.0f), glm::vec3{ 1.0f,  1.0f, 5.0f}};
    _lights[2] = { glm::vec3(0.0f, 300.0f, 150.0f), glm::vec3{-1.0f, -1.0f, 5.0f}};
    _lights[3] = { glm::vec3(150.0f, 0.0f, 300.0f), glm::vec3{ 1.0f, -1.0f, 5.0f}};
    _model = new Model();
    resetModelMatrix();
}

Scene::~Scene()
{
    if (_model != nullptr) delete _model;
    if (_skybox != nullptr) delete _skybox;
}

void Scene::render(glm::mat4& view, glm::mat4& proj, glm::vec3& camPos)
{
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
}

void Scene::setLightColor(size_t idx, glm::vec3 color)
{
    if (idx > 3) {
        fprintf(stderr, "idx %zu is out of range for size 4\n", idx);
        return;
    }

    _lights[idx].color = color;
}

void Scene::setLightPosition(size_t idx, glm::vec3 pos)
{
    if (idx > 3) {
        fprintf(stderr, "idx %zu is out of range for size 4\n", idx);
        return;
    }

    _lights[idx].pos = pos;
}

void Scene::loadModel(std::string filepath)
{
    // File existence check here
    fprintf(stderr, "Loading model %s\n", filepath.c_str());

    Model *model_holder = new Model();
    model_holder->load(filepath);

    // Check model was loaded properly
    fprintf(stderr, "Model successfully loaded!");
    delete _model;
    _model = model_holder;
}

bool Scene::isModelLoaded()
{
    return _model->isLoaded();
}
