//
//  color.hpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#ifndef color_hpp
#define color_hpp

//#include <assert.h>
#include <algorithm>
#include <iostream>

#include "Containers.hpp"

#include "vec3.hpp"

class color{
public:
    color();
    color(unsigned char r, unsigned char g, unsigned char b);
    color(float r, float g, float b);
    color(vec3 colour);
    float Red() const;
    float Green() const;
    float Blue() const;
    vec3f RGB() const;
    
	array3<unsigned char> ucharRep();
    //operators
    color operator+(const color& c);
    color operator+=(const color& c);
    color operator*(const float f);
    color operator-(const color& c);
    bool operator==(const color& c);
    bool operator!=(const color& c);
private:
    mutable vec3f rgb;
};

#endif /* color_hpp */
