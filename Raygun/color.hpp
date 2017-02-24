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
#include <iostream>

class color{
public:
    color();
    color(unsigned char r, unsigned char g, unsigned char b);
    unsigned char Red(void);
    unsigned char Green(void);
    unsigned char Blue(void);
    void changeRed(unsigned char R);
    void changeGreen(unsigned char G);
    void changeBlue(unsigned char B);
    //operators
    color operator+(color& c);
    color operator*(float f);
    color operator-(color& c);
    color operator=(color &c);
    bool operator==(color& c);
    bool operator!=(color& c);
private:
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

#endif /* color_hpp */
