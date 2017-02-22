//
//  color.cpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "color.hpp"

color::color(){
    red = 0;
    green = 0;
    blue = 0;
}

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

color color::operator+(color &c){
    unsigned char newRed, newGreen, newBlue;
    if(((int) red + (int) c.Red())>255){
        newRed = 255;
    }
    else{
        newRed = red + c.Red();
    }
    if(((int) green + (int) c.Green())>255){
        newGreen = 255;
    }
    else{
        newGreen = green + c.Green();
    }
    if(((int) blue + (int) c.Blue())>255){
        newBlue = 255;
    }
    else{
        newBlue = blue + c.Blue();
    }
    return color(newRed,newGreen,newBlue);
}
color color::operator=(color &c){
    return color(c.Red(),c.Green(),c.Blue());
}
color color::operator-(color &c){
    unsigned char newRed, newGreen, newBlue;
    if(((int) red - (int) c.Red())<0){
        newRed = 0;
    }
    else{
        newRed = red - c.Red();
    }
    if(((int) green - (int) c.Green())<0){
        newGreen = 0;
    }
    else{
        newGreen = green - c.Green();
    }
    if(((int) blue - (int) c.Blue())<0){
        newBlue = 0;
    }
    else{
        newBlue = blue - c.Blue();
    }
    return color(newRed,newGreen,newBlue);
}
color color::operator*(float f){
    unsigned char newRed, newGreen, newBlue;
    if((float) red*f > 255){
        newRed = 255;
    }
    else if((float) red*f < 0){
        newRed = 0;
    }
    else{
        newRed = (unsigned char) ((float) red)*f;
    }
    if((float) green*f > 255){
        newGreen = 255;
    }
    else if((float) green*f < 0){
        newGreen = 0;
    }
    else{
        newGreen = (unsigned char) ((float) green)*f;
    }
    if((float) blue*f > 255){
        newBlue = 255;
    }
    else if((float) blue*f < 0){
        newBlue = 0;
    }
    else{
        newBlue = (unsigned char) ((float) blue)*f;
    }
    return color(newRed,newGreen,newBlue);
}
bool color::operator==(color& c){
    return (c.Red() == red)&&(c.Green() == green)&&(c.Blue() == blue);
}
bool color::operator!=(color& c){
    return !((c.Red() == red)&&(c.Green() == green)&&(c.Blue() == blue));
}
