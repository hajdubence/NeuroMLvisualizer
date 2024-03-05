#include <stdio.h>

#include <GLFW/glfw3.h>

#include "camera.h"

using namespace glm;

Camera* Camera::activeCamera;

Camera::Camera()
{
    position = vec3(0.0f, 0.0f, 0.0f);
    horizontalAngle = 0.0f;
    verticalAngle = 0.0f;
    radius = 1000.0f;
    maxRadius = 2000.0f;
    minRadius = 10.0f;
    cameraPosition = vec3(1000.0f, 0.0f, 0.0f);

    windowWidth = 16;
    windowHeight = 9;
    verticalFOV = 45.0f;
    nearZ = 1.0f;
    farZ = 10000.0f;

    zoomSpeed = 1000.0f;
    turnSpeed = 0.001f;
    isClick = false;
    clickXpos = 0;
    clickYpos = 0;
}



void Camera::OnRender(GLFWwindow* window)
{
    activeCamera = this;
    glfwSetScrollCallback(window, scroll_callback);

    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    static double lastXpos = xpos;
    static double lastYpos = ypos;
    double deltaX = xpos - lastXpos;
    double deltaY = ypos - lastYpos;
    lastXpos = xpos;
    lastYpos = ypos;

    isClick = false;
    static bool isClicking = false;
    static bool isMoved = false;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        if (!isClicking)
        {
            clickXpos = xpos;
            clickYpos = ypos;
            isClicking = true;
        }
        if (abs(xpos - clickXpos) > 2 || abs(ypos - clickYpos) > 2)
        {
            isMoved = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        horizontalAngle -= turnSpeed * (float)deltaX;
        verticalAngle -= turnSpeed * (float)deltaY;
        verticalAngle = clamp(verticalAngle, -half_pi<float>(), half_pi<float>());
    }
    if (state == GLFW_RELEASE)
    {
        if (isClicking)
        {
            if (isMoved)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else
                isClick = true;
            isClicking = false;
        }
        isMoved = false;
    }

    cameraPosition = position - (GetTargetVector() * radius);

    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP)) {
        radius = clamp(radius - deltaTime * zoomSpeed, minRadius, maxRadius);
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN)) {
        radius = clamp(radius + deltaTime * zoomSpeed, minRadius, maxRadius);
    }

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    activeCamera->radius = clamp(activeCamera->radius - (float)yoffset * 50.0f, activeCamera->minRadius, activeCamera->maxRadius);
}



vec3 Camera::GetTargetVector()
{
    return vec3(
        cos(verticalAngle) * sin(horizontalAngle), 
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
}

vec3 Camera::GetForwardVector()
{
    return vec3(
        sin(horizontalAngle), 
        0,
        cos(horizontalAngle)
    );
}

vec3 Camera::GetRightVector()
{
    return vec3(
        sin(horizontalAngle - half_pi<float>()), 
        0,
        cos(horizontalAngle - half_pi<float>())
    );
}

vec3 Camera::GetUpVector()
{
    return cross(GetRightVector(), GetTargetVector());
}

mat4 Camera::GetViewMatrix()
{
    return lookAt(cameraPosition, cameraPosition +GetTargetVector(), GetUpVector());
}

mat4 Camera::GetProjectionMatrix()
{
    windowWidth = windowWidth == 0 ? 1 : windowWidth;
    windowHeight = windowHeight == 0 ? 1 : windowHeight;

    return perspective(
        radians(verticalFOV),
        (float)windowWidth / (float)windowHeight,
        nearZ, farZ);
}