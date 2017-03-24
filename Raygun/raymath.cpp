//
//  raymath.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "raymath.hpp"

//////////////////////// RAY CLASS ////////////////////////////
vec3f Ray::GetStartPos(void){
    return RayOrigin;
}

Ray::Ray(vec3f origin, vec3f direction){
    for(int i =0; i<3; i++){
        RayOrigin.coords[i] = origin.coords[i];
        RayDirection.coords[i] = direction.coords[i];
        InvDirection.coords[i] = 1.0f/RayDirection.coords[i];
        InvDirSign.coords[i] = InvDirection.coords[i] < 0;
    }
}
void Ray::SetRayOrigin(vec3f origin){
    RayOrigin.x = origin.x;
    RayOrigin.y = origin.y;
    RayOrigin.z = origin.z;
}
vec3f Ray::GetDirection(void){
    return RayDirection;
}
vec3f Ray::GetInvDirection(void){
    return InvDirection;
}
vec3<bool> Ray::GetInvDirectionSign(void) {
	return InvDirSign;
}
void Ray::SetDirection(vec3f direction){
    RayDirection.x = direction.x;
    RayDirection.y = direction.y;
    RayDirection.z = direction.z;
};

