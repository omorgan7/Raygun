#ifndef trace_hpp
#define trace_hpp

#include <vector>
#include "shape.hpp"
#include "raymath.hpp"
#include "color.hpp"
#include "world.hpp"
#include "photon.hpp"

bool ShadowRayIntersection(Mesh * mesh, Ray * shadowRay);
float ForwardRayIntersection(Mesh * mesh, Ray * ray, size_t * out_tri_number);
Photon * PhotonIntersection(Mesh * mesh, Ray * ray, size_t depth);
vec3f ambientraytracer(Mesh * object, LightSurface * light, Ray * ray);
vec3f MC_SurfaceSample(Mesh * object, LightSurface * light, Ray * ray, size_t depth, size_t * outTri, float * t_param);
vec3f MC_LightSample(Mesh * object, LightSurface * light, Ray * ray, size_t depth, size_t * outTri, float * t_param);
vec3f MC_GlobalSample(Mesh * object, LightSurface * light, Ray * ray, size_t depth);
vec3f MC_specular_illumination(Mesh * object, LightSurface * light, Ray * ray,size_t depth, size_t * outTri, float * t_param);
#endif // !trace_hpp

