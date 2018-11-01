#include <assert.h>
#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
//#include "TestModelH.h"
#include <stdint.h>
#include <memory>
#include <omp.h>

#include "Camera.h"
#include "Light.h"
#include "Ray.h"
#include "ImageBuffer.h"
#include "LightSphere.h"
#include "Material.h"
#include "Triangle.h"
#include "Sphere.h"
#include "PhotonMap.h"
#include "LightsAndMaterials.h"
#include "KDTree.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update(
Camera& camera,
LightSphere& light
);

void Draw(
screen* screen,
Camera& camera,
LightSphere& ls,
PhotonMap pmap,
vector<Shape *> shapes,
KDTree * globalTracedPointer,
vector<Photon> nearestPhotonsPointer,
vec4 testPosition
);

void loadShapes(
vector<Triangle>& triangles,
vector<Sphere>& spheres
);


/* ----------------------------------------------------------------------------*/
/* GLOBAL                                                                 */

#define NUM_PHOTONS 1000
#define NUM_NEAREST_PHOTONS 5
#define FULLSCREEN_MODE true
#define FOCAL_LENGTH SCREEN_HEIGHT
#define DRAW_ITERATIONS 3
#define ANTI_ALIASING true

/* ----------------------------------------------------------------------------*/
/* BEGIN PROGRAM                                                               */


int main (int argc, char* argv[]) {

    omp_set_num_threads(6);
    
    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);

    // Initialise vector of triangles and fill it with triangles representing
    // cornell room
    vector<Triangle> triangles;
    vector<Sphere> spheres;

    loadShapes(triangles, spheres);

    vector<Shape *> shapes;
    for (int i = 0 ; i < triangles.size(); i++) {
        Shape * sptr (&triangles[i]);
        shapes.push_back(sptr);
    }

    for (int i = 0 ; i < spheres.size(); i++) {
        Shape * sptr (&spheres[i]);
        shapes.push_back(sptr);
    }

    cout << "REACHED IN MAIN" << endl;

    // Create a new camera
    Camera camera(vec4(0, 0, -3, 1));

    vec4 testPosition = vec4(0, 0, 1, 1);
    vector<Photon> nearestPhotons;


    PhotonMap pmap(ls, NUM_PHOTONS, NUM_NEAREST_PHOTONS, shapes);
    KDTree * globalTracedPointer = pmap.GetGlobalPhotonsPointer();

    int i = 1;
    while (NoQuitMessageSDL() && i < DRAW_ITERATIONS) {
        i++;
        Update(camera, ls);
        Draw(screen, camera, ls, pmap, shapes, globalTracedPointer, nearestPhotons, testPosition);
        SDL_Renderframe(screen);
    }

    SDL_SaveImage(screen, "screenshot.bmp");
    KillSDL(screen);

    delete globalTracedPointer;

    return 0;
}

