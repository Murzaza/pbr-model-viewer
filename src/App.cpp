#include "App.hpp"
#include "Shader.hpp"
#include "Scene.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cstdio>
#include <cstdlib>

/* World State Init */
uint32_t scrnWidth;
uint32_t scrnHeight;
Camera* camera;
Skybox* skybox;
Scene* scene;
ImVec4 light_colors[4] = {
    ImVec4(0.3f, 0.15f, 0.0f, 1.0f),
    ImVec4(0.0f, 0.15f, 0.3f, 1.0f),
    ImVec4(0.0f, 0.3f, 0.15f, 1.0f),
    ImVec4(0.15f, 0.0f, 0.3f, 1.0f)
};

const float LIGHT_SHIFT = 1000.0f;
void set_colors();
/* World State Init end */

App::App(uint32_t width, uint32_t height, std::string title)
{

    // Setup GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    scrnWidth = width;
    scrnHeight = height;
    _window = glfwCreateWindow(scrnWidth, scrnHeight, title.c_str(), nullptr, nullptr);

    if ( _window == nullptr )
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(_window, this);

    //glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(_window, handle_mouse_events);
    glfwSetMouseButtonCallback(_window, handle_mouse_button);

    glfwSetScrollCallback(_window, handle_scroll_events);
    
    // debug info
    GLint numAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttribs);
    fprintf(stderr, "OpenGL version %s\nMax Vertex Attributes Supported: %d\n", glGetString(GL_VERSION), numAttribs);
    
    //Setup viewport
    glViewport(0, 0, scrnWidth, scrnHeight);

    glfwSetFramebufferSizeCallback(_window, handle_resize);

    //Setup IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _io = ImGui::GetIO();

    if (!ImGui_ImplGlfw_InitForOpenGL(_window, true))
    {
        fprintf(stderr, "Unable to init IMGUI for GLFW\n");
        assert(false);
    }
    if (!ImGui_ImplOpenGL3_Init())
    {
        fprintf(stderr, "Unable to init IMGUI for OpenGL\n");
        assert(false);
    }


    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    skybox = new Skybox();
    scene = new Scene();
    renderer = new Renderer();

	std::vector<std::string> cubemap
	{
		"skybox/posx.jpg",
		"skybox/negx.jpg",
		"skybox/posy.jpg",
		"skybox/negy.jpg",
		"skybox/posz.jpg",
		"skybox/negz.jpg"
	};
	skybox->load(cubemap);

	Shader* skyboxShader = new Shader();
	std::string vert = "skybox.vert";
	std::string frag = "skybox.frag";
	skyboxShader->add(GL_VERTEX_SHADER, vert);
	skyboxShader->add(GL_FRAGMENT_SHADER, frag);
	skyboxShader->link();
	skybox->setShader(skyboxShader);
}

App::~App()
{
    delete camera;
    delete skybox;
    delete scene;
    delete renderer;
    glfwTerminate();
}

