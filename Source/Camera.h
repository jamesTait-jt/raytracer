#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Camera {

    private:
        vec4 position;
        mat4 R;
        float yaw;
        
        // rotate the camera to look at a certain point (does it work?)
        mat4 lookAt(vec3 from, vec3 to);

    public:
        // Constructor
        Camera(vec4 position);

        // Getters
        vec4 getPosition();
        mat4 getR();
        float getYaw();

        // Setters
        void setPosition(vec4 position);
        void setR(mat4 R);
        void setYaw(float yaw);

        // Camera rotation
        void rotateLeft(float yaw);
        void rotateRight(float yaw);
        

        // Camera translation
        void moveForwards(float distance);
        void moveBackwards(float distance);
};
#endif
