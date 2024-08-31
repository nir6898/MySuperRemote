#include <IRremote.h>

#define IR_SEND_PIN         4
#define IR_RECIVE_PIN       2

#define IR_DATA_BUFFER_LEN  500

uint16_t rawDataBuffer[IR_DATA_BUFFER_LEN];
int rawDataBuffer_indx = 0;

void setup() {
  // Waiting to start serial session
  Serial.begin(9600);
  while(!Serial)
    delay(100);
  
  delay(1000);

  // Config send pin
  IrSender.begin(IR_SEND_PIN);
  // Config and start receive pin
  IrReceiver.begin(IR_RECIVE_PIN, ENABLE_LED_FEEDBACK);
  IrReceiver.start();
} 

void loop() {
  // Get command input from an IR remote and save it to buffer
  waitForCommand(IrReceiver);
  rawDataBuffer_indx = pushCommandToBuffer(IrReceiver, rawDataBuffer, IR_DATA_BUFFER_LEN);
  Serial.println("------ Data recorded: ------");
  printBuffer(rawDataBuffer, IR_DATA_BUFFER_LEN, rawDataBuffer_indx);
  Serial.println(" ------ End of data ------ ");
  delay(100);
  if (Serial.available() > 0) {
    // read the incoming byte:
    Serial.read();

    Serial.print("Sending command");
    IrSender.sendRaw(rawDataBuffer, rawDataBuffer_indx, 38);

  }
}

void waitForCommand(IRrecv IrReceiver){

  // Wait for command input
  Serial.println("waiting for command to be recorded...");
  while(!(IrReceiver.decode())){
    delay(100);
  }
  Serial.println("Got command recorded!");
}

int pushCommandToBuffer(IRrecv IrReceiver, uint16_t dataBuffer[], int dataBuffer_size){
  int dataBuffer_indx = 0;

  // Push recorded raw pulse input to buffer as on/off intervals
  // NOTE -- Padding with a zero before and after the recorded signal - result of trail and error...
  dataBuffer[0] = 0;
  for (int i = 0 ; (i < IrReceiver.decodedIRData.rawlen) && (i < dataBuffer_size - 2); i++) {
    dataBuffer[i+1] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i] * MICROS_PER_TICK;
    dataBuffer_indx = i + 2;
  }
  dataBuffer[dataBuffer_indx] = 0;
  // NOTE -- Padding with a zero before and after the recorded signal - result of trail and error...

  IrReceiver.resume();
  return(dataBuffer_indx);
}

void printBuffer(uint16_t dataBuffer[], int dataBuffer_size, int dataBuffer_indx){
  Serial.print("Printing the current buffer of length ");
  Serial.print(dataBuffer_indx);
  Serial.println(":");
  Serial.print("{0");
  for (int i = 1; (i < dataBuffer_indx)&&(i < dataBuffer_size); i++) {
    Serial.print(", ");
    // if (abs(dataBuffer[i] - 1000) <= 200){
    //   Serial.print(1);
    // } else if (abs(dataBuffer[i] - 2000) <= 200){
    //   Serial.print(2);
    // } else if (abs(dataBuffer[i] - 3000) <= 200){
    //   Serial.print(3);
    // } else if (abs(dataBuffer[i] - 4000) <= 200){
    //   Serial.print(4);
    // } else if (abs(dataBuffer[i]) <= 200){
    //   Serial.print(0);
    // } else {
    //   Serial.print("INVALID VALUE");
    // }
    Serial.print(dataBuffer[i]);
  }
  Serial.println("}");
  Serial.println("");

}