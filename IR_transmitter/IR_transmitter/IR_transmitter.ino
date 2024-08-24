#include <IRremote.h>

#define IR_SEND_PIN         4
#define IR_RECIVE_PIN       2

#define IR_DATA_BUFFER_LEN  500

uint16_t rawDataBuffer[IR_DATA_BUFFER_LEN];
int rawDataBuffer_len = 0;


void setup() {
  Serial.begin(9600);
  while(!Serial)
    ;

  IrSender.begin(IR_SEND_PIN); // Start with IR_SEND_PIN -which is defined in PinDefinitionsAndMore.h- as send pin and enable feedback LED at default feedback LED pin
  IrReceiver.begin(IR_RECIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("waiting for command to be recorded...");
  while(!(IrReceiver.decode()))
    ;

  Serial.println("Got command recorded!");
  
  for (int i = 0; (i < IrReceiver.decodedIRData.rawlen) && (i < IR_DATA_BUFFER_LEN); i++) {
    rawDataBuffer[i] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i] * MICROS_PER_TICK;
    rawDataBuffer_len = i;
    Serial.print(rawDataBuffer[i]);
    Serial.print(", ");
  }
  // Serial.println("commans length: ");
  // Serial.println(sizeof(rawData) / sizeof(rawData[0]));


} 


void loop() {
  Serial.println("Sending command ->");
  Serial.println("commans length: ");
  Serial.print(rawDataBuffer_len);
  IrSender.sendRaw(rawDataBuffer, rawDataBuffer_len, 38);
  delay(5000);
}
