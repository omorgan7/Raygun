#ifndef vec3_hpp
#define vec3_hpp

#include <stdint.h>
#include <math.h>

#include <xmmintrin.h>
#include <smmintrin.h>

// Shuffle helpers.
// Examples: SHUFFLE3(v, 0,1,2) leaves the vector unchanged.
//           SHUFFLE3(v, 0,0,0) splats the X coord out.
#define VEC3_SHUFFLE(V, X,Y,Z) vec3(_mm_shuffle_ps((V).v, (V).v, _MM_SHUFFLE(Z,Z,Y,X)))

#define FORCE_INLINE __attribute__((always_inline)) inline

struct vec3 {
    __m128 v;
    vec3(){};
    
    FORCE_INLINE explicit vec3(const float* p) {
        v = _mm_set_ps(p[2], p[2], p[1], p[0]);
    }
    
    FORCE_INLINE explicit vec3(__m128 vec) {
        v = vec;
    }
    
    FORCE_INLINE explicit vec3(float x, float y, float z) {
        v = _mm_set_ps(z, z, y, x);
    }

    FORCE_INLINE float x() {
        return _mm_cvtss_f32(v);
    }
    
    FORCE_INLINE float y() {
        return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)));
    }
    
    FORCE_INLINE float z() {
        return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2)));
    }
    
    FORCE_INLINE void store(float* out) {
        out[0] = x();
        out[1] = y();
        out[2] = z();
    }
    
    FORCE_INLINE void setX(float x) {
        v = _mm_move_ss(v, _mm_set_ss(x));
    }
    
    FORCE_INLINE void setY(float y) {
        __m128 temp = _mm_move_ss(v, _mm_set_ss(y));
        temp = _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(3, 2, 0, 0));
        v = _mm_move_ss(temp, v);
    }
    
    FORCE_INLINE void setZ(float z) {
        __m128 temp = _mm_move_ss(v, _mm_set_ss(z));
        temp = _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(3, 0, 1, 0));
        v = _mm_move_ss(temp, v);
    }
    
    FORCE_INLINE vec3 xzy() {
        return VEC3_SHUFFLE(*this, 0, 2, 1);
    }
    
    FORCE_INLINE vec3 yxz() {
        return VEC3_SHUFFLE(*this, 1, 0, 2);
    }
    
    FORCE_INLINE vec3 yzx() {
        return VEC3_SHUFFLE(*this, 1, 2, 0);
    }
    
    FORCE_INLINE vec3 zyx() {
        return VEC3_SHUFFLE(*this, 2, 1, 0);
    }
    
    FORCE_INLINE vec3 zxy() {
        return VEC3_SHUFFLE(*this, 2, 0, 1);
    }
    
    static FORCE_INLINE vec3 max(vec3 a, vec3 b) {
        a.v = _mm_max_ps(a.v, b.v);
        return a;
    }
    
    static FORCE_INLINE vec3 min(vec3 a, vec3 b) {
        a.v = _mm_min_ps(a.v, b.v);
        return a;
    }
    
    FORCE_INLINE float minElement() {
        vec3 temp = min(*this, yxz());
        return min(temp, zxy()).x();
    }
    
    FORCE_INLINE float maxElement() {
        vec3 temp = max(*this, yxz());
        return max(temp, zxy()).x();
    }
};


FORCE_INLINE vec3 operator+ (vec3 a, vec3 b) {
    a.v = _mm_add_ps(a.v, b.v);
    return a;
}

FORCE_INLINE vec3 operator- (vec3 a, vec3 b) {
    a.v = _mm_sub_ps(a.v, b.v);
    return a;
}

FORCE_INLINE vec3 operator- (vec3 a) {
    return vec3(_mm_setzero_ps()) - a;
}

FORCE_INLINE vec3 operator* (vec3 a, vec3 b) {
    a.v = _mm_mul_ps(a.v, b.v);
    return a;
}

FORCE_INLINE vec3 operator* (vec3 a, float b) {
    a.v = _mm_mul_ps(a.v, _mm_set_ps1(b));
    return a;
}

