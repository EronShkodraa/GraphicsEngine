#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <windows.h>
#include <cmath>
#include <cstdlib>
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(100.0f, 20.0f, 100.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float soundTimer = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void playMovementSound(float speed) {
    // TempleOS style - low, raw blips
    int freq = 100 + (int)(speed * 10.0f);
    Beep(freq, 15);  // Very short
}

void playTurnSound(float turnSpeed) {
    if (abs(turnSpeed) > 1.0f) {
        // Random pitch jumps like old PC speaker
        int freq = 150 + (rand() % 100);
        Beep(freq, 10);  // Even shorter
    }
}

void processInput(GLFWwindow* window, Terrain& terrain) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    
    float terrainHeight = terrain.getTerrainHeight(camera.Position.x, camera.Position.z);
    if (camera.Position.y < terrainHeight + 2.0f)
        camera.Position.y = terrainHeight + 2.0f;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mountain Terrain Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
    Terrain terrain(200, 200, 1.0f);
    Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
    
    std::cout << "Terrain Engine Started!" << std::endl;
    std::cout << "Controls: WASD to move, Mouse to look, ESC to exit" << std::endl;
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glm::vec3 oldPos = camera.Position;
        float oldYaw = camera.Yaw;
        
        processInput(window, terrain);
        
        float moveSpeed = glm::length(camera.Position - oldPos);
        float turnSpeed = camera.Yaw - oldYaw;
        
        soundTimer += deltaTime;
        
        if (moveSpeed > 0.1f && soundTimer > 0.1f) {
            playMovementSound(moveSpeed);
            soundTimer = 0.0f;
        }
        
        if (abs(turnSpeed) > 0.5f && soundTimer > 0.05f) {
            playTurnSound(turnSpeed);
            soundTimer = 0.0f;
        }
        
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                                0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        
        terrainShader.use();
        terrainShader.setMat4("projection", projection);
        terrainShader.setMat4("view", view);
        terrainShader.setMat4("model", model);
        terrainShader.setVec3("lightDir", glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f)));
        terrainShader.setVec3("viewPos", camera.Position);
        
        terrain.Draw(terrainShader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}