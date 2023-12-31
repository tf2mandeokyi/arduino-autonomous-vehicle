void DCMotorModule::setup()
{}

void DCMotorModule::run(int speed, DrivingDirection direction)
{
    static int prevSpeed;
    static DrivingDirection prevDirection;
    if(prevSpeed == speed && prevDirection == direction) return;
    prevSpeed = speed;
    prevDirection = direction;
    
    Serial.print("speed=");
    Serial.print(speed);
    Serial.print(", direction=");
    switch(direction)
    {
        case DrivingDirection::LEFT:  Serial.println("LEFT");  break;
        case DrivingDirection::NONE:  Serial.println("NONE(STRAIGHT)");  break;
        case DrivingDirection::RIGHT: Serial.println("RIGHT"); break;
    }

    speed = constrain(speed, -100, 100);
    speed = map(speed, -100, 100, -255, 255);

    int leftSpeed = speed, rightSpeed = speed;
    if(direction == DrivingDirection::LEFT)
    {
        leftSpeed = -speed;
    }
    else if(direction == DrivingDirection::RIGHT)
    {
        rightSpeed = -speed;
    }
    
    // int leftSpeed  = direction == DrivingDirection::RIGHT ? speed / 4 : speed;
    // int rightSpeed = direction == DrivingDirection::LEFT  ? speed / 4 : speed;

    motorFL.run(leftSpeed  > 0 ? FORWARD : leftSpeed  < 0 ? BACKWARD : RELEASE);
    motorBL.run(leftSpeed  > 0 ? FORWARD : leftSpeed  < 0 ? BACKWARD : RELEASE);
    motorFR.run(rightSpeed > 0 ? FORWARD : rightSpeed < 0 ? BACKWARD : RELEASE);
    motorBR.run(rightSpeed > 0 ? FORWARD : rightSpeed < 0 ? BACKWARD : RELEASE);
    motorFL.setSpeed(abs( leftSpeed));
    motorBL.setSpeed(abs( leftSpeed));
    motorFR.setSpeed(abs(rightSpeed));
    motorBR.setSpeed(abs(rightSpeed));
}