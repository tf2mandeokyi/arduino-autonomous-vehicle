#pragma once

#include <AFMotor.h>
#include <IRremote.h>

enum class IRButton : byte
{
    BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9,
    NOTHING
};

enum class SideMarking : byte
{
    SLOW, FAST, CHOOSE_DIRECTION, STOP, NOTHING
};

enum class DrivingDirection : byte
{
    LEFT, NONE, RIGHT
};

enum class StoppingReason : byte
{
    NONE, MANUAL, PAUSE, AWAITING_DECISION
};

class DCMotorModule
{
public:
    DCMotorModule();
    ~DCMotorModule();
    /**
     * @param speed 속도; 최소 -100, 최대 100
     **/
    void run(int speed, DrivingDirection direction);
private:
    // F: 앞, B: 뒤, L: 왼쪽, R: 오른쪽
    AF_DCMotor motorBL, motorBR, motorFR, motorFL;
};

class UltraSonicSensorModule
{
public:
    UltraSonicSensorModule(int triggerPin, int echoPin);
    ~UltraSonicSensorModule();
    float measureCm();
private:
    const int triggerPin, echoPin;
};

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

class LineTrackerModule
{
public:
    LineTrackerModule(int sideLeftPin, int frontLeftPin, int frontPin, int frontRightPin, int sideRightPin);
    ~LineTrackerModule();
    byte readFront();
    SideMarking readSideMarking();
private:
    const int sideLeftPin, frontLeftPin, frontPin, frontRightPin, sideRightPin;
    TimedBinaryInputStacker leftStacker, rightStacker;
};

class IRreceiverModule
{
public:
    IRreceiverModule(int pin);
    ~IRreceiverModule();
    void checkInput();
    IRButton read();
private:
    IRrecv irrecv;
    decode_results results;
    IRButton lastReceived;
};