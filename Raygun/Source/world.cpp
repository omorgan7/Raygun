//
//  world.cpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "world.hpp"

color world::background_color = color(5.0f,5.0f,5.0f);
vec3f world::sunlightPosition;
vec3f world::sunlightDirection;

void world::assembleCameraCoords(vec3f *eye_origin, vec3f *look_at, vec3f * look_up, int width, int height, float field_of_view,vec3f *eye_u,vec3f *eye_v,vec3f *L_vector,float * pixel_width, float* pixel_height,float focal_length){

    vec3f eye_normal = Vec3Sub(*eye_origin,*look_at);
    NormaliseVector(&eye_normal);

    *eye_u = Vec3CrossProduct(*look_up, eye_normal);
    NormaliseVector(eye_u);
    *eye_v = Vec3CrossProduct(eye_normal, *eye_u);
    *pixel_height = tan((field_of_view/360.0f) * PI)*(2.0f*focal_length);
    *pixel_width = *pixel_height * ((float)width/(float)height);
    
    vec3f C_vector = Vec3Sub(*eye_origin,Vec3ScalarMultiply(eye_normal,focal_length));
    *L_vector = Vec3Sub(Vec3Add(C_vector,Vec3ScalarMultiply(*eye_u,*pixel_width*0.5f)),Vec3ScalarMultiply(*eye_v, *pixel_height*0.5f));
}
