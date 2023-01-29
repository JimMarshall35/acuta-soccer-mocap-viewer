#include "Camera.h"
#include <iostream>
#include <fstream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::LookCameraAt(const glm::vec3& point, const glm::vec3& right )
{
    Front = glm::normalize(point - Position);
    //Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    //Up = glm::normalize(glm::cross(Right, Front));

    //Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    //Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::Report()
{
    std::cout << "_____________Camera report_____________\n";
    std::cout << "camera.WorldUp = glm::vec3{" << WorldUp.x << ", " << WorldUp.y << ", " << WorldUp.z << "};\n";
    std::cout << "camera.Position = glm::vec3{" << Position.x << ", " << Position.y << ", " << Position.z << "};\n";
    std::cout << "camera.Front = glm::vec3{" << Front.x << ", " << Front.y << ", " << Front.z << "};\n";
    std::cout << "camera.Up = glm::vec3{" << Up.x << ", " << Up.y << ", " << Up.z << "};\n";
    std::cout << "camera.Right = glm::vec3{" << Right.x << ", " << Right.y << ", " << Right.z << "};\n";
    std::cout << "camera.Yaw = " << Yaw << ";\n";
    std::cout << "camera.Pitch = " << Pitch << ";\n";
    std::cout << "camera.Zoom = " << Zoom << ";\n";
    std::cout << "camera.MovementSpeed = " << MovementSpeed << ";\n";
    std::cout << "camera.MouseSensitivity = " << MouseSensitivity << ";\n";
    std::cout << "\n";

}
