#define _USE_MATH_DEFINES

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "World.h"
#include "Camera.h"

// general
GLFWwindow* window = nullptr;
glm::vec2 windowSize(1200, 800);
GLuint program = 0;
Camera camera;
World world;
LightPtr light;

// shaders
Shader worldShader;
Shader playerShader;
Shader pointerShader;
Shader skyShader;
Shader shadowMapShader;

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
int currentSpotInArrayTex = 0;

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

GLuint loadTexture(const std::string& filename)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // fills the GPU texture with the data stored in the CPU image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    // adds the texture to the array
    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
        0,                      //Mipmap number
        0, 0, currentSpotInArrayTex,   //xoffset, yoffset, zoffset
        width, height, 1,                 //width, height, depth
        GL_RGBA,                //format
        GL_UNSIGNED_BYTE,       //type
        data); //pointer to data

    // Freeing the now useless CPU memory
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    stbi_image_free(data);

    return texID;
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

FboShadowMap allocateShadowMapFbo(unsigned int width = 1024, unsigned int height = 768)
{
    FboShadowMap fbo;
    glGenFramebuffers(1, &fbo._depthMapFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo._depthMapFbo);

    fbo._depthMapTextureWidth = width;
    fbo._depthMapTextureHeight = height;

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &fbo._depthMapTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbo._depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo._depthMapTexture, 0);

    glDrawBuffer(GL_NONE);      // No color buffers are written.

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        std::cout << "PROBLEM IN FBO FboShadowMap::allocate(): FBO NOT successfully created" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return fbo;
}

// Executed each time the window is resized.
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    windowSize.x = width;
    windowSize.y = height;
    camera.setAspectRatio(windowSize, pointerShader);
    camera.bindProjection(worldShader, playerShader, skyShader);
    glViewport(0, 0, (GLint)width, (GLint)height);
}

// Executed each time a key is pressed.
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_E)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    else if (action == GLFW_PRESS && key == GLFW_KEY_F)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    else if (action == GLFW_PRESS && key == GLFW_KEY_G)
        world.saveShadowMap();

    else if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);
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
    window = glfwCreateWindow(windowSize.x, windowSize.y, "MonCrafte", nullptr, nullptr);

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
    Texture sandTex, dirtTex, grassTex, gravelTex, brickTex, woodplanckTex, stoneTex, woodTex, waterTex, leavesTex, fontTex, selTex;

    GLuint sand(loadTexture("texture/512x512/sand.bmp"));
    sandTex.addSample(sand, currentSpotInArrayTex++);

    GLuint sideGrass(loadTexture("texture/512x512/grass_side.bmp"));
    grassTex.addSample(sideGrass, currentSpotInArrayTex++);

    GLuint topGrass(loadTexture("texture/512x512/grass_top.bmp"));
    grassTex.addSample(topGrass, currentSpotInArrayTex++);

    GLuint dirt(loadTexture("texture/512x512/dirt.bmp"));
    grassTex.addSample(dirt, currentSpotInArrayTex);
    dirtTex.addSample(dirt, currentSpotInArrayTex++);

    GLuint gravel(loadTexture("texture/512x512/gravel.bmp"));
    gravelTex.addSample(gravel, currentSpotInArrayTex++);

    GLuint brick(loadTexture("texture/512x512/brick.bmp"));
    brickTex.addSample(brick, currentSpotInArrayTex++);

    GLuint woodplanck(loadTexture("texture/512x512/woodplanck.bmp"));
    woodplanckTex.addSample(woodplanck, currentSpotInArrayTex++);

    GLuint stone(loadTexture("texture/512x512/stone.bmp"));
    stoneTex.addSample(stone, currentSpotInArrayTex++);

    GLuint sideWood(loadTexture("texture/512x512/wood_side.bmp"));
    woodTex.addSample(sideWood, currentSpotInArrayTex++);

    GLuint topWood(loadTexture("texture/512x512/wood_top.bmp"));
    woodTex.addSample(topWood, currentSpotInArrayTex);
    woodTex.addSample(topWood, currentSpotInArrayTex++);

    GLuint water(loadTexture("texture/512x512/water.bmp"));
    waterTex.addSample(water, currentSpotInArrayTex++);

    GLuint leaves(loadTexture("texture/512x512/leaves.bmp"));
    leavesTex.addSample(leaves, currentSpotInArrayTex++);

    GLuint font(loadTexture("texture/512x512/font.bmp"));
    fontTex.addSample(font, currentSpotInArrayTex++);

    GLuint selection(loadTexture("texture/512x512/selection.png"));
    selTex.addSample(selection, currentSpotInArrayTex++);

    sandTex.setType(Type::SOLID);
    dirtTex.setType(Type::SOLID);
    grassTex.setType(Type::SOLID);
    gravelTex.setType(Type::SOLID);
    brickTex.setType(Type::SOLID);
    woodplanckTex.setType(Type::SOLID);
    stoneTex.setType(Type::SOLID);
    woodTex.setType(Type::SOLID);
    waterTex.setType(Type::TRANSPARENT);
    leavesTex.setType(Type::TRANSPARENT);
    fontTex.setType(Type::TRANSPARENT);
    selTex.setType(Type::TRANSPARENT);

    textures["sand"]       = sandTex;
    textures["dirt"]       = dirtTex;
    textures["grass"]      = grassTex;
    textures["gravel"]     = gravelTex;
    textures["brick"]      = brickTex;
    textures["woodplanck"] = woodplanckTex;
    textures["stone"]      = stoneTex;
    textures["wood"]       = woodTex;
    textures["water"]      = waterTex;
    textures["leaves"]     = leavesTex;
    textures["font"]       = fontTex;
    textures["selection"]  = selTex;
}

