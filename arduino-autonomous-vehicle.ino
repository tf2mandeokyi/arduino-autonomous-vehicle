#include "header.h"

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
int startStoppingMs;
StoppingReason stoppingReason = StoppingReason::NONE;
bool directionChangingMode = false;
DrivingDirection directionSelection = DrivingDirection::NONE;
bool manualDrivingMode = false;

void handleIRButton(IRButton receivedButton);
void handleManualDrivingIRButton(IRButton receivedButton);
void handleSideMarking(SideMarking sideMarking, int timeMs);
void handleFrontPathFlag(byte frontPathFlag);
void handleStoppingExpire();

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
    handleIRButton(receivedButton);
    handleSideMarking(sideMarking, timeMs);
    handleFrontPathFlag(frontPathFlag);
    handleStoppingExpire();

    // 출력
    motors.run(stoppingReason == StoppingReason::NONE ? drivingSpeed : 0, drivingDirection);
}

void handleIRButton(IRButton receivedButton)
{
    if(manualDrivingMode)
    {
        handleManualDrivingIRButton(receivedButton);
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
            }
            break;
            
        case IRButton::BTN_8:
            stoppingReason = StoppingReason::MANUAL;
            break;
    }
}

void handleManualDrivingIRButton(IRButton receivedButton)
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

void handleSideMarking(SideMarking sideMarking, int timeMs)
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

void handleFrontPathFlag(byte frontPathFlag)
{
    if(manualDrivingMode) return;

    switch(frontPathFlag)
    {
        case 0b000:
        case 0b010:
            drivingDirection = DrivingDirection::NONE;
            break;

        case 0b110:
            if(directionChangingMode) 
            {
                drivingDirection = directionSelection == DrivingDirection::LEFT ? DrivingDirection::LEFT : DrivingDirection::NONE;
                break;
            }
        case 0b100:
            drivingDirection = DrivingDirection::LEFT;
            break;

        case 0b011:
            if(directionChangingMode) 
            {
                drivingDirection = directionSelection == DrivingDirection::RIGHT ? DrivingDirection::RIGHT : DrivingDirection::NONE;
                break;
            }
        case 0b001:
            drivingDirection = DrivingDirection::RIGHT;
            break;

        case 0b111:
            drivingDirection = directionChangingMode ? directionSelection : DrivingDirection::NONE;
            break;
        case 0b101:
            if(directionChangingMode) 
            {
                drivingDirection = directionSelection == DrivingDirection::NONE ? DrivingDirection::LEFT : directionSelection;
                break;
            }
            drivingDirection = DrivingDirection::NONE;
            break;
    }
}

void handleStoppingExpire()
{
    int timeMs;
    switch(stoppingReason)
    {
        case StoppingReason::MANUAL:
            break;

        case StoppingReason::AWAITING_DECISION:
            timeMs = millis();
            if(timeMs - startStoppingMs >= 7000)
            {
                stoppingReason = StoppingReason::NONE;
                directionSelection = static_cast<DrivingDirection>(random(3));
            }
            break;
        
        case StoppingReason::PAUSE:
            timeMs = millis();
            if(timeMs - startStoppingMs >= 5000)
            {
                stoppingReason = StoppingReason::NONE;
            }
            break;
    }
}