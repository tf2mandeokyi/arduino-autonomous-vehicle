#include "header.h"

// 최대 100
const int SPEED = 100;
int drivingSpeed = SPEED;
DrivingDirection drivingDirection = DrivingDirection::NONE;

DrivingDirection turningMode = DrivingDirection::NONE;
long turningStartMs = 0;

long startStoppingMs;
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

int i = 0;
long frontMarkingLastChecked = millis();
void loop()
{
    timeMs = millis();

    // 입력
    IRreceiverModule::checkInput();
    IRButton receivedButton = IRreceiverModule::read();
    int distanceCm = 10; //UltraSonicSensorModule::measureCm();
    byte frontPathFlag = LineTrackerModule::readFront();
    // SideMarking sideMarking = LineTrackerModule::readSideMarking();

    // 계산: 마킹 신호 반응
    handleIRButton(receivedButton);
    // handleSideMarking(sideMarking);
    if(timeMs - frontMarkingLastChecked >= 150)
    {
        handleFrontPathFlag(frontPathFlag);
        frontMarkingLastChecked = timeMs;
    }
    handleStoppingExpire();

    if(turningMode != DrivingDirection::NONE && timeMs - turningStartMs >= 0)
    {
        long delta = timeMs - turningStartMs;
        if(delta <= 400)
        {
            drivingSpeed = -SPEED;
        }
        else if(delta <= 800)
        {
            drivingSpeed = SPEED;
            drivingDirection = turningMode;
        }
        else
        {
            drivingSpeed = SPEED;
            drivingDirection = DrivingDirection::NONE;
            turningMode = DrivingDirection::NONE;
        }
    }

    // 출력
    static StoppingReason prevStop = StoppingReason::NONE;
    int speed = drivingSpeed;
    if((distanceCm < 8 || stoppingReason != StoppingReason::NONE) && !manualDrivingMode)
    {
        if(prevStop != stoppingReason)
        {
            prevStop = stoppingReason;
            switch(stoppingReason)
            {
                case StoppingReason::NONE: Serial.println("Stopping reason: NONE"); break;
                case StoppingReason::MANUAL: Serial.println("Stopping reason: MANUAL"); break;
                case StoppingReason::PAUSE: Serial.println("Stopping reason: PAUSE"); break;
                case StoppingReason::AWAITING_DECISION: Serial.println("Stopping reason: AWAITING_DECISION"); break;
            }
        }
        // 앞에 장애물이 있거나 정지 사유가 존재할 경우
        speed = 0;
    }
    else
    {
        prevStop = StoppingReason::NONE;
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
            drivingSpeed = SPEED;
            Serial.println("Manual driving disabled");
            return;
        }
        drivingSpeed = 0;
        drivingDirection = DrivingDirection::NONE;
        Serial.println("Stopped!");
        return;
    }

    switch(receivedButton)
    {
        case IRButton::BTN_1: case IRButton::BTN_2: case IRButton::BTN_3: drivingSpeed =  SPEED; break;
        case IRButton::BTN_7: case IRButton::BTN_8: case IRButton::BTN_9: drivingSpeed = -SPEED; break;
    }
    switch(receivedButton)
    {
        case IRButton::BTN_1: case IRButton::BTN_7: drivingDirection = DrivingDirection::LEFT ; break;
        case IRButton::BTN_2: case IRButton::BTN_8: drivingDirection = DrivingDirection::NONE ; break;
        case IRButton::BTN_3: case IRButton::BTN_9: drivingDirection = DrivingDirection::RIGHT; break;
    }
}

void handleSideMarking(SideMarking sideMarking)
{
    if(manualDrivingMode) return;

    switch(sideMarking)
    {
        case SideMarking::SLOW:
            if(!directionChangingMode)
            {
                drivingSpeed = SPEED;
                Serial.println("Read slow marking");
            }
            break;
            
        case SideMarking::FAST:
            if(!directionChangingMode) 
            {
                drivingSpeed = SPEED;
                Serial.println("Read fast marking");
            }
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
                Serial.println("Read direction marking again; disabled direction changing mode");
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

    Serial.println(frontPathFlag, 2);
    switch(frontPathFlag)
    {
        case 0b000:
        case 0b010:
            drivingDirection = DrivingDirection::NONE;
            drivingSpeed = SPEED;
            break;

        case 0b110:
            if(directionChangingMode) 
            {
                drivingDirection = directionSelection == DrivingDirection::LEFT ? DrivingDirection::LEFT : DrivingDirection::NONE;
            }
            break;
        case 0b100:
            // drivingDirection = DrivingDirection::LEFT;
            // drivingDirection = DrivingDirection::RIGHT;
            // drivingSpeed = -SPEED;
            turningMode = DrivingDirection::LEFT;
            turningStartMs = timeMs;
            break;

        case 0b011:
            if(directionChangingMode) 
            {
                drivingDirection = directionSelection == DrivingDirection::RIGHT ? DrivingDirection::RIGHT : DrivingDirection::NONE;
            }
            break;
        case 0b001:
            // drivingDirection = DrivingDirection::RIGHT;
            // drivingDirection = DrivingDirection::LEFT;
            // drivingSpeed = -SPEED;
            turningMode = DrivingDirection::RIGHT;
            turningStartMs = timeMs;
            break;

        case 0b111:
            if(directionChangingMode)
            {
                drivingDirection = directionSelection;
                break;
            }
            drivingDirection = DrivingDirection::NONE;
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
                selectDirection(static_cast<DrivingDirection>(random(3)));
                Serial.println("Can't wait longer than this! 7s passed, chose random direction...");
            }
            break;
        
        case StoppingReason::PAUSE:
            if(timeMs - startStoppingMs >= 5000)
            {
                Serial.println("Waited long enough! Will continue driving");
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
    Serial.println("Enabled direction changing mode");
}