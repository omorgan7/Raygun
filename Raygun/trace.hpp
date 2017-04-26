#ifndef trace_hpp
#define trace_hpp

#include <vector>
#include "shape.hpp"
#include "raymath.hpp"
#include "color.hpp"
#include "world.hpp"
#include "photon.hpp"

bool ShadowRayIntersection(Mesh * mesh, Ray * shadowRay);
vec3f ForwardRayIntersection(Mesh * mesh, Ray * ray, size_t * out_tri_number);
Photon * PhotonIntersection(Mesh * mesh, Ray * ray, size_t depth);

#endif // !trace_hpp

