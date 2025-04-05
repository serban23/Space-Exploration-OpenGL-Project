#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/matrix_inverse.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include "Model3D.hpp"

#include <iostream>

// window
gps::Window window;

// matrices
glm::mat4 model;
glm::mat4 originlModelHand = model;
glm::mat4 originlModelPanou = model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

//point light
int activatePointLight = 0;

//spot light
float spotlight1;
float spotlight2;
glm::vec3 spotLightDirection;
glm::vec3 spotLightPosition;
int activateSpotLight = 1;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

//Shader
gps::Shader basicShader;

//models
gps::Model3D terrain;
gps::Model3D astronaut1;
gps::Model3D astronaut2;
gps::Model3D rocket;
gps::Model3D rover;
gps::Model3D roverDetail;
gps::Model3D dome;
gps::Model3D corridor;
gps::Model3D windows;
gps::Model3D solarPanel;
gps::Model3D structure;
gps::Model3D ufo;

//Presentation
bool startPresentation = false;

// camera
gps::Camera myCamera(glm::vec3(10.0f, 2.0f, -10.0f), glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

//mouse
bool firstMouse = true;
float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw = +180.0f;

GLfloat cameraSpeed = 0.3f;
GLboolean pressedKeys[1024];

//SkyBox
gps::SkyBox mySkyBox;
gps::Shader skyBoxShader;

//Fog
int activateFog = 0;

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

//callbacks
void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        basicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        basicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        view = myCamera.getViewMatrix();
        basicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        view = myCamera.getViewMatrix();
        basicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        activateFog = !activateFog;
        basicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(basicShader.shaderProgram, "activateFog"), activateFog);
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        activatePointLight == 0 ? activatePointLight = 1 : activatePointLight = 0;
        basicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(basicShader.shaderProgram, "activatePointLight"), activatePointLight);
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        activateSpotLight == 0 ? activateSpotLight = 1 : activateSpotLight = 0;
        basicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(basicShader.shaderProgram, "activateSpotLight"), activateSpotLight);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }

    if (pressedKeys[GLFW_KEY_J])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_K])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_L])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        startPresentation = !startPresentation; 
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
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

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
    basicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(window.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(window.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(window.getWindow(), mouseCallback);
    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

//inits
void initWindow() {
    window.Create(1920, 1000, "OpenGL Project Core");
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, window.getWindowDimensions().width, window.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS); 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); 
    glFrontFace(GL_CCW); 
}

void initModels() {

    terrain.LoadModel("resurse/terr/terrain.obj");
    astronaut1.LoadModel("resurse/astro/astronaut1.obj");
    astronaut2.LoadModel("resurse/astro/astronaut2.obj");
    rocket.LoadModel("resurse/rock/rocket.obj");
    rover.LoadModel("resurse/rov/rover.obj");
    roverDetail.LoadModel("resurse/rovdet/roverDetails.obj");
    dome.LoadModel("resurse/dom/dome.obj");
    corridor.LoadModel("resurse/corr/corridor.obj");
    windows.LoadModel("resurse/win/windows.obj");
    solarPanel.LoadModel("resurse/sol/solarPanels.obj");
    structure.LoadModel("resurse/struc/structure.obj");
    ufo.LoadModel("resurse/uf/UFO.obj");
}

void initShaders() {
    basicShader.loadShader("shaders/basic.vert","shaders/basic.frag");
    skyBoxShader.loadShader("shaders/skyBox.vert","shaders/skyBox.frag");
}

