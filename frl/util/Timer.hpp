#pragma once

#define REFRESH_RATE 1000

class Timer {
    unsigned long long current;
    double goal;
public:
    void setGoal(double g) {
        goal = g * REFRESH_RATE;
    }
    void reset() {
        current = 0;
    }
    void elapse() {
        current ++;
    }
    operator bool() {
        return hasExpired();
    }
    bool hasExpired() {
        return current > goal;
    }
    bool isActive() {
        return current == 0;
    }
};