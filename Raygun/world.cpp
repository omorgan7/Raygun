//
//  world.cpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "world.hpp"

color world::background_color = color(5,5,5);
std::vector<float> world::sunlightPosition = std::vector<float>(3);
std::vector<float> world::sunlightDirection = std::vector<float>(3);
