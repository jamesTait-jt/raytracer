# Photon Mapper

### Introduction
For the raytracer we wanted to dedicate our time to producing a spectacular image and optimise in areas where necessary, rather than focusing on just optimising. We spent most of our time implementing global illumination and optimising its process.

As specified by the unit director, this report aims to simply present the features we have added to our raytracer rather than explaining their theory and implementation. Below we have list all of the additional features we have added to our raytracer past the basic implementation.

### Raytracer Features

#### Specular Highlights

We used the Blinn shader to give specular highlights on certain materials. We originally implemented the Phong shader, however we wanted to be more efficient and not have to calculate the dot product for each intersection, so we decided to go with the Blinn-Phong model.

#### Reflections, Refractions and Fresnel

Adding realistic and correct reflections and refractions to the project took a long time as we had to perfect this both for the photon mapping case, as well as for general raytracing. We correctly combined our reflections and refractions to find a fresnel ratio meaning objects could be partially reflective and transmissive at the same time. We tested this heavily by checking how the light interacted with glass spheres until it matched the theoretical description of light reflection, refraction and fresnel. Below, an early picture of the fresnel effect is displayed for a transmissive block.



[final]: https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 2"
