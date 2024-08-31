#include <IRremote.h>

#define IR_SEND_PIN         4
#define IR_RECIVE_PIN       2

#define IR_DATA_BUFFER_LEN  500

uint16_t rawDataBuffer[IR_DATA_BUFFER_LEN];
int rawDataBuffer_indx = 0;
// uint16_t recorded_command[188] = {0, 3050, 3950, 950, 1000, 1950, 1900, 950, 1000, 1950, 1000, 900, 1000, 950, 1000, 950, 1000, 950, 1000, 950, 1000, 900, 2000, 950, 1000, 900, 1000, 1950, 1000, 950, 950, 950, 1000, 950, 1000, 950, 1000, 950, 950, 1000, 1000, 900, 1050, 900, 1000, 950, 1000, 950, 1000, 950, 950, 1000, 950, 950, 1000, 950, 1000, 950, 950, 1000, 1950, 1900, 1050, 2900, 3950, 950, 1000, 1950, 1950, 950, 950, 1950, 1000, 950, 1000, 950, 950, 1000, 1000, 900, 1000, 950, 1000, 950, 1950, 1000, 950, 950, 950, 1950, 1000, 950, 1000, 950, 1000, 900, 1000, 950, 1000, 950, 1000, 1000, 950, 950, 950, 1000, 1000, 950, 950, 950, 1000, 950, 1000, 950, 950, 1000, 950, 1000, 950, 950, 1000, 1000, 1900, 1950, 1000, 2900, 4000, 1000, 900, 1950, 1950, 950, 1000, 1950, 950, 950, 1000, 1000, 950, 950, 950, 1050, 900, 1000, 1000, 950, 1900, 1000, 950, 1000, 950, 1950, 950, 1000, 950, 950, 1000, 950, 1000, 950, 1000, 950, 1000, 950, 950, 1000, 950, 1000, 950, 950, 1000, 900, 1000, 1000, 950, 1000, 950, 1000, 950, 1000, 900, 1050, 900, 1000, 1950, 1950, 950, 3900};


void setup() {
  // Start serial session
  Serial.begin(9600);
  while(!Serial)
    ;
  
  delay(5000);

  // Config send pin
  IrSender.begin(IR_SEND_PIN);
  // Config recieve pin
  IrReceiver.begin(IR_RECIVE_PIN, ENABLE_LED_FEEDBACK);

  // Get command input from an IR remote and save it to buffer
  rawDataBuffer_indx = waitAndPushCommandToBuffer(IrReceiver, rawDataBuffer, IR_DATA_BUFFER_LEN);
  delay(1000);
  printBuffer(rawDataBuffer, IR_DATA_BUFFER_LEN, rawDataBuffer_indx);
  delay(2000);
} 

void loop() {
  // resend saved command from buffer
  IrSender.sendRaw(rawDataBuffer, rawDataBuffer_indx, 38);
  delay(500);
  if (IrReceiver.decode())
  {
    rawDataBuffer_indx = pushCommandToBuffer(IrReceiver, rawDataBuffer, IR_DATA_BUFFER_LEN);
    delay(500);
    printBuffer(rawDataBuffer, IR_DATA_BUFFER_LEN, rawDataBuffer_indx);
    delay(500);
  }

  // delay(5000);
  // Serial.println("tick");
  // delay(500);  
  // if (IrReceiver.decode()){
  //     Serial.println("Got command recorded!");
  
  //   for (int i = 0; (i < IrReceiver.decodedIRData.rawlen) && (i < IR_DATA_BUFFER_LEN); i++) {
  //     rawDataBuffer[i] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i] * MICROS_PER_TICK;
  //     rawDataBuffer_len = i;
  //     Serial.print(rawDataBuffer[i]);
  //     Serial.print(", ");
  //   }
  //   delay  (25000000000);
  // }
  // else {
  //   Serial.println("Failed to transmit");
  // }
  // delay(3000);
  // // Serial.println("Sending command ->");
  // // Serial.print("commans length: ");
  // // Serial.println(rawDataBuffer_len);
  // // IrSender.sendRaw(rawDataBuffer, rawDataBuffer_len, 38);
  // // delay(5000);
}

int waitAndPushCommandToBuffer(IRrecv IrReceiver, uint16_t dataBuffer[], int dataBuffer_size){

  // Wait for command input
  Serial.println("waiting for command to be recorded...");
  while(!(IrReceiver.decode())){
    delay(500);
  }
  Serial.println("Got command recorded!");

  return(pushCommandToBuffer(IrReceiver, dataBuffer, dataBuffer_size));
}

int pushCommandToBuffer(IRrecv IrReceiver, uint16_t dataBuffer[], int dataBuffer_size){
  int dataBuffer_indx = 0;

  // Push recorded raw pulse input to buffer as on/off intervals
  for (int i = 0; (i < IrReceiver.decodedIRData.rawlen) && (i < dataBuffer_size); i++) {
    dataBuffer[i] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i] * MICROS_PER_TICK;
    dataBuffer_indx = i + 1;
  }
  Serial.print("command length: ");
  Serial.println(dataBuffer_indx);
  return(dataBuffer_indx);
}

void printBuffer(uint16_t dataBuffer[], int dataBuffer_size, int dataBuffer_indx){
  Serial.println("");
  Serial.print("Printing the current buffer of length ");
  Serial.print(dataBuffer_indx);
  Serial.println(":");
  Serial.println("");
  Serial.print("[");
  for (int i = 0; (i < dataBuffer_indx)&&(i < dataBuffer_size); i++) {
    Serial.print(dataBuffer[i]);
    Serial.print(", ");
  }
  Serial.println("]");
}