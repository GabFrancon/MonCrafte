#define _USE_MATH_DEFINES

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "World.h"
#include "Camera.h"

// general
GLFWwindow* window = nullptr;
GLuint program = 0;
Camera camera;
World world;

// shaders
Shader worldShader;
Shader playerShader;
Shader pointerShader;
Shader skyShader;

// time
float currentFrame = 0.f;
float lastFrame    = 0.f;
float lastTime     = 0.f;
int nbFrames       = 0;

// mouse
float lastX = 0.f;
float lastY = 0.f;
bool  firstMouse = true;

// textures
GLuint arrayTexture;
std::map<std::string, Texture> textures;
int currentSpotInArrayTexture = 0;

void initializeTextureArray()
{
    glGenTextures(1, &arrayTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA,
        512,
        512,
        100,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void loadTexture(const std::string& filename, const std::string& name)
{
    Texture tex = Texture();
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // fills the GPU texture with the data stored in the CPU image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);  
    glBindTexture(GL_TEXTURE_2D, 0);

    // adds the texture to the array
    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
        0,                      //Mipmap number
        0, 0, currentSpotInArrayTexture,   //xoffset, yoffset, zoffset
        width, height, 1,                 //width, height, depth
        GL_RGBA,                //format
        GL_UNSIGNED_BYTE,       //type
        data); //pointer to data

    tex.setTexID(texID);
    tex.setLocationInArray(currentSpotInArrayTexture++);
    textures[name] = tex;

    // Freeing the now useless CPU memory
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    stbi_image_free(data);
}

GLuint loadCubemap(std::vector<std::string> faces, bool withAlpha = false)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            if (withAlpha)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

    return textureID;
}
// Executed each time the window is resized.
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    camera.setAspectRatio(window);
    camera.bindProjection(worldShader, playerShader, skyShader);
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
        world.destroyBlock();
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        world.addBlock(camera.getCurrentBlock());
}


// Executed each time a scroll is proceed.
void scrollCallback(GLFWwindow* window, double xoDffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}

// Executed each time an exception is raised.
void errorCallback(int error, const char* desc)
{
    std::cerr << "Error " << error << ": " << desc << std::endl;
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
    size_t height = 1200;
    size_t width = 800;
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
    // culling parameters
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // z-buffer parameters
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // blending parameters
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void loadTextures()
{
    loadTexture("texture/512x512/sand.bmp", "sand");
    loadTexture("texture/512x512/dirt.bmp", "dirt");
    loadTexture("texture/512x512/gravel.bmp", "gravel");
    loadTexture("texture/512x512/brick.bmp", "brick");
    loadTexture("texture/512x512/woodplanck.bmp", "woodplanck");
    loadTexture("texture/512x512/stone.bmp", "stone");
    loadTexture("texture/512x512/water.bmp", "water+");
    loadTexture("texture/512x512/leaves.bmp", "leaves+");
    loadTexture("texture/512x512/font.bmp", "font+");
}

void setupShaders()
{
    worldShader   = Shader("shader/chunkVertexShader.glsl", "shader/chunkFragShader.glsl");
    playerShader  = Shader("shader/playerVertexShader.glsl", "shader/playerFragShader.glsl");
    pointerShader = Shader("shader/textVertexShader.glsl", "shader/textFragShader.glsl");
    skyShader     = Shader("shader/skyboxVertexShader.glsl", "shader/skyboxFragShader.glsl");

    pointerShader.use();
    pointerShader.setInt("material.textureData", 0);

    worldShader.use();
    worldShader.setInt("material.textureArray", 0);

    playerShader.use();
    playerShader.setInt("material.textureData", 0);

    skyShader.use();
    skyShader.setInt("cubemap", 0);

    camera.bindProjection(worldShader, playerShader, skyShader);
}


void update()
{
    camera.updateCamPos(window, currentFrame - lastFrame, world);
    world.updateSelection(camera.getPosition(), camera.getViewDirection());

    // Measure speed
    nbFrames++;
    if ((double)currentFrame - lastTime >= 1.0)
    {
        printf("%f ms/frame\n", 1000.0 / double(nbFrames));
        nbFrames = 0;
        lastTime += 1.0;
    }
}

void render()
{
    camera.bindView(worldShader, playerShader, skyShader);
    world.bindLights(worldShader, playerShader);

    // render world
    world.render(worldShader, skyShader, camera.getPosition(), camera.getViewDirection());

    // render camera
    camera.render(playerShader, pointerShader, world);
}

void clear()
{
    world.clearBuffers();
    camera.clearBuffers();
  
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    initGLFW();
    initOpenGL();
    initializeTextureArray();
    loadTextures();

    std::vector<std::string> faces = {
    "texture/skybox/default/right.bmp",
    "texture/skybox/default/left.bmp",
    "texture/skybox/default/top.bmp",
    "texture/skybox/default/bottom.bmp",
    "texture/skybox/default/front.bmp",
    "texture/skybox/default/back.bmp" };

    // setup the world (ground + light)
    world = World(textures, arrayTexture, loadCubemap(faces));
    world.genWorld();

    // setup the camera (player + pointer)
    camera = Camera(
        world,
        glm::vec3(0.0, 10.0, 0.0),  // position
        glm::vec3(0.0, 0.0, -1.0), // front vector
        glm::vec3(0.0, 1.0, 0.0),  // up vector
        textures["font+"].getTexID());

    camera.setAspectRatio(window);
    camera.insertBlock("sand", 0);
    camera.insertBlock("dirt", 1);
    camera.insertBlock("gravel", 2);
    camera.insertBlock("woodplanck", 3);
    camera.insertBlock("brick", 4);
    camera.insertBlock("water+", 5);
    camera.insertBlock("leaves+", 6);
    camera.insertBlock("stone", 7);

    // finally send all the data to the shaders
    setupShaders();

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