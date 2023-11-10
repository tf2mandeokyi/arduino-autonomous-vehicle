#pragma once

#include <AFMotor.h>
#include <IRremote.h>

enum class IRButton : byte
{
    CH_MINUS, CH, CH_PLUS,
    PREV, NEXT, PLAY_PAUSE,
    MINUS, PLUS, EQ, PLUS100, PLUS200,
    BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9, BTN_0,
    NOTHING
};

enum class SideMarking : byte
{
    NOTHING, SLOW, FAST, CHOOSE_DIRECTION, STOP
};

enum class DrivingDirection : byte
{
    NONE, LEFT, RIGHT
};

enum class StoppingReason : byte
{
    NONE, MANUAL, PAUSE, AWAITING_DECISION
};

namespace DCMotorModule
{
    AF_DCMotor motorBL(1);
    AF_DCMotor motorBR(2);
    AF_DCMotor motorFR(3);
    AF_DCMotor motorFL(4);

    void setup();
    /**
     * @param speed 속도; 최소 -100, 최대 100
     **/
    void run(int speed, DrivingDirection direction);
};

namespace UltraSonicSensorModule
{
    const int triggerPin = 22;
    const int echoPin = 23;

    void setup();
    float measureCm();
};

namespace LineTrackerModule
{
    class TimedBinaryInputStacker
    {
    public:
        TimedBinaryInputStacker(int timeoutMs);
        ~TimedBinaryInputStacker();
        void insertInput(bool input);
        int readCount();
        bool isCountingMode();
    private:
        bool countingMode, prevInput;
        const int timeoutMs;
        int timeSinceLastInputMs, count;
    };

    const int sideLeftPin = 24;
    const int frontLeftPin = 26;
    const int frontPin = 28;
    const int frontRightPin = 30;
    const int sideRightPin = 32;
    TimedBinaryInputStacker leftStacker(500);
    TimedBinaryInputStacker rightStacker(500);

    void setup();
    byte readFront();
    SideMarking readSideMarking();
};

namespace IRreceiverModule
{
    IRrecv irrecv(33);
    decode_results results;
    IRButton lastReceived;

    void setup();
    void checkInput();
    IRButton read();
};