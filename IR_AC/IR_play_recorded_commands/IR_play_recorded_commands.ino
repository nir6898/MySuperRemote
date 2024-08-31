#include <IRremote.h>
#include "./recorded_IR_AC_commands.h"

#define IR_SEND_PIN         4
#define IR_DEFUALT_FREQ     38

void setup() {
  // Waiting to start serial session
  Serial.begin(9600);
  while(!Serial)
    delay(100);

  // Config send pin
  IrSender.begin(IR_SEND_PIN);
  delay(1000);
  Serial.println("sending command On");
  IrSender.sendRaw(command_ON_OFF, command_len, IR_DEFUALT_FREQ);
} 

void loop() {
  delay(3000);
}

