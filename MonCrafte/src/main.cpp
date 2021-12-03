#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#include "Block.h"
#include "Camera.h"

GLFWwindow* window = nullptr;
GLuint program = 0;
Camera camera;

// Time
float currentFrame = 0.f;  // Time of current frame
float lastFrame    = 0.f;  // Time of last frame

// Mouse
float lastX;
float lastY;
bool firstMouse = true;
bool onClick = false;

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    // Dimension of the rendering region in the window
    glViewport(0, 0, (GLint)width, (GLint)height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_E) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (action == GLFW_PRESS && key == GLFW_KEY_F) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }
}

// Executed each time the mouse is mooved.
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMoovement(xoffset, yoffset);
}

// Executed each time a scroll is proceed.
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}

void errorCallback(int error, const char* desc)
{
    std::cout << "Error " << error << ": " << desc << std::endl;
}

void initGLFW()
{
    glfwSetErrorCallback(errorCallback);

    // Initialize GLFW, the library responsible for window management
    if (!glfwInit())
    {
        std::cerr << "ERROR: Failed to init GLFW" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Before creating the window, set some option flags
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create the window
    size_t height = 1500;
    size_t width = 900;

    window = glfwCreateWindow(height, width, "MonCrafte", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "ERROR: Failed to open window" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGL()
{
    // Load extensions for modern OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
    glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
    glClearColor(245.f/255, 214.f/255, 175.f/255, 1.f); // specify the background color
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string& filename)
{
    std::ifstream t(filename.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

void loadShader(GLuint program, GLenum type, const std::string& shaderFilename) {
    GLuint shader = glCreateShader(type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
    std::string shaderSourceString = file2String(shaderFilename); // Loads the shader source from a file to a C++ string
    const GLchar* shaderSource = (const GLchar*)shaderSourceString.c_str(); // Interface the C++ string through a C pointer
    glShaderSource(shader, 1, &shaderSource, NULL); // load the vertex shader code
    glCompileShader(shader);
    glAttachShader(program, shader);
    glDeleteShader(shader);
}

void initGPUprogram()
{
    program = glCreateProgram();
    loadShader(program, GL_VERTEX_SHADER, "shader/vertexShader.glsl");
    loadShader(program, GL_FRAGMENT_SHADER, "shader/fragmentShader.glsl");

    glLinkProgram(program);
    glUseProgram(program);
}

void render(std::shared_ptr<Block> block)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 viewMatrix = camera.computeViewMatrix();
    const glm::mat4 projMatrix = camera.computeProjectionMatrix();

    // compute the view matrix of the camera and pass it to the GPU program
    glUniformMatrix4fv(glGetUniformLocation(program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    // compute the projection matrix of the camera and pass it to the GPU program
    glUniformMatrix4fv(glGetUniformLocation(program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix));

    block->draw();
}

void clear(std::shared_ptr<Block> block)
{
    block->freeBuffer();
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    auto block = std::make_shared<Block>();
    block->initGeometry();

    initGLFW();
    initOpenGL();
    initGPUprogram();

    camera = Camera(glm::vec3(0.0, 5.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));

    block->bindToGPU();

    while (!glfwWindowShouldClose(window))
    {
        lastFrame = currentFrame;
        currentFrame = glfwGetTime();
        camera.updateCamPos(window, currentFrame - lastFrame);

        render(block);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    clear(block);
    return EXIT_SUCCESS;
}