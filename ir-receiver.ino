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
        if(rawData == 0) return;
        lastReceived = static_cast<IRButton>(rawData);

        // Reset the IR receiver for the next signal
        irrecv.resume();
    }
}

IRButton IRreceiverModule::read()
{
    IRButton result = lastReceived;
    lastReceived = IRButton::NOTHING;
    return result;
}