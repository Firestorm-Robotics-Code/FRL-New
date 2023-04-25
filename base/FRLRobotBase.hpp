// An frc robot base (better name coming soon)

#include <frc/RobotBase.h>
#include <atomic>
#include <string>
#include <frc/Timer.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <frc/internal/DriverStationModeThread.h>
#include <frc/DriverStation.h>

template <typename T>
concept ModeType = requires(T t) {
    t.Init();
    t.Begin();
    t.Loop();
    t.End();
};

template <ModeType TeleopMode, ModeType AutoMode, ModeType TestMode, ModeType DisabledMode>
class FRLRobotBase : public frc::RobotBase {
public:
    std::atomic<bool> m_exit = false;

    TeleopMode* teleop = new TeleopMode;

    AutoMode* autonomous = new AutoMode;

    TestMode* test = new TestMode;

    DisabledMode* disabled = new DisabledMode;

    void* current = 0;

    frc::internal::DriverStationModeThread modeThread;

    void setMode(void* mode) {
        if (mode != current) {
            current -> End();
            mode -> Begin();
            current = mode;
        }
    }

    void* getActiveMode() {
        if (IsTeleop()) {
            return teleop;
        }
        if (IsAutonomous()) {
            return autonomous;
        }
        if (IsTest()) {
            return test;
        }
        if (IsDisabled()) {
            return disabled;
        }
    }

    void StartCompetition() {
        wpi::Event event {false, false};
        frc::DriverStation::ProvideRefreshedDataEventHandle(event.GetHandle());

        HAL_ObserveUserProgramStarting();

        teleop -> Init();
        autonomous -> Init();
        test -> Init();
        disabled -> Init();

        while (!m_exit) {
            modeThread.InTeleop(IsTeleop());
            modeThread.InAutonomous(IsAutonomous());
            modeThread.InTest(IsTest());
            modeThread.InDisabled(IsDisabled());

            setMode(getActiveMode());
        }

        current -> Loop();
    }

    void EndCompetition() {
        m_exit = true;
    }
};