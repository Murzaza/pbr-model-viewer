#ifndef APP
#define APP 

#include "util.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"


#include <GLFW/glfw3.h>
#include <string>

class App
{
public:
    App() = delete;
    App(uint32_t width, uint32_t height, std::string title);
    ~App();

    void start();

    //Static members for access from callbacks from glfw
    uint32_t _width;
    uint32_t _height;

    Camera *camera;
    float lastX = 0;
    float lastY = 0;
    bool firstMouse = true;
    float _deltaTime = 0;
    float lastFrame = 0;

    //Callbacks
    void resize_window(GLFWwindow* window, int width, int height);
    void process_input(GLFWwindow* window);
    void process_mouse(GLFWwindow* window, double xPosition, double yPosition);
    void process_scroll(GLFWwindow* window, double xOffset, double yOffset);

private:
    GLFWwindow* _window;
    Renderer* renderer;

    //Model model
    Shader* model_shader;


};

void handle_mouse_events(GLFWwindow* window, double x, double y);
void handle_scroll_events(GLFWwindow* window, double x, double y);
void handle_resize(GLFWwindow* window, int w, int h);

#endif