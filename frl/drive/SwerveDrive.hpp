/* Custom swerve drive */
// todo: document

#pragma once

#include <ctre/Phoenix.h>
#include <frl-reprogrammed/motor/PIDController.hpp>
#include <frl-reprogrammed/util/Vector.hpp>

struct SwerveConfig {
    BaseMotor* speed;
    BaseMotor* direction;
    short cancoderId;
    double cancoderOffset;
};

class SwerveModule {
    CANCoder* cancoder;
    BaseMotor* speed;
    PIDController<BaseMotor>* directionPID;

    SwerveModule* linkSwerve;
    short swerveRole;
    double cancoderOffset;
    float speedApply = 0;
    Vector apply;

    unsigned int GetDirection() {
        return smartLoop(cancoder -> GetAbsolutePosition() - cancoderOffset);
    }

public:
    SwerveModule(SwerveConfig config, short role) {
        cancoder = new CANCoder {config.cancoderId};
        speed = config.speed;
        directionPID = new PIDController<BaseMotor> {config.direction};
        swerveRole = role;
        cancoderOffset = config.cancoderOffset;

        speed -> setIdleMode(kBrake);
        directionPID -> SetCircumference(4096);
        directionPID -> constants.MinOutput = -.3;
        directionPID -> constants.MaxOutput = .3;
        directionPID -> constants.P = .012;
    }

    ~SwerveModule()  {
        delete cancoder;
        delete directionPID;
    }

    void setToVector(Vector trans, Vector rot = {0, 0}) {        // Not repsonsible for deadbands
        Vector app = trans;
        if (rot.isZero() && trans.isZero()) {
            return;
        }
        rot.setAngAndMag(smartLoop(PI/2 - rot.angle() * swerveRole, PI/2), rot.magnitude());
            //SetDirection(PI/2 - swerveRole * app.angle());
        app += rot;
        SetDirection(app.angle() * (PI/4096));
        SetSpeed(app.magnitude());

        if (!linkSwerve) {
            linkSwerve -> setToVector(trans, rot);
        }
    }

    void SetSpeed(float perc, bool followLink = true) {
        speedApply = perc;

        if (!linkSwerve && followLink) {            // If the pointer is not 0, thus it is linked
            linkSwerve -> SetSpeed(perc);
        }
    }

    void SetDirection(int pos, bool followLink = true) {
        /*if (std::abs(mostEfficient(GetDirection(), pos, 4096)) > 2048) {
            pos = smartLoop(pos - 2048);
            speed -> setInverted(true);
        }
        else {
            speed -> setInverted(false);
        }*/
        directionPID -> SetPosition(pos);
        if (!linkSwerve && followLink) {
            linkSwerve -> SetDirection(pos, followLink);
        }
    }

    void Apply() {

        speed -> SetPercent(speedApply);
        speedApply = 0;
        directionPID -> Update(GetDirection());
        apply = {0, 0};   // reset vector

        if (!linkSwerve) {
            linkSwerve -> Apply();
        }
    }

    void Link(SwerveModule* link) {
        linkSwerve = link;
    }

    void TranslationDrive(float x, float y, int heading) {
        Vector add {x, y};
        add.setAngAndMag(toRadians(smartLoop(toDegrees(apply.angle()) - 90 + heading, 360)), apply.magnitude());
        apply += add;
    }
};

SwerveModule GetSwerveDrive(SwerveConfig topleft, SwerveConfig topright, SwerveConfig bottomright, SwerveConfig bottomleft) {
    SwerveModule* tl = new SwerveModule {topleft, 1};
    SwerveModule* tr = new SwerveModule {topright, 2};
    SwerveModule* br = new SwerveModule {bottomright, 3};
    SwerveModule* ret = new SwerveModule {bottomleft, 4};
    ret -> Link(br);
    br -> Link(tr);
    tr -> Link(tl);
    return *ret;
}