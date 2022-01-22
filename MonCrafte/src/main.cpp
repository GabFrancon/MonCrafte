#define _USE_MATH_DEFINES

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "World.h"
#include "Camera.h"
#include "Light.h"

// general
GLFWwindow* window = nullptr;
glm::vec2 windowSize(1200, 800);
GLuint program = 0;

// Scene
Camera camera;
World world;
Player player;
Light light;

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
bool survivalMode = false;

// textures
std::map<std::string, Texture> textures;
unsigned int currentSpotInArrayTex = 0;
unsigned int availableTextureSlot = 0;


void addToArray(GLuint &arrayID, const std::string& filename)
{
    int width, height, numComponents;

    // Loading the image in CPU memory using stb_image
    unsigned char* data = stbi_load(
        filename.c_str(),
        &width, &height,
        &numComponents,
        0);

    // adds the texture to the array
    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayID);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
        0,                                //Mipmap number
        0, 0, currentSpotInArrayTex++,    //xoffset, yoffset, zoffset
        width, height, 1,                 //width, height, depth
        GL_RGBA,                          //format
        GL_UNSIGNED_BYTE,                 //type
        data);                            //pointer to data

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    stbi_image_free(data);
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
    glActiveTexture(GL_TEXTURE0 + availableTextureSlot);
    glBindTexture(GL_TEXTURE_2D, texID);

    // setup texture filtering option and repeat model
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // fills the GPU texture with the data stored in the CPU image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return texID;
}

GLuint initTexArray()
{
    GLuint arrayTexID = 0;

    glGenTextures(1, &arrayTexID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexID);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA,        // format
        128,            // width
        128,            // height
        100,            // depth (= nb of textures)
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    return arrayTexID;
}

GLuint buildTextureArrays()
{
    // usage : 1. side (mandatory) / 2. top (optional) / 3. bottom (optional)
    currentSpotInArrayTex = 0;
    GLuint arrayTexID = initTexArray();

    Texture sandTex(Type::SOLID);
    sandTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/sand.png");
    Texture grassTex(Type::SOLID);
    grassTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/grass_block_side.png");
    grassTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/grass_block_top.png");
    Texture snowTex(Type::SOLID);
    snowTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/grass_block_snow.png");
    snowTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/powder_snow.png");
    Texture dirtTex(Type::SOLID);
    grassTex.addSample(currentSpotInArrayTex);
    snowTex.addSample(currentSpotInArrayTex);
    dirtTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/dirt.png");
    Texture gravelTex(Type::SOLID);
    gravelTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/gravel.png");
    Texture bricksTex(Type::SOLID);
    bricksTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/bricks.png");
    Texture oakplanksTex(Type::SOLID);
    oakplanksTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/oak_planks.png");
    Texture stoneTex(Type::SOLID);
    stoneTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/stone.png");
    Texture oaklogTex(Type::SOLID);
    oaklogTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/oak_log.png");
    oaklogTex.addSample(currentSpotInArrayTex);
    oaklogTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/oak_log_top.png");
    Texture waterTex(Type::TRANSPARENT);
    waterTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/water.png");
    Texture oakleavesTex(Type::TRANSPARENT);
    oakleavesTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/oak_leaves.png");
    /*Texture cobblestoneTex(Type::SOLID);
    cobblestoneTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/cobblestone.png");
    Texture cactusTex(Type::SOLID);
    cactusTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/cactus_side.png");
    cactusTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/cactus_top.png");
    cactusTex.addSample(currentSpotInArrayTex);
    addToArray(arrayTexID, "texture/block/cactus_bottom.png");*/

    textures["sand"] = sandTex;
    textures["grass"] = grassTex;
    textures["snow"] = snowTex;
    textures["dirt"] = dirtTex;
    textures["gravel"] = gravelTex;
    textures["bricks"] = bricksTex;
    textures["oakplanks"] = oakplanksTex;
    textures["stone"] = stoneTex;
    textures["oaklog"] = oaklogTex;
    textures["water"] = waterTex;
    textures["oakleaves"] = oakleavesTex;
    //textures["cobblestone"] = cobblestoneTex;
    //textures["cactus"] = cactusTex;
}

GLuint buildNormalTextureArrays()
{
    currentSpotInArrayTex = 0;
    GLuint arrayTexID = initTexArray();

    addToArray(arrayTexID, "texture/block/sand_n.png");
    addToArray(arrayTexID, "texture/block/grass_block_side_n.png");
    addToArray(arrayTexID, "texture/block/grass_block_top_n.png");
    addToArray(arrayTexID, "texture/block/grass_block_snow_n.png");
    addToArray(arrayTexID, "texture/block/powder_snow_n.png");
    addToArray(arrayTexID, "texture/block/dirt_n.png");
    addToArray(arrayTexID, "texture/block/gravel_n.png");
    addToArray(arrayTexID, "texture/block/bricks_n.png");
    addToArray(arrayTexID, "texture/block/oak_planks_n.png");
    addToArray(arrayTexID, "texture/block/stone_n.png");
    addToArray(arrayTexID, "texture/block/oak_log_n.png");
    addToArray(arrayTexID, "texture/block/oak_log_top_n.png");
    addToArray(arrayTexID, "texture/block/water_n.png");
    addToArray(arrayTexID, "texture/block/oak_leaves_n.png");

    return arrayTexID;
}

GLuint loadCubemap(std::vector<std::string> faces, bool withAlpha = false)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
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

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}

