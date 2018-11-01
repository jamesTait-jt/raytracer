#include "Material.h"

/* ----------------------------------------------------------------------------*/
/* COLOUR BOIS                                                                 */

vec3 red      (0.75f , 0.15f , 0.15f );
vec3 yellow   (0.75f , 0.75f , 0.15f );
vec3 green    (0.15f , 0.75f , 0.15f );
vec3 cyan     (0.15f , 0.75f , 0.75f );
vec3 blue     (0.15f , 0.15f , 0.75f );
vec3 purple   (0.75f , 0.15f , 0.75f );
vec3 white    (0.75f , 0.75f , 0.75f );
vec3 black    (0.00f , 0.00f , 0.00f );
vec3 darkRed  (0.902f, 0.153f, 0.225f);
vec3 babyBlue (0.8f  , 1.0f  , 1.0f  );
vec3 pink     (1.0f  , 0.6f  , 1.0f  );

/* ----------------------------------------------------------------------------*/
/* ILLUMINATION BOIS                                                                 */

vec3 ambientColour = 0.f * vec3(1, 1, 0.9);
vec3 diffuseColour = 1.0f * vec3(1, 1, 0.9);
vec3 specularColour = 1.0f * vec3(1, 1, 0.9);


float power = 30.0f;

float r = 0.05f;
LightSphere ls(vec4(0, -0.4, -0.9, 1.0), r, 50, ambientColour, diffuseColour, specularColour, power);
   
//LightSphere ls(vec4(-0.9, -0.9, 0.9, 1.0), r, 1, ambientColour, diffuseColour, specularColour, power);

/* ----------------------------------------------------------------------------*/
/* TEXTURE BOIS                                                                 */

// No specular lighting
Material defaultRed    = Material(red   , red   , red   , black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);
Material defaultYellow = Material(yellow, yellow, yellow, black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);
Material defaultGreen  = Material(green , green , green , black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);
Material defaultCyan   = Material(cyan  , cyan  , cyan  , black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);
Material defaultBlue   = Material(blue  , blue  , blue  , black, 0, 0.8f, 0, 0, false, 0.0f, 1.0f, false);
Material defaultPurple = Material(purple, purple, purple, black, 0, 0.8f, 0, 0, false, 0.0f, 1.0f, false);
Material defaultWhite  = Material(white , white , white , black, 0, 0.8f, 0, 0, false, 0.0f, 1.0f, false);

Material specularRed    = Material(red   , red   , white, black, 0.5, 0.1, 0, 5, false, 0.0f, 1.0f, false);
Material specularYellow = Material(yellow, yellow, white, black, 0.5, 0.1, 0, 2, false, 0.0f, 1.0f, false);
Material specularGreen  = Material(green , green , white, black, 0.5, 0.1, 0, 5, true , 0.2f, 1.0f, false);
Material specularCyan   = Material(cyan  , cyan  , white, black, 0.5, 0.1, 0, 2, false, 0.0f, 1.0f, false);
Material specularPurple = Material(purple, purple, white, black, 0.5, 0.1, 0, 2, false, 0.0f, 1.0f, false);
Material specularBabyBlue  = Material(babyBlue, babyBlue, white, black, 0.5, 0.1, 0, 5, true , 0.1f, 1.0f, false);

// Reflective material
//Material reflectiveRed    = Material(red   , red   , red   , black, 0, true, 0.6f, 1.0f, false);
//Material reflectiveYellow = Material(yellow, yellow, yellow, black, 0, true, 0.6f, 1.0f, false);
//Material reflectiveGreen  = Material(green , green , green , black, 0, true, 0.6f, 1.5f, false);
//Material reflectiveCyan   = Material(cyan  , cyan  , cyan  , black, 0, true, 0.6f, 1.0f, false);
Material reflectiveBlue   = Material(blue  , blue  , blue  , black, 0.9f, 0, 0, 0, true, 0.4f, 0.0f, false);
Material reflectiveDarkRed   = Material(darkRed  , darkRed  , darkRed  , black, 0.9f, 0, 0, 0, true, 0.2f, 0.0f, false);
//Material reflectivePurple = Material(purple, purple, purple, black, 0, true, 0.6f, 1.0f, false);
//Material reflectiveWhite  = Material(white , white , white , black, 0, true, 0.6f, 1.0f, false);

Material mirror = Material(white, white, white, black, 0.99f, 0, 0, 0, true, 1.0f, 0.0f, false);
Material specularBlue  = Material(blue,  blue, white, black, 0.5, 0.1, 0, 5, true , 0.2f, 1.0f, false);
Material specularPink  = Material(pink,  pink, white, black, 0.5, 0.1, 0, 200, true , 0.1f, 1.0f, false);
Material specularWhite  = Material(white, white, white, black, 0.5, 0.1, 0, 200, true , 0.3f, 1.0f, false);

// Refractive
Material refractiveGreen = Material(green , green , green , black, 0, 0, 0.8, 0, false, 0.6f, 1.5f, true);
Material refractiveWhite = Material(white, white, white, black, 0.05, 0, 0.9, 10, false, 0.6f, 1.5f, true);
Material fresnelWhite = Material(white, white, white, black, 0.05, 0, 0.9, 10, true, 1.0f, 1.8f, true);

//Back wall material
vec3 turquoise(0.43f, 0.827f, 0.819f);
Material backWall = Material(turquoise, turquoise, turquoise, black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);

//Right wall material
vec3 bright_purple(0.565f, 0.408f, 0.745f);
Material rightWall = Material(bright_purple, bright_purple, bright_purple, black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);

//Top wall material
Material topWall = Material(white, white, white, black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);

//Bottom wall material
Material bottomWall = Material(white, white, white, black, 0, 0.3f, 0, 0, true, 0.3f, 1.0f, false);

//Left Wall material
Material leftWall = Material(darkRed, darkRed, darkRed, black, 0, 0.3f, 0, 0, false, 0.0f, 1.0f, false);
