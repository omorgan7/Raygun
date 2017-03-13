//
//  raymath.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "raymath.hpp"

////////////// INLINE MATH FUNCTIONS ///////////////////////


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

