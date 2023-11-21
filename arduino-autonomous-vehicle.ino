#include "header.h"

// 최대 100
int drivingSpeed = 100;
DrivingDirection drivingDirection = DrivingDirection::NONE;
int startStoppingMs;
StoppingReason stoppingReason = StoppingReason::NONE;
bool directionChangingMode = false;
DrivingDirection directionSelection = DrivingDirection::NONE;
bool manualDrivingMode = false;
long timeMs;

void handleIRButton(IRButton receivedButton);
void handleManualDrivingIRButton(IRButton receivedButton);
void handleSideMarking(SideMarking sideMarking);
void handleFrontPathFlag(byte frontPathFlag);
void handleStoppingExpire();
void selectDirection(DrivingDirection direction);

void setup()
{
    Serial.begin(115200);
    DCMotorModule::setup();
    UltraSonicSensorModule::setup();
    LineTrackerModule::setup();
    IRreceiverModule::setup();
}

void loop()
{
    timeMs = millis();

    // 입력
    IRreceiverModule::checkInput();
    IRButton receivedButton = IRreceiverModule::read();
    int distanceCm = UltraSonicSensorModule::measureCm();
    byte frontPathFlag = LineTrackerModule::readFront();
    SideMarking sideMarking = LineTrackerModule::readSideMarking();

    // 계산: 마킹 신호 반응
    handleIRButton(receivedButton);
    handleSideMarking(sideMarking);
    handleFrontPathFlag(frontPathFlag);
    handleStoppingExpire();

    // 출력
    int speed = drivingSpeed;
    if(distanceCm < 8 || stoppingReason != StoppingReason::NONE)
    {
        // 앞에 장애물이 있거나 정지 사유가 존재할 경우
        speed = 0;
    }
    DCMotorModule::run(speed, drivingDirection);
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
                Serial.println("Disabled manual stop");
            }
            else if(stoppingReason == StoppingReason::AWAITING_DECISION)
            {
                selectDirection(DrivingDirection::NONE);
            }
            break;
            
        case IRButton::BTN_4:
            if(stoppingReason == StoppingReason::AWAITING_DECISION)
            {
                selectDirection(DrivingDirection::LEFT);
            }
            break;
            
        case IRButton::BTN_5:
            manualDrivingMode = true;
            Serial.println("Manual driving enabled");
            break;
            
        case IRButton::BTN_6:
            if(stoppingReason == StoppingReason::AWAITING_DECISION)
            {
                selectDirection(DrivingDirection::RIGHT);
            }
            break;
            
        case IRButton::BTN_8:
            stoppingReason = StoppingReason::MANUAL;
            Serial.println("Manually stopped");
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
            Serial.println("Manual driving disabled");
            return;
        }
        drivingSpeed = 0;
        drivingDirection = DrivingDirection::NONE;
        Serial.println("Stopping!");
    }

    bool buttonSuccess = false;
    switch(receivedButton)
    {
        case IRButton::BTN_1: case IRButton::BTN_2: case IRButton::BTN_3: drivingSpeed =  100; buttonSuccess = true; break;
        case IRButton::BTN_7: case IRButton::BTN_8: case IRButton::BTN_9: drivingSpeed = -100; buttonSuccess = true; break;
    }
    switch(receivedButton)
    {
        case IRButton::BTN_1: case IRButton::BTN_7: drivingDirection = DrivingDirection::LEFT ; buttonSuccess = true; break;
        case IRButton::BTN_2: case IRButton::BTN_8: drivingDirection = DrivingDirection::NONE ; buttonSuccess = true; break;
        case IRButton::BTN_3: case IRButton::BTN_9: drivingDirection = DrivingDirection::RIGHT; buttonSuccess = true; break;
    }
    if(buttonSuccess)
    {
        Serial.print("Changing direction and speed to: speed=");
        Serial.print(drivingSpeed);
        Serial.print(", direction=");
        Serial.println(static_cast<byte>(drivingDirection));
    }
}

void handleSideMarking(SideMarking sideMarking)
{
    if(manualDrivingMode) return;

    switch(sideMarking)
    {
        case SideMarking::SLOW:
            if(!directionChangingMode) drivingSpeed = 100;
            Serial.print("Read slow marking; driving speed is now ");
            Serial.println(drivingSpeed);
            break;
            
        case SideMarking::FAST:
            if(!directionChangingMode) drivingSpeed = 100;
            Serial.print("Read fast marking; driving speed is now ");
            Serial.println(drivingSpeed);
            break;
            
        case SideMarking::CHOOSE_DIRECTION:
            if(!directionChangingMode)
            {
                stoppingReason = StoppingReason::AWAITING_DECISION;
                startStoppingMs = timeMs;
                Serial.println("Read direction marking; awaiting signal...");
            }
            else if(directionChangingMode)
            {
                directionChangingMode = false;
                Serial.println("Read direction marking again; disabling direction changing mode...");
            }
            break;
            
        case SideMarking::STOP:
            if(!directionChangingMode)
            {
                stoppingReason = StoppingReason::PAUSE;
                startStoppingMs = timeMs;
                Serial.println("Read stop marking!");
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
    switch(stoppingReason)
    {
        case StoppingReason::MANUAL:
            break;

        case StoppingReason::AWAITING_DECISION:
            if(timeMs - startStoppingMs >= 7000)
            {
                Serial.println("Can't wait longer than this! 7s passed, choosing random direction...");
                selectDirection(static_cast<DrivingDirection>(random(3)));
            }
            break;
        
        case StoppingReason::PAUSE:
            if(timeMs - startStoppingMs >= 5000)
            {
                Serial.println("Waited long enough! Driving continue");
                stoppingReason = StoppingReason::NONE;
            }
            break;
    }
}

void selectDirection(DrivingDirection direction)
{
    switch(direction)
    {
        case DrivingDirection::LEFT:
            directionSelection = DrivingDirection::LEFT;
            Serial.println("Read direction selection signal: LEFT");
            break;
        case DrivingDirection::NONE:
            directionSelection = DrivingDirection::NONE;
            Serial.println("Read direction selection signal: NONE");
            break;
        case DrivingDirection::RIGHT:
            directionSelection = DrivingDirection::RIGHT;
            Serial.println("Read direction selection signal: RIGHT");
            break;
        default:
            Serial.println("Read unknown signal");
            return;
    }
    stoppingReason = StoppingReason::NONE;
    directionChangingMode = true;
}