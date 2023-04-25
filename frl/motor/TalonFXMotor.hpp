#pragma once

#include <ctre/Phoenix.h>
#include "BaseMotor.hpp"

class TalonFXMotor {
    TalonFX* talon;
public:
    TalonFXMotor(int id) : BaseMotor(id) {
        talon = new TalonFX {id};
        motorType = TALONFX;
    }

    ~TalonFXMotor() {
        delete talon;
    }

    void SetPercent(double perc) {
        if (dis) {
            return;
        }
        if (perc > hardMaxOutput) {
            Motor_printWarning("A TalonFXMotor with CANId " + std::to_string(CANId) + " went over hard \npercent output of " + std::to_string(hardMaxOutput) + "! Aborting now.");
            assert(perc > hardMaxOutput);
        }
        if (perc < hardMinOutput) {
            Motor_printWarning("A TalonFXMotor with CANId " + std::to_string(CANId) + " went under hard \npercent output of " + std::to_string(hardMaxOutput) + "! Aborting now.");
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
            talon -> SetInverted(!inv);
            perc *= -1;
        }
        else {
            talon -> SetInverted(inv);
        }
        talon -> Set(ControlMode::PercentOutput, perc);
    }

    void setP(double kP) {
        talon -> Config_kP(0, kP);
    }

    void setI(double kI) {
        talon -> Config_kI(0, kI);
    }

    void setD(double kD) {
        talon -> Config_kD(0, kD);
    }

    void setF(double kFF) {
        talon -> Config_kF(0, kFF);
    }

    void setPIDOutput(double min, double max, double nominalMin = 0, double nominalMax = 0) {
        talon -> ConfigPeakOutputForward(max);
        talon -> ConfigPeakOutputReverse(min);
        talon -> ConfigNominalOutputForward(nominalMax);
        talon -> ConfigNominalOutputReverse(nominalMin);
    }

    void setIZone(double kIZone) {
        talon -> Config_IntegralZone(0, kIZone);
    }

    double getPosition() {
        return talon -> GetSelectedSensorPosition();
    }

    double getVelocity() {
        return talon -> GetSelectedSensorVelocity();
    }

    void setPIDPosition(double pos) {
        if (dis) {
            return;
        }
        if (pos < 0) {
            talon -> SetInverted(!inv);
            pos *= -1;
        }
        else {
            talon -> SetInverted(inv);
        }
        talon -> Set(ControlMode::Position, pos);
    }

    void setPIDSpeed(double vel) {
        if (dis) {
            return;
        }
        talon -> Set(ControlMode::Velocity, vel);
    }

    void setRelativeEncoderPos(double pos) {
        talon -> SetSelectedSensorPosition(pos);
    }

    void setIdleMode(IdleMode mode) {
        if (mode == kBrake) {
            talon -> SetNeutralMode(NeutralMode::Brake);
        }
        else {
            talon -> SetNeutralMode(NeutralMode::Coast);
        }
    }

    double getTemp() {
        return talon -> GetTemperature();
    }
};