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

class Sphere{
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
    color DiffuseColorCalc(Ray R);
    color SpecularColorCalc(Ray ray);
    float calculateInterSectionProduct(Ray R, float *dist_dot_product);
    
private:
    color SphereColor = color(0,0,255);
    float radius;
    float ambientCoeff = 0.2, diffuseCoeff = 0.4, specularCoeff = 0.4, reflectCoeff;
    std::vector<float> SphereOrigin = std::vector<float>(3);
};

class triangle{
public:
    triangle(std::vector<std::vector<float> > vertices);
    color GetColor(void);
    void SetColor(color C);
    void SetVertexCoord(std::vector<float> vertex, int vertex_index);   
    void ChangeVertexCoord(std::vector<float> vertex, int vertex_index);
    color AmbientRayInterSection(Ray R);
    color DiffuseColorCalc(void);
    color SpecularColorCalc(Ray ray);
    float calculateInterSectionProduct(Ray R, int *SUCCESS);
private:
    color TriangleColor = color(255,0,255);
    float ambientCoeff = 0.1, diffuseCoeff = 0.45, specularCoeff = 0.45, reflectCoeff;
    std::vector<float> vertex_0, vertex_1, vertex_2;
    std::vector<float> triangleNormal = std::vector<float>(3);
    void ComputeNormal(void);
    void flipNormal(void);
};

#endif /* shape_hpp */