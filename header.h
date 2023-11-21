#pragma once

#include <AFMotor.h>
#include <IRremote.h>
#include <NewPing.h>

enum class IRButton : unsigned long int
{
    NOTHING     = 0,

    CH_MINUS    = 0xBA45FF00,
    CH          = 0xB946FF00,
    CH_PLUS     = 0xB847FF00,
    
    PREV        = 0xB844FF00,
    NEXT        = 0xBF40FF00,
    PLAY_PAUSE  = 0xBC43FF00,
    
    MINUS       = 0xF807FF00,
    PLUS        = 0xEA15FF00,
    EQ          = 0xF609FF00,
    PLUS100     = 0xE619FF00,
    PLUS200     = 0xF20DFF00,

    BTN_1 = 0xF30CFF00, BTN_2 = 0xE718FF00, BTN_3 = 0xA15EFF00,
    BTN_4 = 0xF708FF00, BTN_5 = 0xE31CFF00, BTN_6 = 0xA55AFF00,
    BTN_7 = 0xBD42FF00, BTN_8 = 0xAD52FF00, BTN_9 = 0xB54AFF00,
    BTN_0 = 0xE916FF00
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
    const int maxDistanceCm = 20;
    NewPing sonar(triggerPin, echoPin, maxDistanceCm);

    void setup();
    float measureCm();
};

namespace LineTrackerModule
{
    class TimedBinaryInputStacker
    {
    public:
        TimedBinaryInputStacker(long timeoutMs);
        ~TimedBinaryInputStacker();
        void insertInput(bool input);
        int readCount();
        bool isCountingMode();
    private:
        bool countingMode, prevInput;
        const long timeoutMs;
        int count;
        long timeSinceLastInputMs;
    };

    const int sideLeftPin = 24;
    const int frontLeftPin = 26;
    const int frontPin = 28;
    const int frontRightPin = 30;
    const int sideRightPin = 32;
    TimedBinaryInputStacker leftStacker(1000);
    TimedBinaryInputStacker rightStacker(1000);

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