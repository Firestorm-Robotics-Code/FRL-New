/* Custom pid closed-loop */
// todo: document

#pragma once

double smartLoop(double pos, double round = 4096){
    while (pos > round){
        pos -= round;
    }
    while (pos < 0){
        pos += round;
    }
    return pos;
}

double mostEfficient(double cur, double goal, double circ) {        // Finds the most efficient path around a circle
    if (std::abs(goal - cur) > (circ/2)) {
        return (goal - circ - cur);
    }
    return goal - cur;
}

struct PIDConstants {
    double P = 0;
    double I = 0;
    double D = 0;
    double F = 0;
    double IZone = 0;
    float MinOutput = -1;
    float MaxOutput = 1;
};

template <typename T>
concept PIDType = requires(T t, double perc) {
    t.SetPercent(perc);
};

template <PIDType control>
class PIDController {
    control* motor;
    double prevError = 0;
    double circumference = -1;
    double currentError = 0;
    double setPosition = 0;

    bool withinIZone(double error) {
        return error < constants.IZone && error > -constants.IZone;
    }

    double getPositionApply(double error) {
        currentError = error;
        double ret = error * constants.P;
        if (withinIZone(error) && error != 0) {
            ret += constants.I * error;
        }
        ret -= constants.D * (error - prevError);
        ret += constants.F;
        prevError = error;
        if (ret > constants.MaxOutput) {
            return constants.MaxOutput;
        }
        else if (ret < constants.MinOutput) {
            return constants.MinOutput;
        }
        return ret;
    }
public:
    PIDConstants constants;

    PIDController(control* cont) {
        motor = cont;
    }

    void SetCircumference(double cir) {
        circumference = cir;
    }

    void SetPosition(double pos) {
        setPosition = pos;
    }

    void Update() {
        Update(motor -> getPosition());
    }

    void Update(double curr) {
        double ret;
        if (circumference != -1) {
            ret = getPositionApply(mostEfficient(curr, setPosition, circumference));
            currentError = mostEfficient(curr, setPosition, circumference);
        }
        else {
            ret = getPositionApply(setPosition - curr);
            currentError = setPosition - curr;
        }
        motor -> SetPercent(ret);
    }

    double getError() {
        return currentError;
    }
};