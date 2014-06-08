#include "GLHeaders.hpp"
#include "GLShaderLoader.hpp"
#include "Camera.hpp"
#include "ModelData.hpp"
#include "Shapes.hpp"
#include "Renderer.hpp"
#include "City.hpp"
#include "Sun.hpp"
#include "Skybox.hpp"
#include "BuildingFactory.hpp"
#include "Terrain.hpp"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#define TAU (6.283185307179586f)
#define DEG2RAD(x) ((x) / 360.0f * TAU)

#define SKY_DIR (glm::vec3(0, 1, 0))
#define FORWARD_DIR (glm::vec3(0, 0, 1))
#define ORIGIN  (glm::vec3(0))

#define NUMBER_OF_BUILDINGS 20

static ModelData* streetlightModel;
static City* city;
static BuildingFactory* buildingFactory;
static Terrain* ground;

static Renderer* renderer;
static Camera* cam1;
static Sun* sun;

static Skybox* skybox;

static GLsizei screenWidth = 800;
static GLsizei screenHeight = 600;

static long prevTime = 0;

// A simple structure for storing relevant information required for keyboard control
struct KeyState {
    bool up;
    bool down;
    bool left;
    bool right;
} keyState;

// A simple structure for storing relevant information required for mouse control
static struct MouseHandler {
    MouseHandler() : x(0), y(0), prevX(0), prevY(0), isDown(false) {}
    int x, y;
    int prevX, prevY;
    bool isDown;
    inline int dx() { return x - prevX; }
    inline int dy() { return y - prevY; }
    inline void update(int nx, int ny) { prevX = x; prevY = y; x = nx; y = ny; }
} mouseHandler;

// Initialise the program resources
void initResources() {
    GLuint shadowMapProgram = initProgram(shaderFromFile("shaders/shadowmap.v.glsl", GL_VERTEX_SHADER),
        shaderFromFile("shaders/shadowmap.f.glsl", GL_FRAGMENT_SHADER));
    GLuint modelProgram = initProgram(shaderFromFile("shaders/vshader.glsl", GL_VERTEX_SHADER),
        shaderFromFile("shaders/fshader.glsl", GL_FRAGMENT_SHADER));

    GLuint skyboxProgram = initProgram(shaderFromFile("shaders/skybox.v.glsl", GL_VERTEX_SHADER),
        shaderFromFile("shaders/skybox.f.glsl", GL_FRAGMENT_SHADER));

    cam1 = new Camera(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(0.0f, 10.0f, 1.0f));
    sun = new Sun(-TAU / 24.0f, TAU / 12.0f);
    renderer = new Renderer(screenWidth, screenHeight, 30.0f, cam1, sun, modelProgram, shadowMapProgram, skyboxProgram);

    ground = new Terrain(renderer);

    std::vector <std::string> sideTextureNames;
    sideTextureNames.push_back("data/building/windows.jpg");
    std::string topTextureName = "data/default.tga";
    buildingFactory = new BuildingFactory(sideTextureNames, topTextureName);

    // Generate city
    std::vector <ModelData *> modelBuildings;
    std::vector <RawModelData> buildings;
    buildings = buildingFactory->genBuildings(NUMBER_OF_BUILDINGS);
    for (int i=0; i<buildings.size(); i++) {
        ModelData *buildingModel = new ModelData(buildings[i], renderer);
        buildingModel->unify();
        modelBuildings.push_back(buildingModel);
    }
    streetlightModel = new ModelData(loadModelData("data/streetlight/lamppost_01.obj", true), renderer);
    city = new City(modelBuildings, streetlightModel, 30.0f);

    //day filenames
    std::vector<std::string> day_files;
    day_files.push_back("data/skybox/bluecloud_ft.jpg");
    day_files.push_back("data/skybox/bluecloud_bk.jpg");
    day_files.push_back("data/skybox/bluecloud_lf.jpg");
    day_files.push_back("data/skybox/bluecloud_rt.jpg");
    day_files.push_back("data/skybox/bluecloud_up.jpg");
    day_files.push_back("data/skybox/bluecloud_dn.jpg");

    //night filenames
    std::vector<std::string> night_files;
    night_files.push_back("data/skybox/starrynight_ft.jpg");
    night_files.push_back("data/skybox/starrynight_bk.jpg");
    night_files.push_back("data/skybox/starrynight_lf.jpg");
    night_files.push_back("data/skybox/starrynight_rt.jpg");
    night_files.push_back("data/skybox/starrynight_up.jpg");
    night_files.push_back("data/skybox/starrynight_dn.jpg");

    //sunset filenames
    std::vector<std::string> sunset_files;
    sunset_files.push_back("data/skybox/yellowcloud_ft.jpg");
    sunset_files.push_back("data/skybox/yellowcloud_bk.jpg");
    sunset_files.push_back("data/skybox/yellowcloud_lf.jpg");
    sunset_files.push_back("data/skybox/yellowcloud_rt.jpg");
    sunset_files.push_back("data/skybox/yellowcloud_up.jpg");
    sunset_files.push_back("data/skybox/yellowcloud_dn.jpg");

    skybox = new Skybox(renderer, day_files, night_files, sunset_files);
    renderer->attachSkybox(skybox);

    keyState.up = false;
    keyState.down = false;
    keyState.left = false;
    keyState.right = false;
}

