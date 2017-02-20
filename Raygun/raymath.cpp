//
//  raymath.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "raymath.hpp"

////////////// INLINE MATH FUNCTIONS ///////////////////////
void NormaliseVector(std::vector<float> *vec){
    double length = 0.0f;
    for(auto i = vec->begin(); i != vec->end(); i++){
        length+= (*i)*(*i);
    }
    length = powf(length,0.5f);
    for(auto i = vec->begin(); i != vec->end(); i++){
        (*i) /= length;
    }
}

float Vec3DotProduct(std::vector<float> v1, std::vector<float> v2){
    assert(v1.size() == 3);
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

std::vector<float> Vec3CrossProduct(std::vector<float> u, std::vector<float> v){
    assert(u.size() == 3);
    assert(v.size() == 3);
    std::vector<float> crossProduct = std::vector<float>(3);
    crossProduct[0] = u[1]*v[2] - u[2]*v[1];
    crossProduct[1] = u[2]*v[0] - u[0]*v[2];
    crossProduct[2] = u[0]*v[1] - u[1]*v[0];
    return crossProduct;
};

//////////////////////// RAY CLASS ////////////////////////////
std::vector<float> Ray::GetStartPos(void){
    return RayOrigin;
}
Ray::Ray(std::vector<float> origin, std::vector<float> direction){
    RayOrigin = origin;
    RayDirection = direction;
}
void Ray::SetRayOrigin(float x, float y, float z){
    RayOrigin[0] = x;
    RayOrigin[1] = y;
    RayOrigin[2] = z;
}
std::vector<float> Ray::GetDirection(void){
    return RayDirection;
}
void Ray::SetDirection(float x, float y, float z){
    RayDirection[0] = x;
    RayDirection[1] = y;
    RayDirection[2] = z;
};

