/**
  @author Luke White
  @version 1.0
  NOTE: Still under development; has not yet been tested

  * Polymorphic base class for motors.

*/

#pragma once

#include <iostream>
#include <vector>
#include <frl-reprogrammed/util/Timer.hpp>

/**
  * An enum for idle modes
*/

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
    /**
     * A constructor that stores the Can ID
     @param canId The ID to store
    */
    BaseMotor(short canId) {
        CANId = canId;
    }
    /**
     * Whether or not the motor is inverted
    */
    bool inv = false;
    /**
     * Whether or not the motor is disabled
    */
    bool dis = false;
    /**
     * The 'hard' max output; This raises an error if the motor is told to output anything above the hard max output.
    */
    float hardMaxOutput = 2;
    /**
     * The 'hard' min output; This raises an error if the motor is told to output anything below the hard min output.
    */
    float hardMinOutput = -2;
    /**
     * The 'soft' max output; This prevents the motor from setting anything above the limit, but won't raise an error.
    */
    float softMaxOutput = 1;
    /**
     * The 'soft' min output; This prevents the motor from setting anything below the limit, but won't raise an error.
    */
    float softMinOutput = -1;
    /**
     * The limit for the motor's physical temperature; anything above and it raises an error.
    */
    int tempLimit = 90;
    /**
     * The CAN Id for the motor
    */
    short CANId;
    /**
     * The last applied percent
    */
    float appliedPercent = 0;
    /**
     * Sets the motor to run at a percent; limited by either hard or soft limit.
     * NOTE: Motors are never set to a negative percent; instead, the motor is set inverted and set to a positive one
     * @param perc The percent to set the motor to
    */
    virtual void SetPercent(double perc) = 0;
    /**
     * Sets the motor to an inverted state.
     * INTERNAL: The command to invert the actual motor is called under SetPercent(), setPIDPosition(), and setPIDSpeed().
     * @param state The state to invert it at
    */
    void setInverted(bool state) {
        inv = state;
    }
    /**
     * Set the 'P' term of the PID feedback loop
     * @param kP The P term to set
    */
    virtual void setP(double kP) = 0;
    /**
     * Set the 'I' term of the PID feedback loop
     * @param kI The I term to set
    */
    virtual void setI(double kI) = 0;
    /**
     * Set the 'D' term of the PID feedback loop
     * @param kD The D term to set
    */
    virtual void setD(double kD) = 0;
    /**
     * Set the 'F' term of the PID feedback loop
     * @param kFF The F term to set
    */
    virtual void setF(double kFF) = 0;
    /**
     * Set the 'I' zone of the PID feedback loop
     @param kIZone The I zone to set
    */
    virtual void setIZone(double kIZone) = 0;
    /**
     * Set the min and max outputs for the PID feedback loop
     * @param min The min speed to set during PID
     * @param max The max speed to set during PID
     * @param nominalMin The min voltage to set (not used in SparkMax)
     * @param nominalMax The max voltage to set (not used in SparkMax)
    */
    virtual void setPIDOutput(double min, double max, double nominalMin = 0, double nominalMax = 0) = 0;
    /**
     * Returns the position of the motor's relative encoder
    */
    virtual double getPosition() = 0;
    /**
     * Returns the velocity of the motor's relative encoder
    */
    virtual double getVelocity() = 0;
    /**
     * Sets the hard max percent output of the motor; {link hardMaxOutput}
     * @param perc The percent to hard limit it by
    */
    void configHardMaxPercentOutput(float perc) {
        hardMaxOutput = perc;
    }
    /**
     * Sets the hard min percent output of the motor; {link hardMinOutput}
     * @param perc The percent to hard limit it by
    */
    void configHardMinPercentOutput(float perc) {
        hardMinOutput = perc;
    }
    /**
     * Sets the soft max percent output of the motor; {link softMaxOutput}
     * @param perc The percent to soft limit it by
    */
    void configSoftMaxOutputPercent(float perc) {
        softMaxOutput = perc;
    }
    /**
     * Sets the soft min percent output of the motor; {link softMinOutput}
     * @param perc The percent to soft limit it by
    */
    void configSoftMinOutputPercent(float perc) {
        softMinOutput = perc;
    }
    /**
     * Returns whether the motor has reached a certain PID location
     * NOTE: not functioning
     * @param t The target to check for
    */
    bool atTarget(double t) {
        return false;   // fur now
    }
    /**
     * Sets the target for the PID position feedback loop
     * @param pos The position to set
    */
    virtual void setPIDPosition(double pos) = 0;
    /**
     * Sets the target for the PID velocity feedback loop
     * @param vel The speed to set
    */
    virtual void setPIDSpeed(double vel) = 0;
    /**
     * Sets the relative encoder's relative position
     * @param pos The position to set
    */
    virtual void setRelativeEncoderPos(double pos) = 0;
    /**
     * Sets the motor to an idle mode; {link IdleMode}
     * @param mode The mode to set
    */
    virtual void setIdleMode(IdleMode mode) = 0;
    /**
     * Returns the motor's current temperature
    */
    virtual double getTemp() = 0;
    /**
     * Sets the disabled state of the motor
     * @param state The state to set
    */
    void setDisabled(bool state) {
        dis = state;
    }
};

/*std::vector<BaseMotor*> motorWatchlist;
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
}*/