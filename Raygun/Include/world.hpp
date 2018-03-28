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
#define INVPI 0.3183099f

class world{
public:
    static color background_color;
    static vec3f sunlightPosition;
    static vec3f sunlightDirection;
    static void assembleCameraCoords(vec3f& eye_origin,
                                    vec3f& look_at,
                                    vec3f& look_up,
                                    int width,
                                    int height,
                                    float field_of_view,
                                    vec3f& eye_u,
                                    vec3f& eye_v,
                                    vec3f& L_vector,
                                    float& pixel_width,
                                    float& pixel_height,
                                     float focal_length);
};

#endif /* world_hpp */
