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

// L293D 쉴드 장착 모터 4개
DCMotorModule motors;
// 초음파 거리센서 모듈 (HC-SR04)
UltraSonicSensorModule ultraSonicSensor(22, 23);
// 라인트랙킹 센서 (줄여서 트랙커, tracker) 모듈 (KY-033) 5개
LineTrackerModule lineTracker(24, 26, 28, 30, 32);
// IR 리모컨 수신 센서 모듈 (Receiver Diode)
IRreceiverModule irReceiver(33);

// 최대 100
int drivingSpeed = 50;
DrivingDirection drivingDirection = DrivingDirection::NONE;
int startStoppingMs; // TODO: use this
StoppingReason stoppingReason = StoppingReason::NONE;
bool directionChangingMode = false;
DrivingDirection directionSelection = DrivingDirection::NONE;
bool directionSelected = false;
bool manualDrivingMode = false;

void reactIRButton(IRButton receivedButton);
void reactManualDrivingIRButton(IRButton receivedButton);
void reactSideMarking(SideMarking sideMarking, int timeMs);
void reactFrontPathFlag(byte frontPathFlag);

void setup()
{}

void loop()
{
    int timeMs = millis();

    // 입력
    irReceiver.checkInput();
    IRButton receivedButton = irReceiver.read();
    int distance = ultraSonicSensor.measureCm();
    byte frontPathFlag = lineTracker.readFront();
    SideMarking sideMarking = lineTracker.readSideMarking();

    // 계산: 마킹 신호 반응
    reactIRButton(receivedButton);
    reactSideMarking(sideMarking, timeMs);
    reactFrontPathFlag(frontPathFlag);

    // 출력
    motors.run(stoppingReason == StoppingReason::NONE ? drivingSpeed : 0, drivingDirection);
}

void reactIRButton(IRButton receivedButton)
{
    if(manualDrivingMode)
    {
        reactManualDrivingIRButton(receivedButton);
        return;
    }

    switch(receivedButton)
    {
        case IRButton::BTN_2:
            if(stoppingReason == StoppingReason::MANUAL)
            {
                stoppingReason = StoppingReason::NONE;
            }
            break;
            
        case IRButton::BTN_4:
            if(stoppingReason == StoppingReason::AWAITING_DECISION)
            {
                stoppingReason = StoppingReason::NONE;
                directionSelection = DrivingDirection::LEFT;
                directionSelected = true;
            }
            break;
            
        case IRButton::BTN_5:
            manualDrivingMode = true;
            break;
            
        case IRButton::BTN_6:
            if(stoppingReason == StoppingReason::AWAITING_DECISION)
            {
                stoppingReason = StoppingReason::NONE;
                directionSelection = DrivingDirection::RIGHT;
                directionSelected = true;
            }
            break;
            
        case IRButton::BTN_8:
            stoppingReason = StoppingReason::MANUAL;
            break;
    }
}

void reactManualDrivingIRButton(IRButton receivedButton)
{
    if(receivedButton == IRButton::BTN_5)
    {
        if(drivingSpeed == 0 && drivingDirection == DrivingDirection::NONE)
        {
            manualDrivingMode = false;
            return;
        }
        drivingSpeed = 0;
        drivingDirection = DrivingDirection::NONE;
    }
    switch(receivedButton)
    {
        case IRButton::BTN_1: case IRButton::BTN_2: case IRButton::BTN_3: drivingSpeed =  100; break;
        case IRButton::BTN_7: case IRButton::BTN_8: case IRButton::BTN_9: drivingSpeed = -100; break;
    }
    switch(receivedButton)
    {
        case IRButton::BTN_1: case IRButton::BTN_7: drivingDirection = DrivingDirection::LEFT;  break;
        case IRButton::BTN_2: case IRButton::BTN_8: drivingDirection = DrivingDirection::NONE;  break;
        case IRButton::BTN_3: case IRButton::BTN_9: drivingDirection = DrivingDirection::RIGHT; break;
    }
}

void reactSideMarking(SideMarking sideMarking, int timeMs)
{
    if(manualDrivingMode) return;

    switch(sideMarking)
    {
        case SideMarking::SLOW:
            if(!directionChangingMode) drivingSpeed = 50;
            break;
            
        case SideMarking::FAST:
            if(!directionChangingMode) drivingSpeed = 100;
            break;
            
        case SideMarking::CHOOSE_DIRECTION:
            if(!directionChangingMode)
            {
                stoppingReason = StoppingReason::AWAITING_DECISION;
                startStoppingMs = timeMs;
            }
            else if(directionChangingMode)
            {
                directionChangingMode = false;
                directionSelected = false;
            }
            break;
            
        case SideMarking::STOP:
            if(!directionChangingMode)
            {
                stoppingReason = StoppingReason::PAUSE;
                startStoppingMs = timeMs;
            }
            break;
    }
}

void reactFrontPathFlag(byte frontPathFlag)
{
    if(manualDrivingMode) return;

    switch(frontPathFlag)
    {
        case 0b000:
        case 0b010:
            drivingDirection = DrivingDirection::NONE;
            break;

        case 0b110:
            if(directionSelected) 
            {
                drivingDirection = directionSelection == DrivingDirection::LEFT ? DrivingDirection::LEFT : DrivingDirection::NONE;
                break;
            }
        case 0b100:
            drivingDirection = DrivingDirection::LEFT;
            break;

        case 0b011:
            if(directionSelected) 
            {
                drivingDirection = directionSelection == DrivingDirection::RIGHT ? DrivingDirection::RIGHT : DrivingDirection::NONE;
                break;
            }
        case 0b001:
            drivingDirection = DrivingDirection::RIGHT;
            break;

        case 0b111:
            drivingDirection = directionSelected ? directionSelection : DrivingDirection::NONE;
            break;
        case 0b101:
            if(directionSelected) 
            {
                drivingDirection = directionSelection == DrivingDirection::NONE ? DrivingDirection::LEFT : directionSelection;
                break;
            }
            drivingDirection = DrivingDirection::NONE;
            break;
    }
}
