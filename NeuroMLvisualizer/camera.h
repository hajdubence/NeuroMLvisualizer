#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:

    Camera();

    glm::vec3 position; // center of orbit
    glm::vec3 cameraPosition; // camera
    float horizontalAngle;
    float verticalAngle;
    float radius;
    float maxRadius;
    float minRadius;

    int windowWidth;
    int windowHeight;
    float verticalFOV;
    float nearZ;
    float farZ;

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    float zoomSpeed;
    float turnSpeed;
    void OnRender(GLFWwindow* window);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static Camera* activeCamera;
    bool isClick;
    int clickXpos;
    int clickYpos;
    
private:

    glm::vec3 GetTargetVector();
    glm::vec3 GetForwardVector();
    glm::vec3 GetRightVector();
    glm::vec3 GetUpVector();

};

#endif