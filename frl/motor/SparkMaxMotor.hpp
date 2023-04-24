/* Spark motor class */
// TODO: document stuff

#pragma once
#include <rev/CANSparkMax.h>     // Needs revlib vendordep
#include "BaseMotor.hpp"

struct _sparkMaxControls {                // Tyler, you left me no choice
    rev::SparkMaxRelativeEncoder encoder;
    rev::SparkMaxPIDController pid;
};

class SparkMaxMotor : public BaseMotor {
    rev::CANSparkMax* spark;
    _sparkMaxControls* controls;

public:
    SparkMaxMotor(int id) : BaseMotor(id) {
        spark = new rev::CANSparkMax {id, rev::CANSparkMax::MotorType::kBrushless};
        controls = new _sparkMaxControls{spark -> GetEncoder(), spark -> GetPIDController()};
        Motor_addToWatchlist(this);
        motorType = SPARK;
    }
    ~SparkMaxMotor() {
        delete spark;
        delete controls;
        Motor_removeFromWatchlist(this);
    }

    void SetPercent(double perc) {
        if (dis) {
            return;
        }
        if (perc > hardMaxOutput) {
            Motor_printWarning("A SparkMaxMotor with CANId " + std::to_string(CANId) + " went over hard \npercent output of " + std::to_string(hardMaxOutput) + "! Aborting now.");
            assert(perc > hardMaxOutput);
        }
        if (perc < hardMinOutput) {
            Motor_printWarning("A SparkMaxMotor with CANId " + std::to_string(CANId) + " went under hard \npercent output of " + std::to_string(hardMaxOutput) + "! Aborting now.");
            assert(perc < hardMinOutput);
        }
        if (perc > softMaxOutput) {
            perc = softMaxOutput;
        }
        else if (perc < softMinOutput) {
            perc = softMinOutput;
        }
        appliedPercent = perc;
        if (perc < 0) {
            spark -> SetInverted(!inv);
            perc *= -1;
        }
        else {
            spark -> SetInverted(inv);
        }
        spark -> Set(perc);
    }

    void setP(double kP) {
        controls -> pid.SetP(kP);
    }

    void setI(double kI) {
        controls -> pid.SetI(kI);
    }

    void setD(double kD) {
        controls -> pid.SetD(kD);
    }

    void setF(double kFF) {
        controls -> pid.SetFF(kFF);
    }

    void setIZone(double kIZone) {
        controls -> pid.SetIZone(kIZone);
    }

    void setPIDOutput(double min, double max, double nominalMin = 0, double nominalMax = 0) {
        controls -> pid.SetOutputRange(min, max);
    }

    double getPosition() {
        return controls -> encoder.GetPosition();
    }

    double getVelocity() {
        return controls -> encoder.GetVelocity();
    }

    void setPIDPosition(double pos) {
        if (dis) {
            return;
        }
        if (pos < 0) {
            spark -> SetInverted(!inv);
            pos *= -1;
        }
        else {
            spark -> SetInverted(inv);
        }
        controls -> pid.SetReference(pos, rev::CANSparkMax::ControlType::kPosition);
    }

    void setPIDSpeed(double vel) {
        if (dis) {
            return;
        }
        controls -> pid.SetReference(vel, rev::CANSparkMax::ControlType::kVelocity);
    }

    void setRelativeEncoderPos(double pos) {
        controls -> encoder.SetPosition(pos);
    }

    void setIdleMode(IdleMode mode) {
        if (dis) {
            return;
        }
        if (mode == kBrake) {
            spark -> SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        }
        else {
            spark -> SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
        }
    }

    double getTemp() {
        return spark -> GetMotorTemperature();
    }
};