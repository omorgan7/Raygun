#ifndef photon_hpp
#define photon_hpp
struct Photon {
	vec3f pos;
	vec3f color;
    vec3f direction;
    int axes;
	float distance; //to sorting point.
};
#endif
