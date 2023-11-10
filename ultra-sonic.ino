void UltraSonicSensorModule::setup()
{
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

float UltraSonicSensorModule::measureCm()
{
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2); // 2ms 대기
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10); // 10ms 대기 -> 최대 3.4m?
    digitalWrite(triggerPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034f / 2; // 0.034: 소리 속도 340m/s
}