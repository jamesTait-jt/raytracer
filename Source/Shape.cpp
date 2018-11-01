#include "Shape.h"

// Constructor
Shape::Shape(Material material) {
    setMaterial(material);
}

// Getters
Material Shape::getMaterial() {
    return material;
}

// Setters
void Shape::setMaterial(Material material) {
    this->material = material;
}
