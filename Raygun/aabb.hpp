#ifndef AABB_HPP
#define AABB_HPP

#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include "raymath.hpp"
#include <assert.h>

struct Mesh_Stats{
    vec3f min;
    vec3f max;
};

struct AABB{
    //float corners[6];
    vec3f min;
    vec3f max;

    AABB * leftbox = nullptr;
    AABB * rightbox = nullptr;
    std::vector<unsigned int> vertex_indices;
    std::vector<unsigned int> triNumber;

    /*
    corners[0] holds x min, corners[1] holds x max, corners[2] holds y min etc. the box is completely defined by these 6 parameters.
    list of vertex indices contained within it.
    This struct follows min heap policy.
    leaf nodes terminated w/ nullptr. 
    */
};

int buildAABBTree(AABB * root,
                  std::vector<std::vector<float> > * vertices,
                  std::vector<unsigned int> *vertex_indices,
                  std::vector<std::vector<float> > * medians,
                  int depth);

void getminmaxmed(AABB * root, std::vector<std::vector<float> > * vertices, Mesh_Stats * stats);

bool AABBRayIntersection(AABB * root, Ray * R, std::vector<unsigned int> * intersectedVertices, int its, int readonly);

void cleanupAABBTree(AABB * root);

void translateAABBcorners(AABB * root,vec3f * translate);

void quicksort(int * A, int low, int high, int axis,std::vector<std::vector<float> > *vertices);

template <typename Ta,typename Tb>
Tb triangle_quicksort_partition(Ta * A, Tb low, Tb high);


#endif
