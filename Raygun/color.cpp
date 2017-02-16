//
//  color.cpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "color.hpp"

color::color(unsigned char r, unsigned char g, unsigned char b){
    red = r;
    green = g;
    blue = b;
}
unsigned char color::Red(void){
    return red;
}
unsigned char color::Green(void){
    return green;
}
unsigned char color::Blue(void){
    return blue;
}
