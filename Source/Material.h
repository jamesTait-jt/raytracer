#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Material {

    private:
        vec3 m_amb;
        vec3 m_diff;
        vec3 m_spec;
        vec3 m_emi;

        //Photon coefficients
        float coef_spec;
        float coef_diff;
        float coef_trans;

        float shininess;
        bool reflective;
        float reflectRatio;
        bool transparent;
        float refractiveIndex;

    public:
        // Constructor
        Material(vec3 m_amb, vec3 m_diff, vec3 m_spec, vec3 m_emi, float coef_spec, float coef_diff, float coef_trans, float shininess, bool reflective, float reflectRatio, float refractiveIndex, bool transparent);

        // Getters
        vec3 getAmbient();
        vec3 getDiffuse();
        vec3 getSpecular();
        vec3 getEmitted();

        float getCoefSpec();
        float getCoefDiff();
        float getCoefTrans();

        float getShininess();
        bool isReflective();
        float getReflectRatio();
        bool isTransparent();
        float getRefractiveIndex();

        // Setters
        void setAmbient(vec3 ambient);
        void setDiffuse(vec3 diffuse);
        void setSpecular(vec3 specular);
        void setEmitted(vec3 emitted);

        void setCoefSpec(float coef_spec);
        void setCoefDiff(float coef_diff);
        void setCoefTrans(float coef_trans);

        void setShininess(float shininess);
        void setReflective(bool reflective);
        void setReflectRatio(float reflectRatio);
        void setTransparent(bool transparent);
        void setRefractiveIndex(float refractiveIndex);
};


#endif
