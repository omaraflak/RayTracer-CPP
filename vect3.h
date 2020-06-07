#ifndef VECT3_H
#define VECT3_H

#include <iostream>
#include <sstream>
#include <cmath>

class Vect3 {
public:
    float x, y, z;

    Vect3(const float& x, const float& y, const float& z);
    Vect3(const Vect3& v);
    Vect3();

    void set(const Vect3& v);
    void set(const float& x, const float& y, const float& z);

    float dot(const Vect3& v) const;
    Vect3 cross(const Vect3& v) const;
    Vect3 elementWiseProduct(const Vect3& v) const;
    Vect3 normalized() const;
    Vect3 reflected(const Vect3& axis) const;
    float length() const;

    Vect3 plus(const Vect3& v) const;
    Vect3 minus(const Vect3& v) const;
    Vect3 times(const float& f) const;
    std::string toString() const;
};

std::ostream& operator<<(std::ostream& os, const Vect3& v);
Vect3 operator+(const Vect3& a, const Vect3& b);
Vect3 operator-(const Vect3& a, const Vect3& b);
Vect3 operator*(const Vect3& a, const Vect3& b);
Vect3 operator*(const float& f, const Vect3& v);

#endif