/*Place your drawing here*/
void Draw (screen* screen, Camera& camera, LightSphere& ls, PhotonMap pmap, vector<Shape *> shapes, KDTree* globalTracedPointer, vector<Photon> nearestPhotons, vec4 testPosition) {

    /* Clear buffer */
    memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

    //Create an ImageBuffer to store the image
    vector<vec3> buffer(SCREEN_HEIGHT*SCREEN_WIDTH);
    ImageBuffer imBuffer(buffer);

    /******Ray Casting******/
    int pixels = 0;
    int threads = omp_get_num_threads();
    #pragma omp parallel for //schedule(dynamic, (int)(SCREEN_WIDTH/threads))
    for (int x = 0 ; x < SCREEN_WIDTH ; x++) {
        for (int y = 0 ; y < SCREEN_HEIGHT ; y++) {
            # pragma omp critical
            {
                pixels++;
                float pct = (float) pixels / (SCREEN_HEIGHT * SCREEN_WIDTH) * 100;
                cout << pct << "%\r";
            }

            // Change the ray's direction to work for the current pixel (pixel space -> Camera space)
            vec4 dir((x - SCREEN_WIDTH / 2) , (y - SCREEN_HEIGHT / 2) , FOCAL_LENGTH , 1);

            // Create a ray that we will change the direction for below
            Ray ray(camera.getPosition(), dir);
            ray.rotateRay(camera.getYaw());

            // Initialise the closest intersection - will be updated in the for loop
            Intersection closestIntersection;

            //if (ray.closestIntersection(triangles, closestIntersection)) {
            if (ray.closestIntersection(shapes, closestIntersection)) {

                vec4 pos = camera.getPosition();
                vec3 incidentDir(
                        closestIntersection.position.x - pos.x,
                        closestIntersection.position.y - pos.y,
                        closestIntersection.position.z - pos.z
                );

                vec4 incidentDir4(normalize(incidentDir), 1);

                Ray incidentRay(closestIntersection.position, incidentDir4);


                vec3 finalColour = pmap.RadianceEstimate(NUM_NEAREST_PHOTONS, closestIntersection, shapes, incidentRay, camera, ls);

                imBuffer.image[((SCREEN_WIDTH*x) + y)] = finalColour;
                PutPixelSDL(screen, x, y, finalColour);
            }
            else {
                imBuffer.image[((SCREEN_WIDTH*x) + y)] = vec3(0,0,0);
                PutPixelSDL(screen, x, y, vec3(0,0,0));
            }

        Intersection testIntersection;
        }
    }

    /*******Anti Alisaing********/
    cout << "Anti Aliasing" << endl;
    if (ANTI_ALIASING) {
        int samples = 12;
        //Get the gradient image
        vector<vec2> gradientImage;
        imBuffer.SobelGradient(gradientImage);

        //Draw the image stored in the buffer
        pixels = 0;
        int pixels_size = gradientImage.size();
        #pragma omp parallel for schedule(dynamic,(int)(gradientImage.size()/threads))
        for( int i = 0; i < gradientImage.size(); i++) {
            # pragma omp critical
            {
                pixels++;
                //cout << pixels << endl;
                float pct = (float) pixels / pixels_size * 100;
                cout << pct << "%\r";
            }

            int x = (int)gradientImage[i].x;
            int y = (int)gradientImage[i].y;
            //Check if the pixel in the gradient image is an edge -> Perform Anti Aliasing
            //Get the ray we need tp sample with
            Ray ray(camera.getPosition(), vec4(0, 0, 0, 0));
            vec4 dir((x - SCREEN_WIDTH / 2) , (y - SCREEN_HEIGHT / 2) , FOCAL_LENGTH , 1);
            ray.setDirection(dir);
            ray.rotateRay(camera.getYaw());

            vector<Ray> AArays = ray.SuperSamplePixel(samples);

            //Compute each of their colours
            vector<vec3> colourValues(samples);
            for(int i = 0; i < samples; i++){

                Intersection closestIntersection;
                if (AArays[i].closestIntersection(shapes, closestIntersection)) {
                    vec4 pos = camera.getPosition();
                    vec4 incidentDir(
                        closestIntersection.position.x - pos.x,
                        closestIntersection.position.y - pos.y,
                        closestIntersection.position.z - pos.z,
                        1
                        );

                    Ray incidentRay(closestIntersection.position, incidentDir);
                    colourValues[i] = pmap.RadianceEstimate(NUM_NEAREST_PHOTONS, closestIntersection, shapes, incidentRay, camera, ls);
                }
                else{
                    colourValues[i] = vec3(0.0f,0.0f,0.0f);
                }
            }
            //Average the colour and print it to the screen
            float x_r = 0;
            float y_r = 0;
            float z_r = 0;
            for(int j = 0; j < samples; j++){
                x_r += colourValues[j].x;
                y_r += colourValues[j].y;
                z_r += colourValues[j].z;
            }
            PutPixelSDL(screen, x, y, vec3(x_r/(samples), y_r/(samples), z_r/(samples)));
        }
    }
}


/*Place updates of parameters here*/
void Update(Camera& camera, LightSphere& light) {
    static int t = SDL_GetTicks();
    /* Compute frame time */
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;

    cout << "Render time: " << dt << "ms." << endl;

    /* Update variables*/

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDL_SCANCODE_UP]) {
        camera.moveForwards(0.1);
    }
    if (keystate[SDL_SCANCODE_DOWN]) {
        camera.moveBackwards(0.1);
    }
    if (keystate[SDL_SCANCODE_LEFT]) {
        camera.rotateLeft(0.1);
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
        camera.rotateRight(0.1);
    }
    if (keystate[SDL_SCANCODE_A]) {
        light.translateLeft(0.1);
    }
    if (keystate[SDL_SCANCODE_D]) {
        light.translateRight(0.1);
    }
    if (keystate[SDL_SCANCODE_Q]) {
        light.translateUp(0.1);
    }
    if (keystate[SDL_SCANCODE_E]) {
        light.translateDown(0.1);
    }
    if (keystate[SDL_SCANCODE_W]) {
        light.translateForwards(0.1);
    }
    if (keystate[SDL_SCANCODE_S]) {
        light.translateBackwards(0.1);
    }
}


