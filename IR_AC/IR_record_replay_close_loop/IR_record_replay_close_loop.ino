#include <IRremote.h>

#define IR_SEND_PIN         4
#define IR_RECIVE_PIN       2

#define IR_DATA_BUFFER_LEN  500

uint16_t tempRawDataBuffer[IR_DATA_BUFFER_LEN];
int tempRawDataBuffer_indx = 0;

uint16_t origRawDataBuffer[IR_DATA_BUFFER_LEN];
int origRawDataBuffer_indx = 0;

void setup() {
  // Waiting to start serial session
  Serial.begin(9600);
  while(!Serial)
    delay(100);
  
  delay(1000);

  // Config send pin
  IrSender.begin(IR_SEND_PIN);
  // Config recieve pin
  IrReceiver.begin(IR_RECIVE_PIN, ENABLE_LED_FEEDBACK);
  IrReceiver.start();

  // Get command input from an IR remote and save it to buffer
  waitForCommand(IrReceiver);
  origRawDataBuffer_indx = pushCommandToBuffer(IrReceiver, origRawDataBuffer, IR_DATA_BUFFER_LEN);
  Serial.println("------ Original Data recorded: ------");
  printBuffer(origRawDataBuffer, IR_DATA_BUFFER_LEN, origRawDataBuffer_indx);
  Serial.println(" ------ End of original data ------ ");
  Serial.println(" ---------------------------------- ");

} 

void loop() {
  
  if (IrReceiver.decode())
  {
    tempRawDataBuffer_indx = pushCommandToBuffer(IrReceiver, tempRawDataBuffer, IR_DATA_BUFFER_LEN);
    Serial.println("New data recorded! ");
    printBuffer(tempRawDataBuffer, IR_DATA_BUFFER_LEN, tempRawDataBuffer_indx);
  } else {
    // resend saved origianl command from buffer
    delay(3000);
    IrSender.sendRaw(origRawDataBuffer, origRawDataBuffer_indx, 38);
  }
  delay(100);
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
  Serial.print("[");
  for (int i = 0; (i < dataBuffer_indx)&&(i < dataBuffer_size); i++) {
    Serial.print(dataBuffer[i]);
    Serial.print(", ");
  }
  Serial.println("]");
  Serial.println("");

}