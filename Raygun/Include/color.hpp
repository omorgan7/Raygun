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

#include "vec3.hpp"

class color{
public:
    color();
    color(unsigned char r, unsigned char g, unsigned char b);
    color(float r, float g, float b);
    unsigned char Red(void) const;
    unsigned char Green(void) const;
    unsigned char Blue(void) const;
    void changeRed(unsigned char R);
    void changeGreen(unsigned char G);
    void changeBlue(unsigned char B);
	vec3f floatingPointRep(void);
    //operators
    color operator+(const color& c);
    color operator+=(const color& c);
    color operator*(const float f);
    color operator-(const color& c);
    color& operator=(const color &c);
    bool operator==(const color& c);
    bool operator!=(const color& c);
private:
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

#endif /* color_hpp */
