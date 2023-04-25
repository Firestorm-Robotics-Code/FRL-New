#pragma once

#include <frl-reprogrammed/util/Vector.hpp>

class MecanumDrive {
    BaseMotor* frontLeft;
    BaseMotor* frontRight;
    BaseMotor* backLeft;
    BaseMotor* backRight;

    float frontleftApply = 0;
    float frontrightApply = 0;
    float backleftApply = 0;
    float backrightApply = 0;
public:
    MecanumDrive(BaseMotor* fl, BaseMotor* fr, BaseMotor* bl, BaseMotor* br, bool flInv = false, bool frInv = false, bool blInv = false, bool brInv = false) {
        frontLeft = fl;
        frontRight = fr;
        backLeft = bl;
        backRight = br;
        frontLeft -> setInverted(flInv);
        frontRight -> setInverted(frInv);
        backLeft -> setInverted(blInv);
        backRight -> setInverted(brInv);
    }

    void TranslationDrive(float x, float y, int heading = 0) {
        Vector apply {x, y};
        apply.setAngAndMag(apply.angle() + heading, apply.magnitude())
        if (y > 0) {
            frontrightApply -= y;
            backrightApply += y;
        }
        else {
            frontleftApply -= y;
            backleftApply += y;
        }
        frontrightApply += x;
        frontleftApply += x;
        backrightApply += x;
        backleftApply += x;
    }

    void RotationDrive()
}