//
//  shape.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "shape.hpp"

Sphere::Sphere(float x, float y, float z, float r){
    SetRadius(r);
    SetX(x);
    SetY(y);
    SetZ(z);
}
color Sphere::GetColor(void){
    return SphereColor;
}
void Sphere::SetColor(color C){
    SphereColor = C;
}
float Sphere::GetRadius(void){
    return radius;
}
void Sphere::SetRadius(float newRadius){
    radius = newRadius;
}
void Sphere::SetX(float x){
    SphereOrigin[0] = x;
}
void Sphere::SetY(float y){
    SphereOrigin[1] = y;
}
void Sphere::SetZ(float z){
    SphereOrigin[2] = z;
}
std::vector<float> Sphere::FindSurfaceNormal(std::vector<float> coords){
    //potential bugs: doesn't account for finding the surface normal for the same point as the origin.
    std::vector<float> normal = std::vector<float>(3);
    assert(coords.size() == 3);
    for(auto i = 0; i<3; i++){
        normal[i] = coords[i] - SphereOrigin[i];
    }
    NormaliseVector(&normal);
    return normal;
}
color Sphere::AmbientRayInterSection(Ray R){
    float f = 0.0f;
    auto distance = calculateInterSectionProduct(R, &f);
    if(distance<0){
        return world::background_color;
    }
    return SphereColor*ambientCoeff;
}
color Sphere::DiffuseColorCalc(Ray R){
    auto origin = R.GetStartPos();
    auto direction = R.GetDirection();
    float dist_dot_product = 0.0f;
    auto distance = calculateInterSectionProduct(R, &dist_dot_product);
    assert(distance>=0);
    auto d = -1*dist_dot_product + powf(distance,0.5f);
    std::vector<float> surfaceCoordinates = std::vector<float>(3);
    for(auto i = 0; i<3; i++){
        surfaceCoordinates[i] = origin[i] + d * direction[i];
    }
    auto normal = FindSurfaceNormal(surfaceCoordinates);
    auto lambertRay = Vec3DotProduct(normal,world::sunlightDirection);
    return SphereColor*diffuseCoeff*lambertRay;
}
color Sphere::SpecularColorCalc(Ray ray){
    auto origin = ray.GetStartPos();
    auto direction = ray.GetDirection();
    float dist_dot_product = 0.0f;
    auto distance = calculateInterSectionProduct(ray, &dist_dot_product);
    assert(distance>=0);
    auto d = -1*dist_dot_product + powf(distance,0.5f);
    std::vector<float> surfaceCoordinates = std::vector<float>(3);
    for(auto i = 0; i<3; i++){
        surfaceCoordinates[i] = origin[i] + d * direction[i];
    }
    auto normal = FindSurfaceNormal(surfaceCoordinates);
    auto reflectionFactor = 2.0f*Vec3DotProduct(normal,world::sunlightDirection);
    std::vector<float> reflectionVector = std::vector<float>(3);
    for(auto i = 0; i<3; i++) {
        reflectionVector[i] = normal[i]*reflectionFactor - world::sunlightDirection[i];
    }
    auto SpecRay = Vec3DotProduct(ray.GetDirection(),reflectionVector);
    if(SpecRay<0){
        return color(0,0,0);
    }
    auto intermediateProduct = SphereColor*specularCoeff*powf(SpecRay,20);
    //std::cout<<SpecRay<<"\n";
    return SphereColor*specularCoeff*powf(SpecRay,20);
}
float Sphere::calculateInterSectionProduct(Ray R, float *dist_dot_product){
    auto distance_2norm = 0.0f;
    std::vector<float> difference = std::vector<float>(3);
    auto origin = R.GetStartPos();
    for(auto i = 0; i<3; i++){
        difference[i] = origin[i] - SphereOrigin[i];
        distance_2norm += difference[i]*difference[i];
    }
    *dist_dot_product = Vec3DotProduct(R.GetDirection(), difference);
    return powf(*dist_dot_product,2) + radius*radius - distance_2norm;

}


