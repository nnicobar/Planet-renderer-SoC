#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum Cam_mov
{
FORWARD,BACKWARD,LEFT,RIGHT,UP,DOWN
};

class Camera
{
public:
    glm::vec3 cameraPos;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 lookAt;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float curTime = glfwGetTime();
    float deltaTime = 0.0f;
    float fov = 45.0f;
    float globalX;
    float globalY;
    float speed;
    void modify_pitch_yaw(float delta_x, float delta_y);
    void modify_fov(double y_offset);
    void updateTimeCounter(float decay);
    void updateLookAt();
    glm::mat4 getViewMatrix();
    void KeyboardInput(Cam_mov direction, float deltaTime);

    //Constructor
    Camera(glm::vec3 initialPosition, glm::vec3 upperDirection, float width=800, float height=600)
        {
        cameraPos = initialPosition;
        up = upperDirection;

        right = glm::normalize(glm::cross(up, cameraPos));
        globalX = width/2; globalY = height/2;
        }

};

void Camera::modify_pitch_yaw(float delta_x, float delta_y)
    {
    float senstivity = 0.05f;
    pitch += senstivity*delta_y;
    yaw += senstivity*delta_x;
    }

void Camera::modify_fov(double y_offset)
    {
            if (fov > 1.0f && fov <= 45.0f)
                fov -= y_offset;
            else if (fov<=1.0f)
                fov= 1.0f;
            else if (fov > 45.0f)
                fov = 45.0f;
    }

void Camera::updateTimeCounter(float decay = 1.0f)
    {
    deltaTime = glfwGetTime() - curTime;
    curTime = glfwGetTime();
    speed = 450.0f*deltaTime*decay;
    }
    
void Camera::updateLookAt()
    {
    lookAt.y = sin(glm::radians(pitch));
    lookAt.x = cos(glm::radians(pitch))*cos(glm::radians(yaw));
    lookAt.z = cos(glm::radians(pitch))*sin(glm::radians(yaw));
    right = glm::normalize(glm::cross(lookAt, up));
    }

glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPos, cameraPos+lookAt, up);
    }

void Camera::KeyboardInput(Cam_mov direction, float deltaTime)
    {
    float velocity = speed * deltaTime;
    if(direction == FORWARD)
    cameraPos += lookAt*velocity;
    else if(direction == BACKWARD)
    cameraPos -= lookAt * velocity;
    else if(direction == RIGHT)
    cameraPos += right*velocity;
    else if(direction == LEFT)
    cameraPos -= right*velocity;
    }
    
    


