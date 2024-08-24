
#include <IRremote.h>

int IRPIN = 2;

void setup()
{
  Serial.begin(9600);
  Serial.println("Enabling IRin");
  IrReceiver.begin(IRPIN, ENABLE_LED_FEEDBACK);
  Serial.println("Enabled IRin");
}

void loop()
{
  if (IrReceiver.decode())
    {
      Serial.println(":::START:::");
      Serial.println(":::Decoded Data:::");
      Serial.println(IrReceiver.decodedIRData.protocol);
      Serial.println(IrReceiver.decodedIRData.address);
      Serial.println(IrReceiver.decodedIRData.command);
      Serial.println(IrReceiver.decodedIRData.extra);
      Serial.println(IrReceiver.decodedIRData.numberOfBits);
      Serial.println(IrReceiver.decodedIRData.flags);
      Serial.println(IrReceiver.decodedIRData.decodedRawData, 2);
      Serial.println(":::Result Raw:::");
      IrReceiver.printIRResultRawFormatted(&Serial, true);
      Serial.println(":::Send Usage:::");
      IrReceiver.printIRSendUsage(&Serial);
      Serial.println(":::END:::");
      // Serial.println(IrReceiver.decodedIRData.decodedRawDataArray);
      IrReceiver.resume();
    }
  delay(500);
}