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
#include <vector>

class world{
public:
    static color background_color;
    static std::vector<float> sunlightPosition;
    static std::vector<float> sunlightDirection;
};

#endif /* world_hpp */
