//
//  world.cpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "world.hpp"

color world::background_color = color(5,5,5);
vec3f world::sunlightPosition;
vec3f world::sunlightDirection;

void world::assembleCameraCoords(std::vector<float> *eye_origin,std::vector<float> *camera_center, int width, int height, float field_of_view, std::vector<float> *eye_u, std::vector<float> *eye_v, std::vector<float> *L_vector,float * pixel_width, float* pixel_height){
    
    std::vector<float> eye_normal = std::vector<float>(3);
    std::vector<float> v_up = std::vector<float>(3);

    
    
    eye_normal = *eye_origin;
    eye_normal[2] = -eye_normal[2];
    NormaliseVector(&eye_normal);
    v_up[0] = 0;
    v_up[1] = 1;
    v_up[2] = (*eye_origin)[2];
    NormaliseVector(&v_up);
    std::vector<float> w = Vec3Sub(*eye_origin,eye_normal);
    NormaliseVector(&w);
    *eye_u = Vec3CrossProduct(v_up, w);
    NormaliseVector(eye_u);
    *eye_v = Vec3CrossProduct(w, *eye_u);
    *pixel_height = tan((field_of_view/360) * PI)*(2*(*eye_origin)[2]);
    *pixel_width = *pixel_height * ((float)width/(float)height);
    
    
    for (auto i =0; i<3; i++){
        (*L_vector)[i] = (*camera_center)[i] + (*eye_u)[i]*(*pixel_width/2.0f) - (*eye_v)[i]*(*pixel_height/2.0f);
    }
}
