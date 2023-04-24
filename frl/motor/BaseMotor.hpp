/* A base class for the polymorphic motor wrapper */

#pragma once

#include <iostream>
#include <vector>
#include <frl-reprogrammed/util/Timer.hpp>

enum IdleMode {
    kBrake,
    kCoast
};

enum MotorClass {
    SPARK,
    TALONFX,
    TALONSRX,
    VICTORSPX
};

struct BaseMotor {
    MotorClass motorType;

    BaseMotor(short canId) {
        CANId = canId;
    }

    bool inv = false;
    bool dis = false;
    float hardMaxOutput = 2;
    float hardMinOutput = -2;
    float softMinOutput = -1;
    float softMaxOutput = 1;
    int tempLimit = 90;
    short CANId;
    float appliedPercent = 0;

    virtual void SetPercent(double perc) = 0;
    void setInverted(bool state) {
        inv = state;
    }
    virtual void setP(double kP) = 0;
    virtual void setI(double kI) = 0;
    virtual void setD(double kD) = 0;
    virtual void setF(double kFF) = 0;
    virtual void setIZone(double kIZone) = 0;
    virtual void setPIDOutput(double min, double max, double nominalMin = 0, double nominalMax = 0) = 0;
    virtual double getPosition() = 0;
    virtual double getVelocity() = 0;

    void configHardMaxPercentOutput(float perc) {
        hardMaxOutput = perc;
    }

    void configHardMinPercentOutput(float perc) {
        hardMinOutput = perc;
    }

    void configSoftMaxOutputPercent(float perc) {
        softMaxOutput = perc;
    }

    void configSoftMinOutputPercent(float perc) {
        softMinOutput = perc;
    }

    bool atTarget(double t) {
        return false;   // fur now
    }

    virtual void setPIDPosition(double pos) = 0;
    virtual void setPIDSpeed(double vel) = 0;
    virtual void setRelativeEncoderPos(double pos) = 0;
    virtual void setIdleMode(IdleMode mode) = 0;
    virtual double getTemp() = 0;
    void setDisabled(bool state) {
        dis = state;
    }
};

std::vector<BaseMotor*> motorWatchlist;
std::vector<Timer*> timeouts;

void Motor_addToWatchlist(BaseMotor* mot) {
    motorWatchlist.push_back(mot);
}

void Motor_removeFromWatchlist(BaseMotor* mot) {
    for (unsigned int x = 0; x < motorWatchlist.size(); x ++) {
        if (motorWatchlist[x] == mot) {
            std::vector<BaseMotor*>::iterator iterator = motorWatchlist.begin() + x;
            motorWatchlist.erase(iterator);
        }
    }
}

std::string Motor_getStringFromMotorType(MotorClass mot) {
    if (mot == SPARK) {
        return "SparkMaxMotor";
    }
    if (mot == TALONFX) {
        return "TalonFXMotor";
    }
    if (mot == TALONSRX) {
        return "TalonSRXMotor";
    }
    if (mot == VICTORSPX) {
        return "VictorSPXMotor";
    }
    return "";
}

void Motor_printWarning(std::string war) {
    std::cout << "**************************   MOTOR WARNING   **************************" << std::endl;
    std::cout << war << std::endl;
    std::cout << "***********************************************************************" << std::endl;
}

void Motor_watchlistUpdate() {
    Timer tempTimer;
    Timer percentOutTimer;
    timeouts.push_back(&tempTimer);
    timeouts.push_back(&percentOutTimer);
    tempTimer.setGoal(30);
    std::string warningStr = "";
    while (true) {
        for (unsigned int x = 0; x < motorWatchlist.size(); x ++) {
            timeouts[0] -> elapse();
            if ((motorWatchlist[x] -> getTemp() > motorWatchlist[x] -> tempLimit) && (timeouts[0])) {
                warningStr = "A " + Motor_getStringFromMotorType(motorWatchlist[x] -> motorType) + "with CanID " + std::to_string(motorWatchlist[x] -> CANId);
                warningStr += "is at temperature " + std::to_string(motorWatchlist[x] -> getTemp()) + "⁰C !\n";
                warningStr += "Aborting due to temperature being greater than " + std::to_string(motorWatchlist[x] -> tempLimit);
                Motor_printWarning(warningStr);
                timeouts[0] -> reset();
            }
            
        }
    } 
}