void loadShapes(vector<Triangle>& triangles, vector<Sphere>& spheres) {
    
    // ---------------------------------------------------------------------------
    // Room

    float l = 555;			// Length of Cornell Box side.

    vec4 A(l,0,0,1);
    vec4 B(0,0,0,1);
    vec4 C(l,0,l,1);
    vec4 D(0,0,l,1);

    vec4 E(l,l,0,1);
    vec4 F(0,l,0,1);
    vec4 G(l,l,l,1);
    vec4 H(0,l,l,1);

    // Triangles now take a material as an argument rather than a colour
    // Floor:
    Triangle flrTri1 = Triangle(C, B, A, specularWhite);
    triangles.push_back(flrTri1);

    Triangle flrTri2 = Triangle(C, D, B, specularWhite);
    triangles.push_back(flrTri2);

    // Left wall
    Triangle lftWall1 = Triangle(A, E, C, leftWall);
    triangles.push_back(lftWall1);

    Triangle lftWall2 = Triangle(C, E, G, leftWall);
    triangles.push_back(lftWall2);

    // Right wall
    Triangle rghtWall1 = Triangle(F, B, D, rightWall);
    triangles.push_back(rghtWall1);

    Triangle rghtWall2 = Triangle(H, F, D, rightWall);
    triangles.push_back(rghtWall2);

    // Ceiling
    Triangle clng1 = Triangle(E, F, G, topWall);
    triangles.push_back(clng1);

    Triangle clng2 = Triangle(F, H, G, topWall);
    triangles.push_back(clng2);

    // Back wall
    Triangle bckWall1 = Triangle(G, D, C, backWall);
    triangles.push_back(bckWall1);

    Triangle bckWall2 = Triangle(G, H, D, backWall);
    triangles.push_back(bckWall2);

    // ---------------------------------------------------------------------------
    // Short block

    A = vec4(240,0,234,1);  //+120 in z -50 in x
    B = vec4( 80,0,185,1);
    C = vec4(190,0,392,1);
    D = vec4( 32,0,345,1);

    E = vec4(240,165,234,1);
    F = vec4( 80,165,185,1);
    G = vec4(190,165,392,1);
    H = vec4( 32,165,345,1);

    // Front
    triangles.push_back(Triangle(E,B,A,defaultBlue));
    triangles.push_back(Triangle(E,F,B,defaultBlue));

    // Front
    triangles.push_back(Triangle(F,D,B,defaultBlue));
    triangles.push_back(Triangle(F,H,D,defaultBlue));

    // BACK
    triangles.push_back(Triangle(H,C,D,defaultBlue));
    triangles.push_back(Triangle(H,G,C,defaultBlue));

    // LEFT
    triangles.push_back(Triangle(G,E,C,defaultBlue));
    triangles.push_back(Triangle(E,A,C,defaultBlue));

    // TOP
    triangles.push_back(Triangle(G,F,E,defaultBlue));
    triangles.push_back(Triangle(G,H,F,defaultBlue));

    // ---------------------------------------------------------------------------
    // Tall block

    A = vec4(443,0,247,1);
    B = vec4(285,0,296,1);
    C = vec4(492,0,406,1);
    D = vec4(334,0,456,1);

    E = vec4(443,330,247,1);
    F = vec4(285,330,296,1);
    G = vec4(492,330,406,1);
    H = vec4(334,330,456,1);

    // Front
   
    triangles.push_back(Triangle(E,B,A,fresnelWhite));
    triangles.push_back(Triangle(E,F,B,fresnelWhite));

    // Front
    triangles.push_back(Triangle(F,D,B,fresnelWhite));
    triangles.push_back(Triangle(F,H,D,fresnelWhite));

    // BACK
    triangles.push_back(Triangle(H,C,D,fresnelWhite));
    triangles.push_back(Triangle(H,G,C,fresnelWhite));

    // LEFT
    triangles.push_back(Triangle(G,E,C,fresnelWhite));
    triangles.push_back(Triangle(E,A,C,fresnelWhite));

    // TOP
    triangles.push_back(Triangle(G,F,E,fresnelWhite));
    triangles.push_back(Triangle(G,H,F,fresnelWhite));

    // ---------------------------------------------------------------------------
    // Sphere

    //Sphere for the right wall
    spheres.push_back(Sphere(vec4(0, 0, 1.2, 1), 0.6, reflectiveDarkRed));

    //Central smaller sphere
    spheres.push_back(Sphere(vec4(0.5, 165/l - 0.15, -0.2, 1), 0.25, refractiveWhite));

    //Left diffuse phere
    spheres.push_back(Sphere(vec4(-0.2, 0.85, -0.6, 1), 0.19, specularPink));

    // ----------------------------------------------
    // Scale to the volume [-1,1]^3

    for (size_t i = 0 ; i < triangles.size() ; ++i) {
        triangles[i].setV0(triangles[i].getV0() * (2 / l));
        triangles[i].setV1(triangles[i].getV1() * (2 / l));
        triangles[i].setV2(triangles[i].getV2() * (2 / l));

        triangles[i].setV0(triangles[i].getV0() - vec4(1,1,1,1));
        triangles[i].setV1(triangles[i].getV1() - vec4(1,1,1,1));
        triangles[i].setV2(triangles[i].getV2() - vec4(1,1,1,1));

        vec4 newV0 = triangles[i].getV0();
        newV0.x *= -1;
        newV0.y *= -1;
        newV0.w = 1.0;
        triangles[i].setV0(newV0);

        vec4 newV1 = triangles[i].getV1();
        newV1.x *= -1;
        newV1.y *= -1;
        newV1.w = 1.0;
        triangles[i].setV1(newV1);

        vec4 newV2 = triangles[i].getV2();
        newV2.x *= -1;
        newV2.y *= -1;
        newV2.w = 1.0;
        triangles[i].setV2(newV2);

        triangles[i].computeAndSetNormal();
    }
}
