#define _USE_MATH_DEFINES
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#include "Block.h"
#include "Sphere.h"
#include "Camera.h"
#include "Light.h"
#include "World.h"

// general
GLFWwindow* window = nullptr;
GLuint program = 0;
Camera camera;
Light light;
World world;

// 3D meshes
auto block = std::make_shared<Block>();
auto sun = std::make_shared<Sphere>();

// textures
GLuint blockTexture = 0;
GLuint sunTexture = 0;

// time
float currentFrame = 0.f;
float lastFrame    = 0.f;

// mouse
float lastX;
float lastY;
bool firstMouse = true;

// Executed each time the window is resized.
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    camera.setAspectRatio(window);
    glViewport(0, 0, (GLint)width, (GLint)height);
}

// Executed each time a key is pressed.
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
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMoovement(xoffset, yoffset);
}

// Executed each time a mouse button is clicked.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        int selection = world.select(camera.getPosition(), camera.getViewDirection());
        if (selection >= 0)
            world.destroyBlock(selection);
    }
}


// Executed each time a scroll is proceed.
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}

// Executed each time an exception is raised.
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
    size_t height = 2000;
    size_t width = 1400;
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
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGL()
{
    // Load extensions for modern OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
    glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string& filename)
{
    std::ifstream t(filename.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

void loadShader(GLuint program, GLenum type, const std::string& shaderFilename)
{
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

GLuint loadTextureFromFileToGPU(const std::string& filename)
{
    int width, height, numComponents;

    // Loading the image in CPU memory using stb_image
    unsigned char* data = stbi_load(
        filename.c_str(),
        &width, &height,
        &numComponents,
        0);

    GLuint texID;
    // creates a texture and upload the image data in GPU memory
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // setup texture filtering option and repeat model
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // fills the GPU texture with the data stored in the CPU image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Freeing the now useless CPU memory
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

void update()
{
    float time = 2 * M_PI * currentFrame;
    light.setPos( glm::vec3(6 * std::cos(time / 10), 6 * std::sin(time / 10), 0.0));
}

void render()
{
    // render camera
    camera.render(window, program, currentFrame - lastFrame, world);
    // render light
    light.render(program, sunTexture, sun);
    // render world
    world.render(program, blockTexture, block);

    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), camera.getPosition() + camera.getViewDirection()), glm::vec3(0.02));
    block->render(program, model, sunTexture);
}

void clear()
{
    block->freeBuffer();
    sun->freeBuffer();
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    initGLFW();
    initOpenGL();
    initGPUprogram();

    // setup the world
    world = World();
    world.addBlock(glm::vec3( 0.0f, 0.0f, 0.0f));
    world.addBlock(glm::vec3( 1.0f, 0.0f, 0.0f));
    world.addBlock(glm::vec3(-1.0f, 0.0f, 0.0f));
    world.addBlock(glm::vec3( 0.0f, 0.0f, 1.0f));
    world.addBlock(glm::vec3( 0.0f, 0.0f,-1.0f));
    world.addBlock(glm::vec3(-1.0f, 0.0f,-1.0f));
    world.addBlock(glm::vec3(-1.0f, 0.0f, 1.0f));
    world.addBlock(glm::vec3( 1.0f, 0.0f,-1.0f));
    world.addBlock(glm::vec3( 1.0f, 0.0f, 1.0f));

    // setup the camera
    camera = Camera(
        glm::vec3(0.0, 1.0, 0.0),   // initial position
        glm::vec3(0.0, 0.0, -1.0),  // front vector
        glm::vec3(0.0, 1.0, 0.0));  // up vector
    camera.setAspectRatio(window);

    // setup the light
    light = Light(
        glm::vec3(6.0, 6.0, 6.0),   // initial position
        glm::vec3(1.0, 1.0, 1.0));  // color

    // setup 3D geometry and texture
    block->initGeometry();
    blockTexture = loadTextureFromFileToGPU("texture/brick.png");
    sun->initGeometry();
    sunTexture = loadTextureFromFileToGPU("texture/light.jpg");
    
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lastFrame = currentFrame;
        currentFrame = glfwGetTime();
        update();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    clear();
    return EXIT_SUCCESS;
}