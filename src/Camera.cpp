#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up) :
    _front(glm::vec3(0.0f, 0.0f, -1.0f)),
    _speed(SPEED),
    _sensitivity(SENSITIVITY),
    _zoom(ZOOM),
    _position(position),
    _worldUp(up)
{
    updateCameraAttributes();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(_position, _position + _front, _up);
}

void Camera::processKeyPress(CameraDirection direction, float deltaTime)
{
    float v = _speed * deltaTime;
    if (direction == FORWARD) 
        _position += _front * v;
    if (direction == BACKWARD)
        _position -= _front * v;
    if (direction == LEFT)
        _position -= _right * v;
    if (direction == RIGHT)
        _position += _right * v;
    if (direction == UP)
        _position += _up * v;
    if (direction == DOWN)
        _position -= _up * v;

    updateCameraAttributes();
}

void Camera::processMouseScroll(float offset)
{
    _zoom -= (float)offset;
    if ( _zoom < 1.0f )
        _zoom = 1.0f;
    if ( _zoom > 45.0f )
        _zoom = 45.0f;

    fprintf(stderr, "Zoom; %Lf\n", _zoom);
}

void Camera::updateCameraAttributes()
{
    // Front should always point to origin.
    _front = glm::normalize(glm::vec3(0.0f) - _position);

    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
}