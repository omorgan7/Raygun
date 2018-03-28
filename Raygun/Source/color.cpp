//
//  color.cpp
//  Raygun
//
//  Created by Owen Morgan on 16/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "color.hpp"

inline float colourClamp(unsigned char c) {
    return std::min(std::max(0.0f, float(c) / 255.0f), 1.0f);
}

color::color(){
    rgb = vec3(0.0f, 0.0f, 0.0f);
}

color::color(vec3 colour) {
    rgb = colour;
}

color::color(unsigned char r, unsigned char g, unsigned char b){
    rgb = vec3(colourClamp(r), colourClamp(g), colourClamp(b));
}
color::color(float r, float g, float b){
    //clamp
    rgb = vec3(r, g, b);
}
float color::Red() const {
    return rgb.x();
}
float color::Green() const {
    return rgb.y();
}
float color::Blue() const {
    return rgb.z();
}

vec3f color::RGB() const {
    return rgb;
}

array3<unsigned char> color::ucharRep(){
    vec3f temp = rgb * vec3(255.0f, 255.0f, 255.0f);
    return {(unsigned char) temp.x(), (unsigned char) temp.y(), (unsigned char) temp.z()};
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
    rgb += c.rgb;
    rgb = clamp(rgb, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
    return *this;
}

color color::operator-(const color &c){
    vec3 temp = rgb - c.rgb;
    temp = clamp(temp, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
    return temp;
}
color color::operator*(const float f){
    vec3 temp = rgb * f;
    temp = clamp(temp, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
    return temp;
}
bool color::operator==(const color& c){
    return rgb.x() == c.rgb.x() && rgb.y() == c.rgb.y() && rgb.z() == c.rgb.z();
}
bool color::operator!=(const color& c){
    return !(rgb.x() == c.rgb.x() && rgb.y() == c.rgb.y() && rgb.z() == c.rgb.z());
}
