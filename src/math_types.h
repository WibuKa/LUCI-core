#pragma once

namespace Math_
{
    int nextPowerOfTwo(int v);
}

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float _x, float _y) : x(_x), y(_y) {}

    inline Vector2 operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
    inline Vector2 operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }

    inline Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    inline Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

    inline Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
    inline Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
    inline Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    inline Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }
};

struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3() = default;
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    inline Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    inline Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    inline Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    inline Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

    inline Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    inline Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    inline Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    inline Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }
};

struct Vector4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vector4() = default;
    Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    inline Vector4 operator+(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
    inline Vector4 operator-(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
    inline Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
    inline Vector4 operator/(float scalar) const { return Vector4(x / scalar, y / scalar, z / scalar, w / scalar); }

    inline Vector4& operator+=(const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    inline Vector4& operator-=(const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    inline Vector4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    inline Vector4& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }
};

struct Vector2i {
    int x = 0;
    int y = 0;

    // Constructors
    Vector2i() = default;
    Vector2i(int _x, int _y) : x(_x), y(_y) {}

    inline Vector2i operator+(const Vector2i& v) const { return Vector2i(x + v.x, y + v.y); }
    inline Vector2i operator-(const Vector2i& v) const { return Vector2i(x - v.x, y - v.y); }
    inline Vector2i operator*(int scalar) const { return Vector2i(x * scalar, y * scalar); }
    inline Vector2i operator/(int scalar) const { return Vector2i(x / scalar, y / scalar); }

    inline Vector2i& operator+=(const Vector2i& v) { x += v.x; y += v.y; return *this; }
    inline Vector2i& operator-=(const Vector2i& v) { x -= v.x; y -= v.y; return *this; }
    inline Vector2i& operator*=(int scalar) { x *= scalar; y *= scalar; return *this; }
    inline Vector2i& operator/=(int scalar) { x /= scalar; y /= scalar; return *this; }
};

struct Vector3i {
    int x = 0;
    int y = 0;
    int z = 0;

    // Constructors
    Vector3i() = default;
    Vector3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

    inline Vector3i operator+(const Vector3i& v) const { return Vector3i(x + v.x, y + v.y, z + v.z); }
    inline Vector3i operator-(const Vector3i& v) const { return Vector3i(x - v.x, y - v.y, z - v.z); }
    inline Vector3i operator*(int scalar) const { return Vector3i(x * scalar, y * scalar, z * scalar); }
    inline Vector3i operator/(int scalar) const { return Vector3i(x / scalar, y / scalar, z / scalar); }

    inline Vector3i& operator+=(const Vector3i& v) { x += v.x; y += v.y; z += v.z; return *this; }
    inline Vector3i& operator-=(const Vector3i& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    inline Vector3i& operator*=(int scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    inline Vector3i& operator/=(int scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }
};

struct Vector4i {
    int x = 0;
    int y = 0;
    int z = 0;
    int w = 0;

    // Constructors
    Vector4i() = default;
    Vector4i(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w) {}

    inline Vector4i operator+(const Vector4i& v) const { return Vector4i(x + v.x, y + v.y, z + v.z, w + v.w); }
    inline Vector4i operator-(const Vector4i& v) const { return Vector4i(x - v.x, y - v.y, z - v.z, w - v.w); }
    inline Vector4i operator*(int scalar) const { return Vector4i(x * scalar, y * scalar, z * scalar, w * scalar); }
    inline Vector4i operator/(int scalar) const { return Vector4i(x / scalar, y / scalar, z / scalar, w / scalar); }

    inline Vector4i& operator+=(const Vector4i& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    inline Vector4i& operator-=(const Vector4i& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    inline Vector4i& operator*=(int scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    inline Vector4i& operator/=(int scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }
};
