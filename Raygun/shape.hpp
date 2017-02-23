 //
//  shape.hpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include <vector>
//#include <assert.h>
#include <cmath>

#include "color.hpp"
#include "raymath.hpp"
#include "world.hpp"

#ifndef shape_hpp
#define shape_hpp

class object{
public:
    color GetColor(void);
    void SetColor(color C);
    virtual color AmbientRayInterSection(Ray R) = 0;
    virtual color DiffuseColorCalc(void) = 0;
    virtual color SpecularColorCalc(Ray ray) = 0;
    virtual float calculateInterSectionProduct(Ray ray, int * success) =0;
protected:
    color Color;
    float ambientCoeff, diffuseCoeff, specularCoeff, reflectCoeff;
};

class Sphere : public object{
public:
    Sphere(float x, float y, float z, float r);
    color GetColor(void);
    void SetColor(color C);
    float GetRadius(void);
    void SetRadius(float newRadius);
    void SetX(float x);
    void SetY(float y);  
    void SetZ(float z);
    std::vector<float> FindSurfaceNormal(std::vector<float> coords);
    color AmbientRayInterSection(Ray R);
    color DiffuseColorCalc(void);
    color SpecularColorCalc(Ray ray);
    float calculateInterSectionProduct(Ray ray, int * success);
    
private:
    float radius;
    std::vector<float> SphereOrigin = std::vector<float>(3);
    std::vector<float> surfaceCoordinates = std::vector<float>(3);
    std::vector<float> normal = std::vector<float>(3);
    float dist_dot_product;
};

class triangle : public object{
public:
    triangle(std::vector<std::vector<float> > vertices);
    color GetColor(void);
    void SetColor(color C);
    void SetVertexCoord(std::vector<float> vertex, int vertex_index);   
    void ChangeVertexCoord(std::vector<float> vertex, int vertex_index);
    color AmbientRayInterSection(Ray R);
    color DiffuseColorCalc(void);
    color SpecularColorCalc(Ray ray);
    float calculateInterSectionProduct(Ray ray, int * success);
private:
    std::vector<float> vertex_0, vertex_1, vertex_2;
    std::vector<float> triangleNormal = std::vector<float>(3);
    void ComputeNormal(void);
    void flipNormal(void);
};

#endif /* shape_hpp */
