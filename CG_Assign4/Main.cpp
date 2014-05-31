#include "GLHeaders.hpp"
#include "GLShaderLoader.hpp"
#include "Camera.hpp"
#include "ModelData.hpp"
#include "Shapes.hpp"
#include "Renderer.hpp"
#include "City.hpp"
#include "Sun.hpp"
#include "Skybox.hpp"

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

static ModelData* terrainModel;
static ModelData* buildingModel;
static City* city;

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

// Generates a square terrain with the specified texture loaded from a file
RawModelData genTerrainModel(const std::string& terrainTexture) {
    RawModelData data;
    RawModelData::Shape shape = shapes::quad(glm::vec3(-1, 0, 1), glm::vec3(-1, 0, -1),
        glm::vec3(1, 0, -1), glm::vec3(1, 0, 1));
    shape.textureName = terrainTexture;
    data.shapes.push_back(shape);

    // BoundingBox of terrain
    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-1, 0, -1);
    boundingBox.maxVertex = glm::vec3(1, 0, 1);
    data.boundingBox = boundingBox;

    return data;
}

// FIXME: This should be replaced by a better function for generating buildings
//  - Need to generate normals correctly
//  - Need to generate textures for day and night
//  - Need to generate bump map / heightmap
RawModelData genCube(const std::string& texture) {
    RawModelData data;
    RawModelData::Shape shape;

    // Top
    shape = shapes::quad(glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1),
        glm::vec3(1, 1, -1), glm::vec3(1, 1, 1));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Bottom
    shape = shapes::quad(glm::vec3(1, -1, 1), glm::vec3(1, -1, -1),
        glm::vec3(-1, -1, -1), glm::vec3(-1, -1, 1));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Left
    shape = shapes::quad(glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1),
        glm::vec3(-1, -1, -1), glm::vec3(-1, 1, -1));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Right
    shape = shapes::quad(glm::vec3(1, 1, -1), glm::vec3(1, -1, -1),
        glm::vec3(1, -1, 1), glm::vec3(1, 1, 1));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Front
    shape = shapes::quad(glm::vec3(-1, 1, -1), glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1), glm::vec3(1, 1, -1));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Back
    shape = shapes::quad(glm::vec3(1, 1, 1), glm::vec3(1, -1, 1),
        glm::vec3(-1, -1, 1), glm::vec3(-1, 1, 1));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // BoundingBox of whole cube
    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-1, -1, -1);
    boundingBox.maxVertex = glm::vec3(1, 1, 1);
    data.boundingBox = boundingBox;

    return data;
}

void drawProceduralTerrain(const int terrainSize) {

    // Find camera position square
    glm::vec3 cameraPosition = glm::vec3(cam1->getPosition().x, 0.0, cam1->getPosition().z);
    glm::vec3 centerSquare = glm::vec3( 
        terrainSize * 2 * (int)((cameraPosition.x + terrainSize * cameraPosition.x/(fabs(cameraPosition.x)))/(terrainSize * 2)),
        0.0, 
        terrainSize * 2 * (int)((cameraPosition.z + terrainSize * cameraPosition.z/(fabs(cameraPosition.z)))/(terrainSize * 2))
        );

    // Draw center square and 8 surrounding squares
    for (int i=-1; i<2; i++) {
        for (int j=-1; j<2; j++) {
            glm::vec3 square = centerSquare;
            square.x = centerSquare.x + terrainSize * 2 * j;
            square.z = centerSquare.z + terrainSize * 2 * i;
            renderer->drawModel(terrainModel, ORIGIN + square, glm::vec3(terrainSize, 1, terrainSize));
        }
    }
}

// Initialise the program resources
void initResources() {
    GLuint shadowMapProgram = initProgram(shaderFromFile("shaders/shadowmap.v.glsl", GL_VERTEX_SHADER),
        shaderFromFile("shaders/shadowmap.f.glsl", GL_FRAGMENT_SHADER));
    GLuint modelProgram = initProgram(shaderFromFile("shaders/vshader.glsl", GL_VERTEX_SHADER),
        shaderFromFile("shaders/fshader.glsl", GL_FRAGMENT_SHADER));

    GLuint skyboxProgram = initProgram(shaderFromFile("shaders/skybox.v.glsl", GL_VERTEX_SHADER),
        shaderFromFile("shaders/skybox.f.glsl", GL_FRAGMENT_SHADER));

    cam1 = new Camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 10.0f, 1.0f));
    sun = new Sun(-TAU / 24.0f, TAU / 12.0f);
    renderer = new Renderer(screenWidth, screenHeight, 30.0f, cam1, sun, modelProgram, shadowMapProgram, skyboxProgram);

    buildingModel = new ModelData(genCube("data/default.tga"), renderer);
    terrainModel = new ModelData(genTerrainModel("data/default.tga"), renderer);
    city = new City(buildingModel, 30.0f);
    
    //night filenames
    std::vector<std::string> night_files;
    night_files.push_back("data/skybox/graycloud_ft.jpg");
    night_files.push_back("data/skybox/graycloud_bk.jpg");
    night_files.push_back("data/skybox/graycloud_lf.jpg");
    night_files.push_back("data/skybox/graycloud_rt.jpg");
    night_files.push_back("data/skybox/graycloud_up.jpg");
    night_files.push_back("data/skybox/graycloud_dn.jpg");

    skybox = new Skybox(renderer, 
        "data/skybox/bluecloud_ft.jpg", "data/skybox/bluecloud_bk.jpg", "data/skybox/bluecloud_lf.jpg", 
        "data/skybox/bluecloud_rt.jpg", "data/skybox/bluecloud_up.jpg", "data/skybox/bluecloud_dn.jpg",
        night_files);

    renderer->attachSkybox(skybox);

    keyState.up = false;
    keyState.down = false;
    keyState.left = false;
    keyState.right = false;
}

// Display callback
void onDisplay() {
    renderer->clear();
    city->draw(renderer, cam1->getPosition());

    const int terrainSize = 20;
    drawProceduralTerrain(terrainSize);
    
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


    bool collision = renderer->checkCollision(cam1->getPosition());

    // Key control
    if (keyState.up) {
        if (!collision) {
            cam1->move(glm::vec3(0, 0, 0.2f));
        } else {
            cam1->move(glm::vec3(0, 0, -0.1f));
        }
    }
    if (keyState.down) {
        if (!collision) {
            cam1->move(glm::vec3(0, 0, -0.2f));
        } else {
            cam1->move(glm::vec3(0, 0, 0.1f));
        }
    }
    if (keyState.left) {
        if (!collision) {
            cam1->move(glm::vec3(-0.2f, 0, 0));
        } else {
            cam1->move(glm::vec3(0.1f, 0, 0));
        }
    }
    if (keyState.right) {
        if (!collision) {
            cam1->move(glm::vec3(0.2f, 0, 0));
        } else {
            cam1->move(glm::vec3(-0.1f, 0, 0));
        }
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

// Program entry point
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);

#ifndef __APPLE__
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
#else
    glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
#endif
    
    glutInitWindowSize(800, 600);
    glutCreateWindow("Assignment 4");

#ifndef __APPLE__
    glewInit();
#endif

    // Enable GL properties
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
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

    initResources();

    glutMainLoop();
    return 0;
}
