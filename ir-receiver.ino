void IRreceiverModule::setup()
{
    irrecv.enableIRIn();
    irrecv.blink13(true);
}

void IRreceiverModule::checkInput()
{
    irrecv.decode(&results);
    lastReceived = static_cast<IRButton>(results.value);
}

IRButton IRreceiverModule::read()
{
    IRButton result = lastReceived;
    lastReceived = IRButton::NOTHING;
    return result;
}