void initUniforms() {
    basicShader.useShaderProgram();

    // create model
    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(basicShader.shaderProgram, "model");

    // get view 
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(basicShader.shaderProgram, "view");

    // send view 
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(basicShader.shaderProgram, "normalMatrix");

    // send model & normal
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // create projection 
    projection = glm::perspective(glm::radians(45.0f),
        (float)window.getWindowDimensions().width / (float)window.getWindowDimensions().height,
        0.1f, 2000.0f);
    projectionLoc = glGetUniformLocation(basicShader.shaderProgram, "projection");

    // send projection
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction 
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(basicShader.shaderProgram, "lightDir");

    // send light direction
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(basicShader.shaderProgram, "lightColor");

    // send light color
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // spotlight
    spotlight1 = glm::cos(glm::radians(40.5f));
    spotlight2 = glm::cos(glm::radians(100.5f));

    spotLightDirection = glm::vec3(0, -1, 0);
    spotLightPosition = glm::vec3(42.266f, 10.0f, 12.0f);

    glUniform1f(glGetUniformLocation(basicShader.shaderProgram, "material.shininess"), 100.0f);

    //directional
    glUniform3fv(glGetUniformLocation(basicShader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(lightDir));
    glUniform3f(glGetUniformLocation(basicShader.shaderProgram, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(basicShader.shaderProgram, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(basicShader.shaderProgram, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

    //point
    glUniform1f(glGetUniformLocation(basicShader.shaderProgram, "pointLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(basicShader.shaderProgram, "pointLight.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(basicShader.shaderProgram, "pointLight.quadratic"), 0.032f);
    glUniform3f(glGetUniformLocation(basicShader.shaderProgram, "pointLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(basicShader.shaderProgram, "pointLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(basicShader.shaderProgram, "pointLight.specular"), 1.0f, 1.0f, 1.0f);

    //spot
    glUniform1f(glGetUniformLocation(basicShader.shaderProgram, "spotlight1"), spotlight1);
    glUniform1f(glGetUniformLocation(basicShader.shaderProgram, "spotlight2"), spotlight2);
    glUniform3fv(glGetUniformLocation(basicShader.shaderProgram, "spotLightDirection"), 1, glm::value_ptr(spotLightDirection));
    glUniform3fv(glGetUniformLocation(basicShader.shaderProgram, "spotLightPosition"), 1, glm::value_ptr(spotLightPosition));

    glUniform1i(glGetUniformLocation(basicShader.shaderProgram, "activateSpotLight"), activateSpotLight);
    glUniform1i(glGetUniformLocation(basicShader.shaderProgram, "activatePointLight"), activatePointLight);
}

void initSkybox() {
    std::vector<const GLchar*> faces;
    faces.push_back("resurse/left.JPG");
    faces.push_back("resurse/right.JPG");
    faces.push_back("resurse/up.JPG");
    faces.push_back("resurse/down.JPG");
    faces.push_back("resurse/back.JPG");
    faces.push_back("resurse/front.JPG");
    mySkyBox.Load(faces);
}

//render
void renderScene(gps::Shader shader){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.useShaderProgram();

    terrain.Draw(shader);
    astronaut1.Draw(shader);
    astronaut2.Draw(shader);
    rocket.Draw(shader);
    rover.Draw(shader);
    roverDetail.Draw(shader);
    dome.Draw(shader);
    corridor.Draw(shader);
    windows.Draw(shader);
    solarPanel.Draw(shader);
    structure.Draw(shader);
    ufo.Draw(shader);

    mySkyBox.Draw(skyBoxShader, view, projection);
}

//presentation
void presentation(float time) {
    // camera will move between these 5 points
    std::vector<glm::vec3> cameraPositions = {
        glm::vec3(10.0f, 5.0f, 10.0f),
        glm::vec3(-10.0f, 5.0f, 10.0f),
        glm::vec3(-10.0f, 5.0f, -10.0f),
        glm::vec3(10.0f, 5.0f, -10.0f),
        glm::vec3(0.0f, 10.0f, 0.0f)
    };

    int numPoints = cameraPositions.size();
    float segmentTime = 5.0f;  // time for transition between points
    int currentSegment = static_cast<int>(time / segmentTime) % numPoints;
    int nextSegment = (currentSegment + 1) % numPoints;

    // progress between points
    float t = fmod(time, segmentTime) / segmentTime;

    // interpolation between points for movement
    glm::vec3 newPosition = glm::mix(cameraPositions[currentSegment], cameraPositions[nextSegment], t);

    myCamera.setPosition(newPosition);
    myCamera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f)); 

    view = myCamera.getViewMatrix();
    basicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

//cleanup
void cleanup() {
    window.Delete();
}

//main
int main(int argc, const char* argv[]) {

    try {
        initWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initSkybox();
    initUniforms();
    setWindowCallbacks();

    glCheckError();

    while (!glfwWindowShouldClose(window.getWindow())) {

        renderScene(basicShader);

        if (startPresentation) {
            presentation(glfwGetTime());
        }

        glfwPollEvents();
        glfwSwapBuffers(window.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
