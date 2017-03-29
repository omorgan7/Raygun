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
unsigned char color::Red(void) const {
    return red;
}
unsigned char color::Green(void) const {
    return green;
}
unsigned char color::Blue(void) const {
    return blue;
}

void color::changeRed(unsigned char R){
    red = R;
}
void color::changeGreen(unsigned char G){
    green = G;
}
void color::changeBlue(unsigned char B){
    blue = B;
}

color color::operator+(const color &c){
    unsigned char newRed, newGreen, newBlue;
    if(((int) this->Red() +  (int)c.Red()) > 255){
        newRed = 255;
    }
    else{
        newRed = this->Red() + c.Red();
    }
    if(((int) this->Green() + (int) c.Green())>255){
        newGreen = 255;
    }
    else{
        newGreen = this->Green() + c.Green();
    }
    if(((int) this->Blue() + (int) c.Blue())>255){
        newBlue = 255;
    }
    else{
        newBlue = this->Blue() + c.Blue();
    }
    return color(newRed,newGreen,newBlue);
}
color color::operator+=(const color &c){
    if(((int) this->Red() + (int) c.Red())>255){
        this->changeRed(255);
    }
    else{
        this->changeRed(this->Red() + c.Red());
    }
    if(((int) this->Green() + (int) c.Green())>255){
        this->changeGreen(255);
    }
    else{
        this->changeGreen(this->Green() + c.Green());
    }
    if(((int) this->Blue() + (int) c.Blue())>255){
        this->changeBlue(255);
    }
    else{
        this->changeBlue(this->Blue() + c.Blue());
    }
    return *this;
}
color& color::operator=(const color &c){
    if(this != &c){
        this->changeRed(c.Red());
        this->changeGreen(c.Green());
        this->changeBlue(c.Blue());
    }
    return *this;
}
color color::operator-(const color &c){
    unsigned char newRed, newGreen, newBlue;
    if(((int) this->Red() - (int) c.Red())<0){
        newRed = 0;
    }
    else{
        newRed = this->Red() - c.Red();
    }
    if(((int) this->Green() - (int) c.Green())<0){
        newGreen = 0;
    }
    else{
        newGreen = this->Green() - c.Green();
    }
    if(((int) this->Blue() - (int) c.Blue())<0){
        newBlue = 0;
    }
    else{
        newBlue = this->Blue() - c.Blue();
    }
    return color(newRed,newGreen,newBlue);
}
color color::operator*(const float f){
    unsigned char newRed, newGreen, newBlue;
    if((float) red*f > 255.0f){
        newRed = 255;
    }
    else if((float) red*f < 0.0f){
        newRed = 0;
    }
    else{
        newRed = (unsigned char) ((float) red)*f;
    }
    if((float) green*f > 255.0f){
        newGreen = 255;
    }
    else if((float) green*f < 0.0f){
        newGreen = 0;
    }
    else{
        newGreen = (unsigned char) ((float) green)*f;
    }
    if((float) blue*f > 255.0f){
        newBlue = 255;
    }
    else if((float) blue*f < 0.0f){
        newBlue = 0;
    }
    else{
        newBlue = (unsigned char) ((float) blue)*f;
    }
    return color(newRed,newGreen,newBlue);
}
bool color::operator==(const color& c){
    return (c.Red() == red)&&(c.Green() == green)&&(c.Blue() == blue);
}
bool color::operator!=(const color& c){
    return !((c.Red() == red)&&(c.Green() == green)&&(c.Blue() == blue));
}
