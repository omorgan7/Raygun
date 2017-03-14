//
//  raymath.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "raymath.hpp"

//////////////////////// RAY CLASS ////////////////////////////
std::vector<float> Ray::GetStartPos(void){
    return RayOrigin;
}
Ray::Ray(std::vector<float> origin, std::vector<float> direction){
    RayOrigin = origin;
    RayDirection = direction;
    InvDirection = {1.0f/RayDirection[0],1.0f/RayDirection[1],1.0f/RayDirection[2]};
}
void Ray::SetRayOrigin(float x, float y, float z){
    RayOrigin[0] = x;
    RayOrigin[1] = y;
    RayOrigin[2] = z;
}
std::vector<float> Ray::GetDirection(void){
    return RayDirection;
}
std::vector<float> Ray::GetInvDirection(void){
    return InvDirection;
}
void Ray::SetDirection(float x, float y, float z){
    RayDirection[0] = x;
    RayDirection[1] = y;
    RayDirection[2] = z;
};

