/**********************************************************************************
*
* Actua soccer motion capture file tool
* Jim Marshall - 2022
*
***********************************************************************************/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include <iostream>
#include <functional>
#include <thread>
#include <memory>
#include "MocapFile.h"
#include "Renderer.h"
#include "ToolUi.h"
#include "WindowsFilesystem.h"
#include "MocapAnimation.h"
#include "Config.h"
#include "Euro.h"

#define SCR_WIDTH 1200
#define SCR_HEIGHT 800

#define TARGET_MS_PER_FRAME 30

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

Camera camera = Camera();
GLFWwindow* window;
Renderer* gRenderer;

double deltaTime = 0;
bool uiWantsMouse = false;
bool uiWantsKeyboard = false;

static void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

void CloseWindow() {
    glfwSetWindowShouldClose(window, true);
}



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Actua Soccer Mocap Tool", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    using namespace std;
    using namespace std::chrono;

    auto prevClock = high_resolution_clock::now();
    SkeletonConnectivity connectivity = {
        //{0,{1,2,3,4,5,8,13,17,23,24,25,26,27,28}},

        {25,{6}}, // shoulder to right elbow
        {6,{7}},  // right elbow to right hand

        {24,{9}}, // shoulder to left elbow
        {9,{10}}, // left elbow to left hand

        {13,{14}}, // right pelvis to right knee
        {14, {21}}, // right knee to right foot
        {21, {16,15,11}}, // right foot

        {17,{18}}, // left pelvis to left knee
        {18,{22}}, // left knee to left foot
        {22,{20,19,12}}, // left foot
    };

    Config config;
    MocapFile file(config.ReverseFileEndianness);
    //file.LoadFromWindowsDatFile(config.MocapFilesFolder + "\\EURO.DAT", config.MocapFilesFolder + "\\EURO.OFF", BIN_MAIN);
    file.Load(config.MocapFilesFolder + "\\485 MPB_JUGGLE.comap");
    Renderer renderer({ SCR_WIDTH, SCR_HEIGHT, config.Font });
    MocapAnimation animation(&file, connectivity);
    WindowsFilesystem windowsFileSystem;
    ToolUi ui(window, (IFilesystem*)&windowsFileSystem, &animation, &file, config);

    camera.WorldUp = glm::vec3{ 0, 1, 0 };
    camera.Position = glm::vec3{ -5.12247, 21.4454, 57.0002 };
    camera.Front = glm::vec3{ 0.0967241, -0.132257, -0.986485 };
    camera.Up = glm::vec3{ 0.0129058, 0.991216, -0.131625 };
    camera.Right = glm::vec3{ 0.995228, -0, 0.0975813 };
    camera.Yaw = -84.4001;
    camera.Pitch = -7.60001;
    camera.Zoom = 45;
    camera.MovementSpeed = 10;
    camera.MouseSensitivity = 0.4;
    renderer.SetLightColour({ 1.0,1.0,1.0 });
    renderer.SetLightPos({ 0,100,0 });

    gRenderer = &renderer;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        auto nextClock = high_resolution_clock::now();
        deltaTime = (nextClock - prevClock).count() / 1e9;
        //printf(" frame time: %.2lf ms\n", deltaTime * 1e3);


        // input
        // -----
        uiWantsKeyboard = ui.WantsKeyboard();
        uiWantsMouse = ui.WantsMouse();
        processInput(window);

        // update
        // ------
        //animation.Update(deltaTime);
        ui.Update(deltaTime);

        // render
        // ------
        glClearColor(1.0, 1.0, 1.0, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawMocapFrame(animation.GetCurrentFrame(), camera);
        ui.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        auto frameClock = high_resolution_clock::now();
        double sleepSecs = 1.0 / 60 - (frameClock - nextClock).count() / 1e9;
        if (sleepSecs > 0) {
            //timeBeginPeriod(1);
            this_thread::sleep_for(nanoseconds((int64_t)(sleepSecs * 1e9)));
            //timeEndPeriod(1);
        }

        prevClock = nextClock;

    }

    glfwTerminate();
    return 0;
}

bool escapeAcknowledged = false;
bool reportAcknowledged = false;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (uiWantsKeyboard) {
        return;
    }

    static glm::vec3 block1Pos(0.0, 1.0, 0.0);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escapeAcknowledged) {

    }
    else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {

    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !reportAcknowledged) {
        reportAcknowledged = true;
        camera.Report();
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
        reportAcknowledged = false;
    }
    //glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        std::cout << "Saving " << std::endl;
    }

    //camera.SaveToFile("Camera.cam");
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    gRenderer->SetScreenDims({ width, height });
}

double lastX = SCR_WIDTH / 2.0f;

double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cameraShouldRecieveInput = false;
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (uiWantsMouse) {
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    if (cameraShouldRecieveInput) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    

    lastX = xpos;
    lastY = ypos;

    //camera.ProcessMouseMovement(xoffset, yoffset);
    //std::cout << "yaw: " << camera.Yaw << " pitch: " << camera.Pitch << std::endl;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (uiWantsMouse) {
        return;
    }
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (uiWantsMouse) {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cameraShouldRecieveInput = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        cameraShouldRecieveInput = false;
    }
}