#include "App.hpp"
#include "Shader.hpp"

#include <cstdio>
#include <cstdlib>

/* World State Init */
uint32_t scrnWidth;
uint32_t scrnHeight;
Camera* camera;
Skybox* skybox;
Model* model;
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

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(_window, handle_mouse_events);

    glfwSetScrollCallback(_window, handle_scroll_events);
    
    // debug info
    GLint numAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttribs);
    fprintf(stderr, "OpenGL version %s\nMax Vertex Attributes Supported: %d\n", glGetString(GL_VERSION), numAttribs);
    
    //Setup viewport
    glViewport(0, 0, scrnWidth, scrnHeight);

    glfwSetFramebufferSizeCallback(_window, handle_resize);

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    skybox = new Skybox();
    model = new Model();
    renderer = new Renderer();

	std::vector<std::string> cubemap
	{
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg"
	};
	skybox->load(cubemap);

	Shader* skyboxShader = new Shader();
	std::string vert = "skybox.vert";
	std::string frag = "skybox.frag";
	skyboxShader->add(GL_VERTEX_SHADER, vert);
	skyboxShader->add(GL_FRAGMENT_SHADER, frag);
	skyboxShader->link();
	skybox->setShader(skyboxShader);

    std::string modelFile = "Helmet/DamagedHelmet.gltf";
    model->load(modelFile);

}

App::~App()
{
    delete camera;
    delete skybox;
    delete model;
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

        process_input(_window);
        glEnable(GL_DEPTH_TEST);
        renderer->render();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
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
    /*
    TODO: Probably want to not have this in the Renderer. Probably an APP or just the main,
    and on the render we feed it the view and projection matrices
    */
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
}

void App::process_mouse(GLFWwindow* window, double xPosition, double yPosition)
{
    fprintf(stderr, "Processing mouse\n");
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

    camera->processMouse(xOffset, yOffset);
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