FORCE_INLINE vec3 operator/ (vec3 a, vec3 b) {
    a.v = _mm_div_ps(a.v, b.v);
    return a;
}

FORCE_INLINE vec3 operator/ (vec3 a, float b) {
    a.v = _mm_div_ps(a.v, _mm_set_ps1(b));
    return a;
}

FORCE_INLINE vec3 operator/ (float a, vec3 b) {
    b.v = _mm_div_ps(_mm_set_ps1(a), b.v);
    return b;
}

FORCE_INLINE vec3& operator+= (vec3& a, vec3 b) {
    a = a + b;
    return a;
}

FORCE_INLINE vec3& operator-= (vec3& a, vec3 b) {
    a = a - b;
    return a;
}

FORCE_INLINE vec3& operator*= (vec3& a, vec3 b) {
    a = a * b;
    return a;
}

FORCE_INLINE vec3& operator*= (vec3& a, float b) {
    a = a * b;
    return a;
}

FORCE_INLINE vec3& operator/= (vec3& a, vec3 b) {
    a = a / b;
    return a;
}

FORCE_INLINE vec3& operator/= (vec3& a, float b) {
    a = a / b;
    return a;
}

FORCE_INLINE vec3 operator== (vec3 a, vec3 b) {
    a.v = _mm_cmpeq_ps(a.v, b.v);
    a.v = _mm_and_ps(a.v, _mm_set_epi32(0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000));
    return a;
}

FORCE_INLINE vec3 operator!= (vec3 a, vec3 b) {
    a.v = _mm_cmpneq_ps(a.v, b.v);
    a.v = _mm_and_ps(a.v, _mm_set_epi32(0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000));
    return a;
}

FORCE_INLINE vec3 operator< (vec3 a, vec3 b) {
    a.v = _mm_cmplt_ps(a.v, b.v);
    a.v = _mm_and_ps(a.v, _mm_set_epi32(0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000));
    return a;
}

FORCE_INLINE vec3 operator> (vec3 a, vec3 b) {
    a.v = _mm_cmpgt_ps(a.v, b.v);
    a.v = _mm_and_ps(a.v, _mm_set_epi32(0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000));
    return a;
}

FORCE_INLINE vec3 operator<= (vec3 a, vec3 b) {
    a.v = _mm_cmple_ps(a.v, b.v);
    a.v = _mm_and_ps(a.v, _mm_set_epi32(0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000));
    return a;
}

FORCE_INLINE vec3 operator>= (vec3 a, vec3 b) {
    a.v = _mm_cmpge_ps(a.v, b.v);
    a.v = _mm_and_ps(a.v, _mm_set_epi32(0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000));
    return a;
}

FORCE_INLINE vec3 cross(vec3 a, vec3 b) {
    return (a.zxy() * b - a * b.zxy()).zxy();
}

FORCE_INLINE float sum(vec3 a) {
    a.v = _mm_add_ps(_mm_add_ps(a.v, a.zyx().v), a.yxz().v);
    return a.x();
}

FORCE_INLINE float dot(vec3 a, vec3 b) {
    return sum(a * b);
}

FORCE_INLINE float lengthSquared(vec3 a) {
    return sum(a * a);
}

FORCE_INLINE float length(vec3 a) {
    return sqrt(lengthSquared(a));
}

FORCE_INLINE vec3 normalise(vec3 a) {
    return a / length(a);
}

FORCE_INLINE vec3 abs(vec3 a) {
    a.v = _mm_andnot_ps(_mm_set_ps(0x80000000, 0x80000000, 0x80000000, 0x80000000) , a.v);
    return a;
}

FORCE_INLINE vec3 sign(vec3 a) {
    return (a > vec3(0.0f, 0.0f, 0.0f)) - (a < vec3(0.0f, 0.0f, 0.0f));
}

FORCE_INLINE vec3 clamp(vec3 t, vec3 a, vec3 b) {
    return vec3::min(vec3::max(t, a), b);
}

typedef vec3 vec3f;
typedef vec3 vec3b;


#endif // !vec3_hpp
