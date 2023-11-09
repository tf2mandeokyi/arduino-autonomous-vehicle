IRreceiverModule::IRreceiverModule(int pin)
 : irrecv(pin)
{
    irrecv.enableIRIn();
    irrecv.blink13(true);
}

IRreceiverModule::~IRreceiverModule()
{}

void IRreceiverModule::checkInput()
{
    this->irrecv.decode(&this->results);
    switch(this->results.value)
    {
        case 0xFF30CF: this->lastReceived = IRButton::BTN_1; break;
        case 0xFF18E7: this->lastReceived = IRButton::BTN_2; break;
        case 0xFF7A85: this->lastReceived = IRButton::BTN_3; break;
        case 0xFF10EF: this->lastReceived = IRButton::BTN_4; break;
        case 0xFF38C7: this->lastReceived = IRButton::BTN_5; break;
        case 0xFF5AA5: this->lastReceived = IRButton::BTN_6; break;
        case 0xFF42BD: this->lastReceived = IRButton::BTN_7; break;
        case 0xFF4AB5: this->lastReceived = IRButton::BTN_8; break;
        case 0xFF52AD: this->lastReceived = IRButton::BTN_9; break;
        default: break;
    }
}

IRButton IRreceiverModule::read()
{
    IRButton result = this->lastReceived;
    this->lastReceived = IRButton::NOTHING;
    return result;
}