// Display callback
void onDisplay() {
    renderer->clear();

    ground->draw(renderer, city, cam1->getPosition(), 15);
    city->draw(renderer, cam1->getPosition());
    renderer->renderScene();

    // Swap buffers
    glutSwapBuffers();
}

// FPS counter vars
long past = 0;
int frames = 0;

// Idle callback
void onIdle() {
    long time = glutGet(GLUT_ELAPSED_TIME);
    double dt = static_cast<double>(time - prevTime) / 1000.0;
    prevTime = time;

    sun->update(dt);

    // FPS counter
    frames += 1;
    if (static_cast<double>(time - past) / 1000.0 >= 1.0) {
        std::cout << "FPS: " << frames << std::endl;
        frames = 0;
        past = time;
    }

    glm::vec3 relativeMovement = glm::vec3(0.0f, 0.0f, 0.0f);

    if (keyState.up && !keyState.down) {
        relativeMovement.z += 0.2f;
    } 
    if (keyState.down && !keyState.up) {
        relativeMovement.z -= 0.2f;
    }
    if (keyState.left && !keyState.right) {
        relativeMovement.x -= 0.2f;
    }
    if (keyState.right && !keyState.left) {
        relativeMovement.x += 0.2f;
    }

    glm::vec3 absoluteMovement = cam1->inspectMovement(relativeMovement);
    // Break the movement down into seperate components
    glm::vec3 movementX = glm::vec3(absoluteMovement.x, 0, 0);
    glm::vec3 movementY = glm::vec3(0, absoluteMovement.y, 0);
    glm::vec3 movementZ = glm::vec3(0, 0, absoluteMovement.z);

    // Move each of the components if they don't collide with something
    // Not the most efficient but very simple.

    if (!(renderer->checkCollision(cam1->getPosition() + movementX))) {
        cam1->moveAbsolute(movementX);
    }
    if (!(renderer->checkCollision(cam1->getPosition() + movementY))) {
        cam1->moveAbsolute(movementY);
    }
    if (!(renderer->checkCollision(cam1->getPosition() + movementZ))) {
        cam1->moveAbsolute(movementZ);
    }

    glutPostRedisplay();
}

// Keyboard callback
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 033: exit(EXIT_SUCCESS); break;
    case 'w': keyState.up = true; break;
    case 's': keyState.down = true; break;
    case 'a': keyState.left = true; break;
    case 'd': keyState.right = true; break;
    case 'i': sun->increaseSpeed(); break;
    case 'o': sun->decreaseSpeed(); break;
    case 'p': sun->togglePause(); break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': keyState.up = false; break;
    case 's': keyState.down = false; break;
    case 'a': keyState.left = false; break;
    case 'd': keyState.right = false; break;
    }
}

// Mouse click callback
void onMouse(int button, int state, int x, int y) {
    mouseHandler.update(x, y);
}

// Mouse motion callback
void onMotion(int x, int y) {
    mouseHandler.update(x, y);
    // Compute the relative mouse movement
    const glm::vec3 movement = glm::vec3(TAU * static_cast<float>(mouseHandler.dx()) / screenWidth,
        TAU * static_cast<float>(-mouseHandler.dy()) / screenHeight, 0.0f);
    cam1->moveTarget(movement);

    glutPostRedisplay();
}

// Window shape callbackwww
void onReshape(GLsizei width, GLsizei height) {
    renderer->resize(width, height);
}


void setTimeMenu(int id) {
    switch(id) {
        case 1: sun->setVerticalAngle(3.14159); break;
        case 2: sun->setVerticalAngle(0); break;
        case 3: sun->setVerticalAngle(0.27); break;
        case 4: sun->setVerticalAngle(2.89); break;
    }
}

void setSpeedMenu(int id) {
    switch(id) {
        case 1: sun->increaseSpeed(); break;
        case 2: sun->decreaseSpeed(); break;
        case 3: sun->togglePause(); break;
        case 4: sun->resetSpeed(); break;
    }

}

void mainMenu(int id) {

}

void initGlutMenu() {

    int settime_menu = glutCreateMenu(setTimeMenu);
    glutAddMenuEntry("Night", 1);
    glutAddMenuEntry("Sunrise", 2);
    glutAddMenuEntry("Day", 3);
    glutAddMenuEntry("Sunset", 4);

    int setspeed_menu = glutCreateMenu(setSpeedMenu);
    glutAddMenuEntry("Increase Speed (i)", 1);
    glutAddMenuEntry("Decrease Speed (o)", 2);
    glutAddMenuEntry("(Un)Pause Speed (p)", 3);
    glutAddMenuEntry("Default Speed", 4);


    glutCreateMenu(mainMenu);
    glutAddSubMenu("Set Time of Day", settime_menu);
    glutAddSubMenu("Set Speed of Sun", setspeed_menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


// Program entry point
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);

#ifndef __APPLE__
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    
    glutInitWindowSize(800, 600);
    glutCreateWindow("Assignment 4");

#ifndef __APPLE__
    glewInit();
#endif

    // Enable GL properties
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    // Set up callbacks
    glutDisplayFunc(onDisplay);
    glutIdleFunc(onIdle);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    initGlutMenu();
    
    initResources();

    glutMainLoop();
    return 0;
}
