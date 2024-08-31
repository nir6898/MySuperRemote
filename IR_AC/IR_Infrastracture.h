
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