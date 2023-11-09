UltraSonicSensorModule::UltraSonicSensorModule(int _triggerPin, int _echoPin)
 : triggerPin(_triggerPin), echoPin(_echoPin)
{
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

UltraSonicSensorModule::~UltraSonicSensorModule()
{}

float UltraSonicSensorModule::measureCm()
{
    digitalWrite(this->triggerPin, LOW);
    delayMicroseconds(2); // 2ms 대기
    digitalWrite(this->triggerPin, HIGH);
    delayMicroseconds(10); // 10ms 대기 -> 최대 3.4m?
    digitalWrite(this->triggerPin, LOW);

    long duration = pulseIn(this->echoPin, HIGH);
    return duration * 0.034f / 2; // 0.034: 소리 속도 340m/s
}