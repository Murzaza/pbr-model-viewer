#ifndef APP
#define APP 

#include "util.hpp"
#include "World.hpp"
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
    void renderGUI();

    // Variables for input
    float lastX = 0;
    float lastY = 0;
    bool firstMouse = true;
    float _deltaTime = 0;
    float lastFrame = 0;

    //Callbacks
    void resize_window(GLFWwindow* window, int width, int height);
    void process_input(GLFWwindow* window);
    void process_mouse(GLFWwindow* window, double xPosition, double yPosition);
    void process_mouse_button(GLFWwindow* window, int button, int action, int mods);
    void process_scroll(GLFWwindow* window, double xOffset, double yOffset);

private:
    GLFWwindow* _window;
    Renderer* renderer;

    Shader* model_shader;

    bool _clicking = false;

};

void handle_mouse_events(GLFWwindow* window, double x, double y);
void handle_mouse_button(GLFWwindow* window, int button, int action, int mods);
void handle_scroll_events(GLFWwindow* window, double x, double y);
void handle_resize(GLFWwindow* window, int w, int h);

#endif