FboShadowMap allocateShadowMapFbo(unsigned int width = 2000, unsigned int height = 2000)
{
    FboShadowMap fbo;
    glGenFramebuffers(1, &fbo._depthMapFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo._depthMapFbo);

    fbo._depthMapTextureWidth = width;
    fbo._depthMapTextureHeight = height;

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &fbo._depthMapTexture);
    glActiveTexture(GL_TEXTURE0 + availableTextureSlot);
    glBindTexture(GL_TEXTURE_2D, fbo._depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    camera.setAspectRatio(windowSize);
    player.setPointerRatio(windowSize, pointerShader);
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

    else if (action == GLFW_PRESS && key == GLFW_KEY_R)
        survivalMode = !survivalMode;

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

    player.processMouseMoovement(xoffset, yoffset);
}

// Executed each time a mouse button is clicked.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        world.destroyBlock();
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        world.addBlock(player.getCurrentBlock());
}


// Executed each time a scroll is proceed.
void scrollCallback(GLFWwindow* window, double xoDffset, double yoffset)
{
    player.processMouseScroll(yoffset);
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

void initTextures()
{
    worldShader     = Shader("shader/chunkVertexShader.glsl",    "shader/chunkFragShader.glsl");
    playerShader    = Shader("shader/playerVertexShader.glsl",    "shader/playerFragShader.glsl");
    pointerShader   = Shader("shader/textVertexShader.glsl",      "shader/textFragShader.glsl");
    skyShader       = Shader("shader/skyboxVertexShader.glsl",    "shader/skyboxFragShader.glsl");
    shadowMapShader = Shader("shader/shadowMapVertexShader.glsl", "shader/shadowMapFragShader.glsl");

    //bind block texture array
    GLuint arrayTex = buildTextureArrays();
    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTex);
    worldShader.use();
    worldShader.setInt("material.textureArray", availableTextureSlot);
    playerShader.use();
    playerShader.setInt("material.textureArray", availableTextureSlot);
    availableTextureSlot++;

    //bind normal map array
    /*GLuint normalTex = buildNormalTextureArrays();
    glBindTexture(GL_TEXTURE_2D_ARRAY, normalTex);
    worldShader.use();
    worldShader.setInt("material.normalMap", availableTextureSlot);
    availableTextureSlot++;*/

    // bind font texture
    GLuint fontTexture = loadTexture("texture/other/font.bmp");
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    pointerShader.use();
    pointerShader.setInt("material.textureData", availableTextureSlot);
    availableTextureSlot++;

    // bind selection texture
    GLuint selectionTexture = loadTexture("texture/other/selection.png");
    glBindTexture(GL_TEXTURE_2D, selectionTexture);
    playerShader.use();
    playerShader.setInt("material.textureData", availableTextureSlot);
    availableTextureSlot++;

    // bind cubemap
    std::vector<std::string> faces = {
        "texture/skybox/default/right.bmp",
        "texture/skybox/default/left.bmp",
        "texture/skybox/default/top.bmp",
        "texture/skybox/default/bottom.bmp",
        "texture/skybox/default/front.bmp",
        "texture/skybox/default/back.bmp" };

    GLuint cubemap = loadCubemap(faces);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    skyShader.use();
    skyShader.setInt("cubemap", 0);
}

void initScene()
{
    //setup the light
    light = Light(
        glm::vec3(150.0, 100.0, 150.0),	   // position
        glm::vec3(1.f, 1.f, 1.f)); 	       // color

    light.setShadowMapOnGPU(availableTextureSlot);
    light.setFBO(allocateShadowMapFbo());
    availableTextureSlot++;

    // setup the world
    world = World(textures);
    world.genWorld();

    // setup the camera
    camera = Camera();
    camera.setAspectRatio(windowSize);
    camera.bindProjection(worldShader, playerShader, skyShader);

    // setup the player
    player = Player(
        glm::vec3(0.0, 50.0, 0.0),     // position
        glm::vec3(0.0, 0.0, -1.0),     // front vector
        glm::vec3(0.0, 1.0, 0.0));     // up vector

    player.setPointerRatio(windowSize, pointerShader);
    player.insertBlock(textures["dirt"], 0);
    player.insertBlock(textures["oakleaves"], 1);
    player.insertBlock(textures["gravel"], 2);
    player.insertBlock(textures["oakplanks"], 3);
    player.insertBlock(textures["bricks"], 4);
    player.insertBlock(textures["sand"], 5);
    player.insertBlock(textures["cactus"], 6);
    player.insertBlock(textures["stone"], 7);
    player.insertBlock(textures["oaklog"], 8);
    player.insertBlock(textures["grass"], 9);
}

void update()
{
    // regen the shadow map
    shadowMapShader.use();
    light.setupCameraForShadowMapping(glm::vec3(0.f), 100.f, player.getPosition());
    shadowMapShader.setMat4("depthMVP", light.getDepthMVP());
    light.bindShadowMap();
    world.simpleRender();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    light.bindLight(worldShader, playerShader);

    //update player and camera position
    player.updatePosition(window, currentFrame - lastFrame, world, survivalMode);
    camera.focusCamOnPlayer(player);
    camera.bindView(worldShader, playerShader, skyShader);

    //update world attributes
    world.updateSelection(camera.getPosition(), player.getFront());
}

void render()
{
    world.render(worldShader, skyShader, player.getPosition());
    player.render(playerShader, pointerShader, world);
}

void printFPS()
{
    nbFrames++;
    if ((double)currentFrame - lastTime >= 1.0)
    {
        printf("%f ms/frame\n", 1000.0 / double(nbFrames));
        nbFrames = 0;
        lastTime += 1.0;
    }
}

void clear()
{
    world.clearBuffers();
    player.deleteVAO();
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    initGLFW();
    initOpenGL();
    initTextures();
    initScene();

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lastFrame = currentFrame;
        currentFrame = glfwGetTime();
        update();
        render();
        printFPS();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    clear();
    return EXIT_SUCCESS;
}
