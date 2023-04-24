#pragma once

#include <cmath>
#define PI 3.1415926

double toDegrees(float r) {
    return r * (180/PI);
}

float toRadians(double deg) {
    return deg * (PI/180);
}

struct Vector {
    double x;
    double y;
    
    Vector(double iX = 0, double iY = 0) {
        x = iX;
        y = iY;
    }

    double angle() {
        double ret = atan(y/x);
        if (x < 0) {
            ret += PI;
        }
        return ret;
    }

    double magnitude() {
        return sqrt(x * x + y * y);
    }

    Vector operator + (Vector const &vect) {
        return {vect.x + x, vect.y + y};
    }

    void operator += (Vector const &vect) {
        x += vect.x;
        y += vect.y;
    }

    void setDeadband(double dead) {
        assert(dead > 0);
        if (x < dead && x > -dead) {
            x = 0;
        }
        if (y < dead && y > -dead) {
            y = 0;
        }
    }

    void setAngAndMag(double ang, double mag) {
        x = cos(ang) * mag;
        y = sin(ang) * mag;
    }

    bool isZero() {
        return x == 0 && y == 0;
    }
};