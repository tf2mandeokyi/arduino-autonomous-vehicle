#pragma once

#include <AFMotor.h>
#include <IRremote.h>

enum class IRButton : long int
{
    NOTHING     = 0,

    CH_MINUS    = 0xFFA25D,
    CH          = 0xFF629D,
    CH_PLUS     = 0xFFE21D,
    
    PREV        = 0xFF22DD,
    NEXT        = 0xFF02FD,
    PLAY_PAUSE  = 0xFFC23D,
    
    MINUS       = 0xFFE01F,
    PLUS        = 0xFFA857,
    EQ          = 0xFF906F,
    PLUS100     = 0xFF9867,
    PLUS200     = 0xFFB04F,

    BTN_1 = 0xFF30CF, BTN_2 = 0xFF18E7, BTN_3 = 0xFF7A85,
    BTN_4 = 0xFF10EF, BTN_5 = 0xFF38C7, BTN_6 = 0xFF5AA5,
    BTN_7 = 0xFF42BD, BTN_8 = 0xFF4AB5, BTN_9 = 0xFF52AD,
    BTN_0 = 0xFF6897
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