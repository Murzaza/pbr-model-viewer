#ifndef WORLD
#define WORLD

#include "Camera.hpp"
#include "Skybox.hpp"
#include "Model.hpp"

#include <glm/gtc/quaternion.hpp>

extern uint32_t scrnWidth;
extern uint32_t scrnHeight;
extern Camera* camera;
extern Skybox* skybox;
extern Model* model;
extern glm::mat4* rotMatrix;
#endif