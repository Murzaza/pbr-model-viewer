#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    _front(glm::vec3(0.0f, 0.0f, -1.0f)),
    _speed(SPEED),
    _sensitivity(SENSITIVITY),
    _zoom(ZOOM),
    _position(position),
    _worldUp(up),
    _yaw(yaw),
    _pitch(pitch)
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
}

void Camera::processMouse(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= _sensitivity;
    yoffset *= _sensitivity;

    _yaw += xoffset;
    _pitch += yoffset;

    if ( constrainPitch )
    {
        if ( _pitch > 89.0f )
            _pitch = 89.0f;
        if ( _pitch < -89.0f )
            _pitch = -89.0f;
    }

    fprintf(stderr, "Yaw %Lf, Pitch %Lf\n", _yaw, _pitch);
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
    _front = glm::normalize(glm::vec3(
        cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)),
        sin(glm::radians(_pitch)),
        sin(glm::radians(_yaw) * cos(glm::radians(_pitch)))
    ));

    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

    _front = glm::normalize(front);

    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));

    fprintf(stderr, "Front: (%f, %f, %f)\nRight: (%f, %f, %f)\nUp: (%f, %f, %f)\n",
        _front.x, _front.y, _front.z,
        _right.x, _right.y, _right.z,
        _up.x, _up.y, _up.z);
}