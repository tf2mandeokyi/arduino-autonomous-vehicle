void IRreceiverModule::setup()
{
    irrecv.enableIRIn();
    irrecv.blink13(true);
}

void IRreceiverModule::checkInput()
{
    irrecv.decode(&results);
    switch(results.value)
    {
        case 0xFF30CF: lastReceived = IRButton::BTN_1; break;
        case 0xFF18E7: lastReceived = IRButton::BTN_2; break;
        case 0xFF7A85: lastReceived = IRButton::BTN_3; break;
        case 0xFF10EF: lastReceived = IRButton::BTN_4; break;
        case 0xFF38C7: lastReceived = IRButton::BTN_5; break;
        case 0xFF5AA5: lastReceived = IRButton::BTN_6; break;
        case 0xFF42BD: lastReceived = IRButton::BTN_7; break;
        case 0xFF4AB5: lastReceived = IRButton::BTN_8; break;
        case 0xFF52AD: lastReceived = IRButton::BTN_9; break;
        default: break;
    }
}

IRButton IRreceiverModule::read()
{
    IRButton result = lastReceived;
    lastReceived = IRButton::NOTHING;
    return result;
}