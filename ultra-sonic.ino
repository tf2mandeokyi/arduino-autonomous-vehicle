void UltraSonicSensorModule::setup()
{}

float UltraSonicSensorModule::measureCm()
{
    return sonar.ping_cm();
}