#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

#define SCREEN_HEIGHT  500
#define SCREEN_WIDTH  SCREEN_HEIGHT
const vec3 INTENSITY_WEIGHTS(0.2989, 0.5870, 0.1140);
class ImageBuffer{

    public:
        vector<vec3> image;

    public:
        // CONSTRUCTOR
        ImageBuffer(vector<vec3> image);

        //Returns a greyscale version of the current image stored in the buffer
        void ConvertToGreyscale(vector<vector<float>> & greyscaleImage);

        //Convolve a black and white image
        void ConvolveImage(float kernel[3][3], vector<vector<float>> & greyscaleImage, vector<vector<float>> & convolvedImage);

        //Computes the sobel gradient of the image stored in the image buffer
        void SobelGradient(vector<vec2> & gradientImage);

        // GETTERS
        vector<vec3> getImage();

        // SETTERS
        void setImage(vector<vec3> image);
};
#endif
