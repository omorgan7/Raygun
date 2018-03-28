//
//  raymath.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "raymath.hpp"

//////////////////////// RAY CLASS ////////////////////////////
Ray::Ray(){
}
vec3f Ray::GetStartPos(){
    return RayOrigin;
}

Ray::Ray(vec3f origin, vec3f direction){
    RayOrigin = origin;
    RayDirection = direction;
    InvDirection = 1.0f / direction;
    InvDirSign = sign(InvDirection);
    
}
void Ray::SetRayOrigin(vec3f origin){
    RayOrigin = origin;
}
vec3f Ray::GetDirection(){
    return RayDirection;
}
vec3f Ray::GetInvDirection(){
    return InvDirection;
}
vec3b Ray::GetInvDirectionSign() {
	return InvDirSign;
}
void Ray::SetDirection(vec3f direction){
    RayDirection = direction;
    InvDirection = 1.0f / direction;
    InvDirSign = sign(InvDirection);
};

