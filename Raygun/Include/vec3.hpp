#ifndef vec3_hpp
#define vec3_hpp
template <typename T>
struct vec3 {
	union {
		struct {
			T x, y, z;
		};
		T coords[3];
	};
};

typedef vec3<float> vec3f;
#endif // !vec3_hpp