void App::start()
{
    while( !glfwWindowShouldClose(_window) )
    {
        float currentFrame = glfwGetTime();
        _deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        set_colors();
        process_input(_window);
        glEnable(GL_DEPTH_TEST);
        renderer->render(*scene);

        renderGUI();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void App::renderGUI()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();

    ImGui::NewFrame();
    {
        ImGui::Begin("GUI");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::InputText("Model", &_modelName);
        ImGui::SameLine();
        if (ImGui::Button("Load", ImVec2(100, 20)))
        {
            fprintf(stderr, "Loading model %s\n", _modelName.c_str());
            scene->loadModel(_modelName);
        };
        
        ImGui::ColorEdit3("Light 1 Color", (float *)&light_colors[0]);
        ImGui::ColorEdit3("Light 2 Color", (float *)&light_colors[1]);
        ImGui::ColorEdit3("Light 3 Color", (float *)&light_colors[2]);
        ImGui::ColorEdit3("Light 4 Color", (float *)&light_colors[3]);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::resize_window(GLFWwindow* window, int width, int height)
{
    fprintf(stderr, "Processing resize\n");
    scrnWidth = width;
    scrnHeight = height;
    glViewport(0, 0, scrnWidth, scrnHeight);
}

void App::process_input(GLFWwindow* window)
{
    if (!ImGui::GetIO().WantCaptureKeyboard) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->processKeyPress(FORWARD, _deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->processKeyPress(BACKWARD, _deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->processKeyPress(LEFT, _deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->processKeyPress(RIGHT, _deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera->processKeyPress(UP, _deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera->processKeyPress(DOWN, _deltaTime);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            scene->resetModelMatrix();
        }
    }
}

void App::process_mouse(GLFWwindow* window, double xPosition, double yPosition)
{
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (firstMouse)
        {
            lastX = xPosition;
            lastY = yPosition;
            firstMouse = false;
        }

        float xOffset = xPosition - lastX;
        float yOffset = lastY - yPosition;
        lastX = xPosition;
        lastY = yPosition;

        if (_clicking) {
            glm::vec3 swipe = glm::vec3(-xOffset, -yOffset, 0);
            glm::vec3 rotAxis = glm::normalize(glm::cross(camera->_front, swipe));
            fprintf(stderr, "rotAxis: [%f, %f, %f]\n", rotAxis.x, rotAxis.y, rotAxis.z);
            glm::quat newRot = glm::angleAxis(glm::radians(glm::length(swipe)), rotAxis);
            scene->setModelMatrix(scene->getModelMatrix() * glm::toMat4(newRot)); 
        }
    }
}

void App::process_mouse_button(GLFWwindow* window, int button, int action, int mods)
{
    if (!_io.WantCaptureMouse) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
                _clicking = true;
            else if (action == GLFW_RELEASE)
                _clicking = false;
        }
        fprintf(stderr, "Button is %s\n", _clicking ? "clicking" : "not clicking");
    }
}

void App::process_scroll(GLFWwindow* window, double xOffset, double yOffset)
{
    fprintf(stderr, "Processing scroll\n");
    camera->processMouseScroll(yOffset);
}

void handle_mouse_events(GLFWwindow* window, double x, double y)
{
    fprintf(stderr, "mouse_handler >> %Lf, %Lf\n", x, y);
    static_cast<App*>(glfwGetWindowUserPointer(window))->process_mouse(window, x, y);
}

void handle_mouse_button(GLFWwindow* window, int button, int action, int mods)
{
    fprintf(stderr, "mouse_button_handler >> %d, %d, %d\n", button, action, mods);
    static_cast<App*>(glfwGetWindowUserPointer(window))->process_mouse_button(window, button, action, mods);
}

void handle_scroll_events(GLFWwindow* window, double x, double y)
{
    fprintf(stderr, "scroll_handler >> %Lf, %Lf\n", x, y);
    static_cast<App*>(glfwGetWindowUserPointer(window))->process_scroll(window, x, y);
}

void handle_resize(GLFWwindow* window, int w, int h)
{
    fprintf(stderr, "resize_handler >> %d, %d\n", w, h);
    static_cast<App*>(glfwGetWindowUserPointer(window))->resize_window(window, w, h);
}

void set_colors()
{
    scene->setLightColor(0, glm::vec3(
        light_colors[0].x * LIGHT_SHIFT,
        light_colors[0].y * LIGHT_SHIFT,
        light_colors[0].z * LIGHT_SHIFT));

    scene->setLightColor(1, glm::vec3(
        light_colors[1].x * LIGHT_SHIFT,
        light_colors[1].y * LIGHT_SHIFT,
        light_colors[1].z * LIGHT_SHIFT));

    scene->setLightColor(2, glm::vec3(
        light_colors[2].x * LIGHT_SHIFT,
        light_colors[2].y * LIGHT_SHIFT,
        light_colors[2].z * LIGHT_SHIFT));

    scene->setLightColor(3, glm::vec3(
        light_colors[3].x * LIGHT_SHIFT,
        light_colors[3].y * LIGHT_SHIFT,
        light_colors[3].z * LIGHT_SHIFT));
}
