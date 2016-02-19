#pragma once

#include <cmath>
#include <algorithm>

struct Vec3 {
    float x, y, z;

    Vec3(float v = 0) : x(v), y(v), z(v) { }

    Vec3(float x, float y, float z) : x(x), y(y), z(z) { }

    void print() const {
        printf("%1.2f, %1.2f, %1.2f\n", x, y, z);
    }

    Vec3 &operator=(const Vec3 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }


    void operator+=(const Vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    void operator-=(const Vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    void operator*=(const Vec3 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
    }

    void operator/=(const Vec3 &other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
    }

    void operator+=(float v) {
        x += v;
        y += v;
        z += v;
    }

    void operator-=(float v) {
        x -= v;
        y -= v;
        z -= v;
    }

    void operator*=(float v) {
        x *= v;
        y *= v;
        z *= v;
    }

    void operator/=(float v) {
        x /= v;
        y /= v;
        z /= v;
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    float Dot(const Vec3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 Cross(const Vec3 &other) const {
        return Vec3(y * other.z - z * other.y,
                    z * other.x - x * other.z,
                    x * other.y - y * other.x);
    }

    float LengthSquared() const {
        return x * x + y * y + z * z;
    }

    float Length() const {
        return sqrt(LengthSquared());
    }

    Vec3 &Normalize() {
        *this /= Length();
        return *this;
    }


    Vec3 Reflect(const Vec3 &n) const {
        float d = 2 * Dot(n);
        return Vec3(x - n.x * d, y - n.y * d, z - n.z * d);
    }

    Vec3 &clamp() {
        x = std::max(x, 0.0f);
        y = std::max(y, 0.0f);
        z = std::max(z, 0.0f);
        return *this;
    }
};


static Vec3 operator+(Vec3 a, const Vec3 &b) {
    a += b;
    return a;
}


static Vec3 operator-(Vec3 a, const Vec3 &b) {
    a -= b;
    return a;
}


static Vec3 operator*(Vec3 a, const Vec3 &b) {
    a *= b;
    return a;
}


static Vec3 operator/(Vec3 a, const Vec3 &b) {
    a /= b;
    return a;
}


static Vec3 operator+(Vec3 a, float b) {
    a += b;
    return a;
}


static Vec3 operator-(Vec3 a, float b) {
    a -= b;
    return a;
}


static Vec3 operator*(Vec3 a, float b) {
    a *= b;
    return a;
}


static Vec3 operator/(Vec3 a, float b) {
    a /= b;
    return a;
}


static Vec3 Normalize(Vec3 v) {
    v.Normalize();
    return v;
}