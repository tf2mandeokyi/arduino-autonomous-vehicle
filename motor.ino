DCMotorModule::DCMotorModule()
 : motorBL(1), motorBR(2), motorFR(3), motorFL(4)
{}

DCMotorModule::~DCMotorModule()
{}

void DCMotorModule::run(int speed, DrivingDirection direction)
{
    speed = constrain(speed, -100, 100);
    speed = map(speed, -100, 100, -255, 255);
    
    int leftSpeed  = direction == DrivingDirection::LEFT  ? speed / 2 : speed;
    int rightSpeed = direction == DrivingDirection::RIGHT ? speed / 2 : speed;

    motorFL.run(leftSpeed  > 0 ? FORWARD : leftSpeed  < 0 ? BACKWARD : RELEASE);
    motorBL.run(leftSpeed  > 0 ? FORWARD : leftSpeed  < 0 ? BACKWARD : RELEASE);
    motorFR.run(rightSpeed > 0 ? FORWARD : rightSpeed < 0 ? BACKWARD : RELEASE);
    motorBR.run(rightSpeed > 0 ? FORWARD : rightSpeed < 0 ? BACKWARD : RELEASE);
    motorFL.setSpeed(abs( leftSpeed));
    motorBL.setSpeed(abs( leftSpeed));
    motorFR.setSpeed(abs(rightSpeed));
    motorBR.setSpeed(abs(rightSpeed));
}