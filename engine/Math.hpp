#pragma once

#include <iostream>
#include <math.h>

const float PI = 3.141592f;

/**
 * Convert degrees to radians.
 */
inline float Deg2Rad(float degrees)
{
    return degrees / 180.0f * PI;
}

/**
 * Convert Radians to degrees.
 */
inline float Rad2Deg(float radians)
{
    return radians / PI * 180.0f;
}

enum class EnumDirection {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z
};

template <class T>
struct matrix4 {
    T m00, m01, m02, m03;
    T m10, m11, m12, m13;
    T m20, m21, m22, m23;
    T m30, m31, m32, m33;

    matrix4() {
        m00 = 1;
        m11 = 1;
        m22 = 1;
        m33 = 1;

        m01 = 0;
        m02 = 0;
        m03 = 0;
        m10 = 0;
        m12 = 0;
        m13 = 0;
        m20 = 0;
        m21 = 0;
        m23 = 0;
        m30 = 0;
        m31 = 0;
        m32 = 0;
    }
};

template <class T>
struct vector2;

template <class T>
struct vector3;

template <class T>
struct vector4;

template <class T>
struct vector2 {
    T x;
    T y;

    vector2() {
        this->x = 0;
        this->y = 0;
    }

    vector2(T x, T y) {
        this->x = x;
        this->y = y;
    }

    vector2<T> operator-();
};

template <class T>
struct vector3 {
    T x;
    T y;
    T z;

    vector3(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    vector3(vector4<T> v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
    }

    vector3() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    vector3(T i) {
        this->x = i;
        this->y = i;
        this->z = i;
    }

    vector3<T> operator-();
};

template <class T>
struct vector4 {
    T x;
    T y;
    T z;
    T w;

    vector4(T x, T y, T z, T w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    vector4(vector3<T> v, T w) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = w;
    }

    vector4<T> operator-();
};


//typedef's because nobody likes typing vector3<float>
typedef vector2<float> vec2;
typedef vector3<float> vec3;
typedef vector4<float> vec4;
typedef vector2<int> vec2i;
typedef vector3<int> vec3i;
typedef vector4<int> vec4i;
typedef matrix4<float> mat4;

//TODO: use quaternions instead of vector for rotation.
struct Transform {
    Transform() {
        this->position = vec3();
        this->rotation = vec3();
        this->scale = vec3(1.0f, 1.0f, 1.0f);
    }

