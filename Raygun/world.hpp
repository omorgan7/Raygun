//
//  world.hpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#ifndef world_hpp
#define world_hpp

#include <stdio.h>
#include "color.hpp"
#include "raymath.hpp"
#include <vector>

#define PI 3.14159265f

class world{
public:
    static color background_color;
    static vec3f sunlightPosition;
    static vec3f sunlightDirection;
    static void assembleCameraCoords(std::vector<float> *eye_origin,
                                     std::vector<float> *camera_center,
                                     int width, int height, float field_of_view,
                                     std::vector<float> *eye_u,
                                     std::vector<float> *eye_v,
                                     std::vector<float> *L_vector,
                                     float * pixel_width, float* pixel_height);
};

#endif /* world_hpp */
