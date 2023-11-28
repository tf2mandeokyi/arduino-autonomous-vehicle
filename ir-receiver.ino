void IRreceiverModule::setup()
{
    irrecv.enableIRIn();
    irrecv.blink13(true);
}

void IRreceiverModule::checkInput()
{
    if(irrecv.decode())
    {
        IRRawDataType rawData = irrecv.decodedIRData.decodedRawData;
        // Reset the IR receiver for the next signal
        irrecv.resume();

        if(rawData == 0) return;
        lastReceived = static_cast<IRButton>(rawData);
    }
}

IRButton IRreceiverModule::read()
{
    IRButton result = lastReceived;
    lastReceived = IRButton::NOTHING;
    return result;
}