    Transform(vec3 position, vec3 rotation, vec3 scale) {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

    vec3 position;
    vec3 rotation;
    vec3 scale;
};

//--------IO operator--------//

template <typename T>
inline std::ostream& operator<<(std::ostream &lhs, vector2<T> rhs) {
    lhs << "(" << rhs.x << ", " << rhs.y << ")";

    return lhs;
}

template <typename T>
inline std::ostream& operator<<(std::ostream &lhs, vector3<T> rhs) {
    lhs << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";

    return lhs;
}

template <typename T>
inline std::ostream& operator<<(std::ostream &lhs, vector4<T> rhs) {
    lhs << "(" << rhs.x << ", " << rhs.y << rhs.z << ", " << rhs.w << ")";

    return lhs;
}




//--------addition--------//

//vec2
template <typename T>
inline vector2<T> operator+(vector2<T> lhs, vector2<T> rhs) {
    return vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template <typename T>
inline vector2<T> operator+(vector2<T> lhs, T rhs) {
    return vector2<T>(lhs.x + rhs, lhs.y + rhs);
}

template <typename T>
inline vector2<T> operator+(T lhs, vector2<T> rhs) {
    return vector2<T>(lhs + rhs.x, lhs + rhs.y);
}

//vec3
template <typename T>
inline vector3<T> operator+(vector3<T> lhs, vector3<T> rhs) {
    return vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template <typename T>
inline vector3<T> operator+(vector3<T> lhs, T rhs) {
    return vector3<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

template <typename T>
inline vector3<T> operator+(T lhs, vector3<T> rhs) {
    return vector3<T>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
}

//vec4
template <typename T>
inline vector4<T> operator+(vector4<T> lhs, vector4<T> rhs) {
    return vector4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

template <typename T>
inline vector4<T> operator+(vector4<T> lhs, T rhs) {
    return vector3<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
}

template <typename T>
inline vector4<T> operator+(T lhs, vector4<T> rhs) {
    return vector4<T>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
}




//--------subtraction--------//

//vec2
template <typename T>
inline vector2<T> operator-(vector2<T> lhs, vector2<T> rhs) {
    return vector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template <typename T>
inline vector2<T> operator-(vector2<T> lhs, T rhs) {
    return vector2<T>(lhs.x - rhs, lhs.y - rhs);
}

template <typename T>
inline vector2<T> operator-(T lhs, vector2<T> rhs) {
    return vector2<T>(lhs - rhs.x, lhs - rhs.y);
}

//vec3
template <typename T>
inline vector3<T> operator-(vector3<T> lhs, vector3<T> rhs) {
    return vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

template <typename T>
inline vector3<T> operator-(vector3<T> lhs, T rhs) {
    return vector3<T>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
}

template <typename T>
inline vector3<T> operator-(T lhs, vector3<T> rhs) {
    return vector3<T>(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
}

//vec4
template <typename T>
inline vector4<T> operator-(vector4<T> lhs, vector4<T> rhs) {
    return vector4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

template <typename T>
inline vector4<T> operator-(vector4<T> lhs, T rhs) {
    return vector3<T>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
}

template <typename T>
inline vector4<T> operator-(T lhs, vector4<T> rhs) {
    return vector4<T>(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
}




//--------multiplication--------//

//vec2
template <typename T>
inline vector2<T> operator*(vector2<T> lhs, vector2<T> rhs) {
    return vector2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

template <typename T>
inline vector2<T> operator*(vector2<T> lhs, T rhs) {
    return vector2<T>(lhs.x * rhs, lhs.y * rhs);
}

template <typename T>
inline vector2<T> operator*(T lhs, vector2<T> rhs) {
    return vector2<T>(lhs * rhs.x, lhs * rhs.y);
}

//vec3
template <typename T>
inline vector3<T> operator*(vector3<T> lhs, vector3<T> rhs) {
    return vector3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

template <typename T>
inline vector3<T> operator*(vector3<T> lhs, T rhs) {
    return vector3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

template <typename T>
inline vector3<T> operator*(T lhs, vector3<T> rhs) {
    return vector3<T>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

//vec4
template <typename T>
inline vector4<T> operator*(vector4<T> lhs, vector4<T> rhs) {
    return vector4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

template <typename T>
inline vector4<T> operator*(vector4<T> lhs, T rhs) {
    return vector3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

template <typename T>
inline vector4<T> operator*(T lhs, vector4<T> rhs) {
    return vector4<T>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
}




//--------division--------//

//vec2
template <typename T>
inline vector2<T> operator/(vector2<T> lhs, vector2<T> rhs) {
    return vector2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

template <typename T>
inline vector2<T> operator/(vector2<T> lhs, T rhs) {
    return vector2<T>(lhs.x / rhs, lhs.y / rhs);
}

template <typename T>
inline vector2<T> operator/(T lhs, vector2<T> rhs) {
    return vector2<T>(lhs / rhs.x, lhs / rhs.y);
}

//vec3
template <typename T>
inline vector3<T> operator/(vector3<T> lhs, vector3<T> rhs) {
    return vector3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

template <typename T>
inline vector3<T> operator/(vector3<T> lhs, T rhs) {
    return vector3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

template <typename T>
inline vector3<T> operator/(T lhs, vector3<T> rhs) {
    return vector3<T>(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

//vec4
template <typename T>
inline vector4<T> operator/(vector4<T> lhs, vector4<T> rhs) {
    return vector4<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

template <typename T>
inline vector4<T> operator/(vector4<T> lhs, T rhs) {
    return vector3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}

template <typename T>
inline vector4<T> operator/(T lhs, vector4<T> rhs) {
    return vector4<T>(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
}




//--------negate--------//

template <typename T>
inline vector2<T> vector2<T>::operator-()
{
    return vector2<T>(-x, -y);
}

template <typename T>
inline vector3<T> vector3<T>::operator-()
{
    return vector3<T>(-x, -y, -z);
}

template <typename T>
inline vector4<T> vector4<T>::operator-()
{
    return vector4<T>(-x, -y, -z, -w);
}




//-------mat4 operations--------//

template <typename T>
inline matrix4<T> operator*(matrix4<T> lhs, matrix4<T> rhs) {
    matrix4<T> result;

    result.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
    result.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
    result.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
    result.m30 = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30;

    result.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
    result.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
    result.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
    result.m31 = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31;

    result.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
    result.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
    result.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
    result.m32 = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32;

    result.m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;
    result.m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;
    result.m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;
    result.m33 = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33;

    return result;
}

template <typename T>
inline vector4<T> operator*(matrix4<T> lhs, vector4<T> rhs) {
    float x = lhs.m00 * rhs.x + lhs.m01 * rhs.y + lhs.m02 * rhs.z + lhs.m03 * rhs.w;
    float y = lhs.m10 * rhs.x + lhs.m11 * rhs.y + lhs.m12 * rhs.z + lhs.m13 * rhs.w;
    float z = lhs.m20 * rhs.x + lhs.m21 * rhs.y + lhs.m22 * rhs.z + lhs.m23 * rhs.w;
    float w = lhs.m30 * rhs.x + lhs.m31 * rhs.y + lhs.m32 * rhs.z + lhs.m33 * rhs.w;
    return vec4(x, y, z, w);
}

template <typename T>
inline vector4<T> operator*(vector4<T> lhs, matrix4<T> rhs) {
    return rhs * lhs;
}











//--------operators--------//

template <typename T>
inline T lengthSquared(vector3<T> vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

template <typename T>
inline T length(vector3<T> vec) {
    return sqrt(lengthSquared(vec));
}

template <typename T>
inline vector3<T> normalize(vector3<T> vec) {
    return vec / length(vec);
}


template <typename T>
inline vector3<T> cross(vector3<T> lhs, vector3<T> rhs) {
    float x = lhs.y * rhs.z - lhs.z * rhs.y;
    float y = lhs.z * rhs.x - lhs.x * rhs.z;
    float z = lhs.x * rhs.y - lhs.y * rhs.x;
    return vec3(x, y, z);
}

template <typename T>
inline T dot(vector3<T> lhs, vector3<T> rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}


template <typename T>
inline matrix4<T> translate(const matrix4<T> &mat, const vector3<T> &vec) {
    matrix4<T> temp;

    temp.m03 = vec.x;
    temp.m13 = vec.y;
    temp.m23 = vec.z;

    return mat * temp;
}

template <typename T>
inline matrix4<T> rotate(const matrix4<T> &mat, const vector3<T>& axis, const T& angle) {
    matrix4<T> result;

    const float c = cos(Deg2Rad(angle));
    const float s = sin(Deg2Rad(angle));
    const float t = 1.0f - c;

    result.m00 = t*axis.x*axis.x + c;
    result.m10 = t*axis.x*axis.y + s*axis.z;
    result.m20 = t*axis.x*axis.z - s*axis.y;

    result.m01 = t*axis.x*axis.y - s*axis.z;
    result.m11 = t*axis.y*axis.y + c;
    result.m21 = t*axis.y*axis.z + s*axis.x;

    result.m02 = t*axis.x*axis.z + s*axis.y;
    result.m12 = t*axis.y*axis.z - s*axis.x;
    result.m22 = t*axis.z*axis.z + c;

    return mat * result;
}

template <typename T>
inline matrix4<T> scale(const matrix4<T> &mat, vector3<T> scale) {
    matrix4<T> scaling;

    scaling.m00 = scale.x;
    scaling.m11 = scale.y;
    scaling.m22 = scale.z;

    return mat * scaling;
}

template <typename T>
inline matrix4<T> scale(const matrix4<T> &mat, T scale) {
    matrix4<T> scaling;

    scaling.m00 = scale;
    scaling.m11 = scale;
    scaling.m22 = scale;

    return mat * scaling;
}

template <typename T>
inline matrix4<T> transpose(matrix4<T> mat) {
    matrix4<T> temp;

    temp.m00 = mat.m00;
    temp.m10 = mat.m01;
    temp.m20 = mat.m02;
    temp.m30 = mat.m03;

    temp.m01 = mat.m10;
    temp.m11 = mat.m11;
    temp.m21 = mat.m12;
    temp.m31 = mat.m13;

    temp.m02 = mat.m20;
    temp.m12 = mat.m21;
    temp.m22 = mat.m22;
    temp.m32 = mat.m23;

    temp.m03 = mat.m30;
    temp.m13 = mat.m31;
    temp.m23 = mat.m32;
    temp.m33 = mat.m33;

    return temp;
}

inline mat4 perspective(float fov, float aspect, float near, float far) {
    mat4 result;

    float top    =  near * tan(Deg2Rad(fov) / 2.0);
    float bottom = -top;
    float right  =  top * aspect;
    float left   = -top * aspect;

    result.m00 = (2.0f*near) / (right - left);

    result.m11 = (2.0f*near) / (top - bottom);

    //result[2][0] = (right + left) / (right - left);
    //result[2][1] = (top + bottom) / (top - bottom);
    result.m22 = -(far + near) / (far - near); // NOTE(Joey): same as: (n + f) / (n - f)
    result.m32 = -1.0f;

    result.m23 = -(2.0f*near*far) / (far - near); // NOTE(Joey): same as 2nf / (n-f)
    result.m33 = 0.0f;


    return result;
}

inline mat4 orthographic(float left, float right, float bottom, float top, float near, float far) {
    mat4 ortho;

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(far + near) / (far - near);

    ortho.m00 = 2.0f / (right - left);
    ortho.m11 = 2.0f / (top - bottom);
    ortho.m22 = -2.0f / (far - near);
    ortho.m03 = tx;
    ortho.m13 = ty;
    ortho.m23 = tz;

    return ortho;
}

inline mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 forward = vec3(0, 0, -1);
    vec3 upVec = vec3(0, 1, 0);
    vec3 side = vec3(1, 0, 0);

    forward.x = center.x - eye.x;
    forward.y = center.y - eye.y;
    forward.z = center.z - eye.z;

    upVec.x = up.x;
    upVec.y = up.y;
    upVec.z = up.z;
    forward = normalize(forward);

    /* Side = forward x up */
    side = cross(forward, upVec);
    side = normalize(side);

    /* Recompute up as: up = side x forward */
    upVec = cross(side, forward);

    mat4 mat;
    mat.m00 = side.x;
    mat.m01 = side.y;
    mat.m02 = side.z;

    mat.m10 = upVec.x;
    mat.m11 = upVec.y;
    mat.m12 = upVec.z;

    mat.m20 = -forward.x;
    mat.m21 = -forward.y;
    mat.m22 = -forward.z;

    return translate(mat, vec3(-eye.x, -eye.y, -eye.z));
}
