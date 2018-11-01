#include "ImageBuffer.h"
#include <iostream>

ImageBuffer::ImageBuffer(vector<vec3> image){
    this->image = image;
}

//Returns a greyscale version of the current image stored in the buffer
void ImageBuffer::ConvertToGreyscale(vector<vector<float>> & greyscaleImage){
    for(int x = 0; x < SCREEN_WIDTH; x++){
        for(int y = 0; y < SCREEN_HEIGHT; y++){
            vec3 val = this->image[((SCREEN_WIDTH*x) + y)];
            greyscaleImage[x][y] = (float)dot(val, INTENSITY_WEIGHTS);
        }
    }
}

//Convolve a black and white image
//kernel[x][y]
void ImageBuffer::ConvolveImage(float kernel[3][3], vector<vector<float>> & greyscaleImage, vector<vector<float>> & convolvedImage){
    //Setup the image for convolution
    vector<vector<float>> paddedImage(SCREEN_WIDTH+2,
                                         vector<float>(SCREEN_HEIGHT+2));

    for(int x = -1; x < SCREEN_WIDTH+1; x++){
        for(int y = -1; y < SCREEN_HEIGHT+1; y++){
            //Fill padding aroudn image
            if(x < 0 || x == SCREEN_WIDTH || y < 0 || y == SCREEN_HEIGHT){
                paddedImage[x+1][y+1] = 0;
            }
            //Else replace with actual image value
            else{
                paddedImage[x+1][y+1] = greyscaleImage[x][y];
            }
        }
    }

    //For every pixel in the image
    for(int x = 1; x < SCREEN_WIDTH+1; x++){
        for(int y = 1; y < SCREEN_HEIGHT+1; y++){

            float accumulator = 0;
            accumulator += paddedImage[x-1][y-1] * kernel[2][2];
            accumulator += paddedImage[x][y-1] * kernel[1][2];
            accumulator += paddedImage[x+1][y-1] * kernel[0][2];
            accumulator += paddedImage[x-1][y] * kernel[2][1];
            accumulator += paddedImage[x][y] * kernel[1][1];
            accumulator += paddedImage[x+1][y] * kernel[0][1];
            accumulator += paddedImage[x-1][y+1] * kernel[2][0];
            accumulator += paddedImage[x][y+1] * kernel[1][0];
            accumulator += paddedImage[x+1][y+1] * kernel[0][0];

            //Update the pixel with the convolved value
            convolvedImage[x-1][y-1] = abs(accumulator);
        }
    }
}

//Computes the sobel gradient of the image stored in the image buffer
void ImageBuffer::SobelGradient(vector<vec2> & gradientImage){

    //DEFINE an area to ignore which is very close to the light
    //Width:
    int width_offset = (int)SCREEN_WIDTH/5;
    int width_ignore = SCREEN_WIDTH/2;
    //Height:
    int height_offset = (int)SCREEN_HEIGHT/10;
    int height_ignore = SCREEN_HEIGHT/4;

    //1) Get a greyscale representation of the image
    vector<vector<float>> greyscaleImage(SCREEN_WIDTH,
                                         vector<float>(SCREEN_HEIGHT));
    ConvertToGreyscale(greyscaleImage);

    //2) Define the sobel kernels
    float sobelx[3][3];
    sobelx[0][0] = 1.0f; sobelx[1][0] = 0.0f; sobelx[2][0] = -1.0f;
    sobelx[0][1] = 2.0f; sobelx[1][1] = 0.0f; sobelx[2][1] = -2.0f;
    sobelx[0][2] = 1.0f; sobelx[1][2] = 0.0f; sobelx[2][2] = -1.0f;

    float sobely[3][3];
    sobely[0][0] = 1.0f; sobely[1][0] = 2.0f; sobely[2][0] = 1.0f;
    sobely[0][1] = 0.0f; sobely[1][1] = 0.0f; sobely[2][1] = 0.0f;
    sobely[0][2] = -1.0f; sobely[1][2] = -2.0f; sobely[2][2] = -1.0f;

    //3) Convolve the image with both kernels
    vector<vector<float>> convolvedX(SCREEN_WIDTH,
                                         vector<float>(SCREEN_HEIGHT));
    vector<vector<float>> convolvedY(SCREEN_WIDTH,
                                         vector<float>(SCREEN_HEIGHT));

    ConvolveImage(sobelx, greyscaleImage, convolvedX);
    ConvolveImage(sobely, greyscaleImage, convolvedY);

    //4) Threshold the integer representation and add it to the gradient
    // image vector, combining both gradients and return the vector
    float threshold = 0.3f;
    for(int x = 0; x < SCREEN_WIDTH; x++){
        for(int y = 0; y < SCREEN_HEIGHT; y++){
            if(convolvedX[x][y] > threshold && !((x > width_offset) && (x < SCREEN_WIDTH - width_offset)  && (y < height_offset*2)) ){
                gradientImage.emplace_back(vec2(x,y));
            }
            if(convolvedY[x][y] > threshold && !((x > width_offset) && (x < SCREEN_WIDTH - width_offset)  && (y < height_offset*2)) ){
                gradientImage.emplace_back(vec2(x,y));
            }
        }
    }
}

// GETTERS
vector<vec3> ImageBuffer::getImage() {
    return image;
}

// SETTERS
void ImageBuffer::setImage(vector<vec3> image) {
    this->image = image;
}



