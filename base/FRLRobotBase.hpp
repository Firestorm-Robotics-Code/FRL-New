// An frc robot base (better name coming soon)

#include <frc/RobotBase.h>
#include <pthread.h>
#include <atomic>
#include <string>
#include <frc/Timer.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <frc/internal/DriverStationModeThread.h>
#include <frc/DriverStation.h>

struct RobotMode {

    virtual void Init() {

    }
    virtual void Begin() {

    }
    virtual void Loop() {

    }
    virtual void Thread() {

    }
    virtual void End() {

    }
};

template <typename T>
concept ModeType = std::is_base_of<RobotMode, T>::value;

template <ModeType TeleopMode, ModeType AutoMode, ModeType TestMode, ModeType DisabledMode>
class FRLRobotBase : public frc::RobotBase {
public:
    std::atomic<bool> m_exit = false;

    TeleopMode* teleop = new TeleopMode;// = new TeleopMode;

    AutoMode* autonomous = new AutoMode;

    TestMode* test = new TestMode;

    DisabledMode* disabled = new DisabledMode;

    RobotMode* current = 0;

    pthread_t currentThread;

    frc::internal::DriverStationModeThread modeThread;

    static void* thread_function(void* r) {               // I read Tyler's code + looked on internet, and this seems to be my only option
        FRLRobotBase* robot = (FRLRobotBase*)r;
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        if (robot -> current) {    // if current isn't 0
            robot -> current -> Thread();
        }
        
        return NULL;
    }

    void setMode(RobotMode* mode) {
        if (mode != current) {
            if (current) {                   // if current pointer is set to something
                current -> End();
                pthread_cancel(currentThread);
            }
            mode -> Begin();
            current = mode;
            pthread_create(&currentThread, NULL, &FRLRobotBase::thread_function, this);
        }
    }

    RobotMode* getActiveMode() {
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
        return 0;
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

            current -> Loop();
        }
    }

    void EndCompetition() {
        m_exit = true;
    }
};
