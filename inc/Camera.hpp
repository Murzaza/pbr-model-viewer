#ifndef CAMERA
#define CAMERA

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Define discreet directions camera can move.
enum CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Camera defaults
const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED       = 20.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM        = 45.0f;

class Camera
{
    
public:
    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _right;
    glm::vec3 _worldUp;

    float _yaw;
    float _pitch;

    float _speed;
    float _sensitivity;
    float _zoom;

    Camera() = delete;
    Camera(glm::vec3 position = glm::vec3(0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW,
           float pitch = PITCH);

    glm::mat4 getViewMatrix();

    void processKeyPress(CameraDirection direction, float deltaTime);
    void processMouse(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float offset);

private:
    void updateCameraAttributes();
};
#endif