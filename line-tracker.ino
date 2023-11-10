#define LINE_BLACK  0
#define LINE_WHITE  1

LineTrackerModule::TimedBinaryInputStacker::TimedBinaryInputStacker(int _timeoutMs)
 : timeoutMs(_timeoutMs), countingMode(false), prevInput(false), count(0)
{}

LineTrackerModule::TimedBinaryInputStacker::~TimedBinaryInputStacker()
{}

void LineTrackerModule::TimedBinaryInputStacker::insertInput(bool input)
{
    if(input)
    {
        if(prevInput) return;

        // input이 false에서 true로 바뀌었을 떄
        if(countingMode) return;

        // countingMode가 false였을 때
        countingMode = true;
        count = 0;
        prevInput = true;
    }
    else if(prevInput)
    {
        // input이 true에서 false로 바뀌었을 때
        int timeMs = millis();
        timeSinceLastInputMs = timeMs;
        count++;
        prevInput = false;
    }
    else
    {
        // input이 계속 false일 때
        if(!countingMode) return;

        // countingMode가 true였을 때
        int timeMs = millis();
        if(timeMs - timeSinceLastInputMs < timeoutMs) return;

        // 지금부터 input이 마지막으로 꺼진 때까지 걸린 시간이 timeoutMs만큼을 넘겼을 때
        countingMode = false;
    }
}

int LineTrackerModule::TimedBinaryInputStacker::readCount()
{
    if(countingMode) return 0;
    int result = count;
    count = 0;
    return result;
}

bool LineTrackerModule::TimedBinaryInputStacker::isCountingMode()
{
    return countingMode;
}

void LineTrackerModule::setup()
{
    pinMode(sideLeftPin, INPUT);
    pinMode(frontLeftPin, INPUT);
    pinMode(frontPin, INPUT);
    pinMode(frontRightPin, INPUT);
    pinMode(sideRightPin, INPUT);
}

byte LineTrackerModule::readFront()
{
    int frontLeft = digitalRead(frontLeftPin);
    int front = digitalRead(frontPin);
    int frontRight = digitalRead(frontRightPin);

    byte result = 0;
    if(frontLeft  == LINE_BLACK) result |= 4;
    if(front      == LINE_BLACK) result |= 2;
    if(frontRight == LINE_BLACK) result |= 1;
    return result;
}

SideMarking LineTrackerModule::readSideMarking()
{
    int sideLeft = digitalRead(sideLeftPin);
    int sideRight = digitalRead(sideRightPin);

    leftStacker.insertInput(sideLeft == LINE_BLACK ? true : false);
    rightStacker.insertInput(sideRight == LINE_BLACK ? true : false);

    if(!leftStacker.isCountingMode() && !rightStacker.isCountingMode())
    {
        int leftCount = leftStacker.readCount();
        int rightCount = rightStacker.readCount();
        
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