void setupShaders()
{
    worldShader   = Shader("shader/chunkVertexShader.glsl", "shader/chunkFragShader.glsl");
    playerShader  = Shader("shader/playerVertexShader.glsl", "shader/playerFragShader.glsl");
    pointerShader = Shader("shader/textVertexShader.glsl", "shader/textFragShader.glsl");
    skyShader     = Shader("shader/skyboxVertexShader.glsl", "shader/skyboxFragShader.glsl");
    shadowMapShader = Shader("shader/shadowMapVertexShader.glsl", "shader/shadowMapFragShader.glsl");

    pointerShader.use();
    pointerShader.setInt("material.textureData", 0);
    camera.setAspectRatio(windowSize, pointerShader);

    worldShader.use();
    worldShader.setInt("material.textureArray", 0);
    worldShader.setInt("depthMap", 1);

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
    
    float lightX = 100.f * glm::cos(2 * M_PI * currentFrame / 30);
    float lightY = 75.f * std::abs(glm::sin(2 * M_PI * currentFrame / 30));
    float lightZ = 100.f * glm::cos(2 * M_PI * currentFrame / 30);

    light->setPosition(glm::vec3(lightX, lightY, lightZ));

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
    camera.bindView(worldShader, skyShader);
    world.bindLights(worldShader, playerShader);

    // render world
    world.renderForShadowMap(shadowMapShader,camera.getPosition());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world.render(worldShader, skyShader, camera.getPosition(), camera.getViewDirection());

    // render camera
    camera.render(playerShader, pointerShader, world, windowSize);
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

    FboShadowMap fbo = allocateShadowMapFbo(2000, 2000);

    // setup the world (ground + light)
    light = std::make_shared<Light>(
        glm::vec3(100.0, 75.0,100.0),					      // position
        glm::vec3(1.f, 1.f, 1.f), 						      // color
        fbo);                                                 // frame buffer

    world = World(textures, arrayTexture, loadCubemap(faces));
    world.genWorld();
    world.addLight(light);

    // setup the camera (player + pointer)
    camera = Camera(
        glm::vec3(0.0, 25.0, 0.0),  // position
        glm::vec3(0.0, 0.0, -1.0), // front vector
        glm::vec3(0.0, 1.0, 0.0),  // up vector
        textures["font"].getTexID(0));

    camera.insertBlock("sand", 0);
    camera.insertBlock("dirt", 1);
    camera.insertBlock("gravel", 2);
    camera.insertBlock("woodplanck", 3);
    camera.insertBlock("brick", 4);
    camera.insertBlock("water", 5);
    camera.insertBlock("leaves", 6);
    camera.insertBlock("stone", 7);
    camera.insertBlock("wood", 8);
    camera.insertBlock("grass", 9);

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
