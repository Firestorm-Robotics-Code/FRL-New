#pragma once

#ifndef PI
#define PI 3.1415926
#endif

double toDegrees(float r) {
    return r * (180/PI);
}

float toRadians(double deg) {
    return deg * (PI/180);
}