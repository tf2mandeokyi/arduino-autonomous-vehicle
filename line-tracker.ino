#define LINE_BLACK  0
#define LINE_WHITE  1

TimedBinaryInputStacker::TimedBinaryInputStacker(int _timeoutMs)
 : timeoutMs(_timeoutMs), countingMode(false), prevInput(false), count(0)
{}

TimedBinaryInputStacker::~TimedBinaryInputStacker()
{}

void TimedBinaryInputStacker::insertInput(bool input)
{
    if(input)
    {
        if(this->prevInput) return;

        // input이 false에서 true로 바뀌었을 떄
        if(this->countingMode) return;

        // countingMode가 false였을 때
        this->countingMode = true;
        this->count = 0;
        this->prevInput = true;
    }
    else if(prevInput)
    {
        // input이 true에서 false로 바뀌었을 때
        int timeMs = millis();
        this->timeSinceLastInputMs = timeMs;
        this->count++;
        this->prevInput = false;
    }
    else
    {
        // input이 계속 false일 때
        if(!this->countingMode) return;

        // countingMode가 true였을 때
        int timeMs = millis();
        if(timeMs - this->timeSinceLastInputMs < this->timeoutMs) return;

        // 지금부터 input이 마지막으로 꺼진 때까지 걸린 시간이 timeoutMs만큼을 넘겼을 때
        this->countingMode = false;
    }
}

int TimedBinaryInputStacker::readCount()
{
    if(this->countingMode) return 0;
    int result = this->count;
    this->count = 0;
    return result;
}

bool TimedBinaryInputStacker::isCountingMode()
{
    return this->countingMode;
}

LineTrackerModule::LineTrackerModule(int _sideLeftPin, int _frontLeftPin, int _frontPin, int _frontRightPin, int _sideRightPin)
 : sideLeftPin(_sideLeftPin), frontLeftPin(_frontLeftPin), frontPin(_frontPin), frontRightPin(_frontRightPin), sideRightPin(_sideRightPin),
   leftStacker(500), rightStacker(500)
{
    pinMode(sideLeftPin, INPUT);
    pinMode(frontLeftPin, INPUT);
    pinMode(frontPin, INPUT);
    pinMode(frontRightPin, INPUT);
    pinMode(sideRightPin, INPUT);
}

LineTrackerModule::~LineTrackerModule()
{}

byte LineTrackerModule::readFront()
{
    int frontLeft = digitalRead(this->frontLeftPin);
    int front = digitalRead(this->frontPin);
    int frontRight = digitalRead(this->frontRightPin);

    byte result = 0;
    if(frontLeft  == LINE_BLACK) result |= 4;
    if(front      == LINE_BLACK) result |= 2;
    if(frontRight == LINE_BLACK) result |= 1;
    return result;
}

SideMarking LineTrackerModule::readSideMarking()
{
    int sideLeft = digitalRead(this->sideLeftPin);
    int sideRight = digitalRead(this->sideRightPin);

    this->leftStacker.insertInput(sideLeft == LINE_BLACK ? true : false);
    this->rightStacker.insertInput(sideRight == LINE_BLACK ? true : false);

    if(!this->leftStacker.isCountingMode() && !this->rightStacker.isCountingMode())
    {
        int leftCount = this->leftStacker.readCount();
        int rightCount = this->rightStacker.readCount();
        
        // 감속 마킹
        if(leftCount + rightCount == 1)
            return SideMarking::SLOW;

        // 가속 마킹
        else if(leftCount == 1 && rightCount == 1)
            return SideMarking::FAST;

        // 방향 결정 마킹
        else if(leftCount + rightCount == 3 && (leftCount == 1 || rightCount == 1))
            return SideMarking::CHOOSE_DIRECTION;

        // 일시정지 마킹
        else if(leftCount == 2 && rightCount == 2)
            return SideMarking::STOP;

        return SideMarking::NOTHING;
    }
}