//
//  color.hpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#ifndef color_hpp
#define color_hpp

#include <stdio.h>
class color{
public:
    color(unsigned char r, unsigned char g, unsigned char b);
    unsigned char Red(void);
    unsigned char Green(void);
    unsigned char Blue(void);
private:
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

#endif /* color_hpp */
