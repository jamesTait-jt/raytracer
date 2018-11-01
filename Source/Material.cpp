#include "Material.h"

Material::Material(vec3 m_amb, vec3 m_diff, vec3 m_spec, vec3 m_emi, float coef_spec, float coef_diff, float coef_trans, float shininess, bool reflective, float reflectRatio, float refractiveIndex, bool transparent) {

    setAmbient(m_amb);
    setDiffuse(m_diff);
    setSpecular(m_spec);
    setEmitted(m_emi);

    setCoefSpec(coef_spec);
    setCoefDiff(coef_diff);
    setCoefTrans(coef_trans);

    setShininess(shininess);
    setReflective(reflective);
    setReflectRatio(reflectRatio);
    setTransparent(transparent);
    setRefractiveIndex(refractiveIndex);

}

// Getters
vec3 Material::getAmbient() {
    return m_amb;
}

vec3 Material::getDiffuse() {
    return m_diff;
}

vec3 Material::getSpecular() {
    return m_spec;
}

vec3 Material::getEmitted() {
    return m_emi;
}

float Material::getCoefSpec() {
    return coef_spec;
}

float Material::getCoefDiff() {
    return coef_diff;
}

float Material::getCoefTrans() {
    return coef_trans;
}

float Material::getShininess() {
    return shininess;
}

bool Material::isReflective() {
    return reflective;
}

float Material::getReflectRatio() {
    return reflectRatio;
}

bool Material::isTransparent() {
    return transparent;
}

float Material::getRefractiveIndex() {
    return refractiveIndex;
}

// Setters
void Material::setAmbient(vec3 ambient) {
    m_amb = ambient;
}

void Material::setDiffuse(vec3 diffuse) {
    m_diff = diffuse;
}

void Material::setSpecular(vec3 specular) {
    m_spec = specular;
}

void Material::setEmitted(vec3 emitted) {
    m_emi = emitted;
}

void Material::setCoefSpec(float coef_spec) {
    this->coef_spec = coef_spec;
}

void Material::setCoefDiff(float coef_diff) {
    this->coef_diff = coef_diff;
}

void Material::setCoefTrans(float coef_trans) {
    this->coef_trans = coef_trans;
}

void Material::setShininess(float shininess) {
    this->shininess = shininess;
}

void Material::setReflective(bool reflective) {
    this->reflective = reflective;
}

void Material::setReflectRatio(float reflectRatio) {
    this->reflectRatio = reflectRatio;
}

void Material::setTransparent(bool transparent) {
    this->transparent = transparent;
}

void Material::setRefractiveIndex(float refractiveIndex) {
    this->refractiveIndex = refractiveIndex;
}
