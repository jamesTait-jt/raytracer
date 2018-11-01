#include "Camera.h"

Camera::Camera(vec4 position) {
    setPosition(position);
    setYaw(0);
    setR(mat4(1.0));
}

void Camera::rotateLeft(float yaw) {
    setYaw(getYaw() - yaw);
    mat4 newR = getR();
    newR[0] = vec4(cos(-yaw), 0, sin(-yaw), 0);
    newR[2] = vec4(-sin(-yaw), 0, cos(-yaw), 0);
    setR(newR);
    setPosition(getR() * getPosition());
}

void Camera::rotateRight(float yaw) {
    setYaw(getYaw() + yaw);
    mat4 newR = getR();
    newR[0] = vec4(cos(yaw), 0, sin(yaw), 0);
    newR[2] = vec4(-sin(yaw), 0, cos(yaw), 0);
    setR(newR);
    setPosition(getR() * getPosition());
}   

void Camera::moveForwards(float distance) {
    vec4 position = getPosition();
    vec3 newCameraPos(
        position[0] - distance * sin(this->yaw),
        position[1],
        position[2] + distance * cos(this->yaw)
        );
    mat4 camToWorld = lookAt(newCameraPos, vec3(0, 0, 0));
    setPosition(camToWorld * vec4(0, 0, 0, 1));
}

void Camera::moveBackwards(float distance) {
    vec4 position = getPosition();
    vec3 newCameraPos(
        position[0] + distance * sin(this->yaw),
        position[1],
        position[2] - distance * cos(this->yaw)
        );
    mat4 camToWorld = lookAt(newCameraPos, vec3(0, 0, 0));
    setPosition(camToWorld * vec4(0, 0, 0, 1));
}

mat4 Camera::lookAt(vec3 from, vec3 to) {
    vec3 forward = normalize(from - to);
    vec3 temp(0, 1, 0);
    vec3 right = cross(normalize(temp), forward);
    vec3 up = cross(forward, right);

    vec4 forward4(forward.x, forward.y, forward.z, 0);
    vec4 right4(right.x, right.y, right.z, 0);
    vec4 up4(up.x, up.y, up.z, 0);
    vec4 from4(from.x, from.y, from.z, 1);

    mat4 camToWorld(right4, up4, forward4, from4);

    return camToWorld;
}

// Getters
vec4 Camera::getPosition() {
    return position;
}

mat4 Camera::getR() {
    return this->R;
}
float Camera::getYaw() {
    return this->yaw;
}

// Setters
void Camera::setPosition(vec4 position) {
    this->position = position;
}

void Camera::setR(mat4 R) {
    this->R = R;
}

void Camera::setYaw(float yaw) {
    this->yaw = yaw;
}
