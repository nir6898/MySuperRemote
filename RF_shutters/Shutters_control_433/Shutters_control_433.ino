// CC1101 direct OOK edge capture (RadioLib)
// - Works on AVR (Uno/Nano) and ESP (ESP32/ESP8266)
// - CS = 10, GDO0 = 2, GDO2 = 3 (optional)
// - radio.begin(...): adjust RXBW if needed

#include <RadioLib.h>

#ifndef IRAM_ATTR
// on AVR the IRAM_ATTR macro doesn't exist — define it away
#define IRAM_ATTR
#endif

// CC1101 module (CS, GDO0, RST, GDO2)
CC1101 radio = new Module(10, 2, RADIOLIB_NC, 3);

// capture parameters - reduce MAX_EDGES if you get RAM issues
#define GDO_PIN        2
#define MAX_EDGES      300      // adjust down if memory problems
#define MIN_GAP_US     300      // minimal length of bit
#define FRAME_GAP_US   5000     // consider frame ended when no edge for 5 ms

// volatile capture buffer (stored in RAM)
volatile uint16_t timings[MAX_EDGES]; // microsecond deltas (clamped to 65535)
volatile uint16_t edgeCount = 0;
volatile uint32_t lastEdge = 0;
volatile bool capturing = false;

void IRAM_ATTR handleEdge() {
  uint32_t now = micros();

  if (!capturing) {
    // start a new capture
    capturing = true;
    edgeCount = 0;
    lastEdge = now;
    // mark the start with a zero-length entry (helps identify the start state)
    if (edgeCount < MAX_EDGES) timings[edgeCount++] = 0;
    return;
  }

  uint32_t delta = now - lastEdge;
  if (delta > 65535) delta = 65535; // clamp so we can store in uint16_t

  if ((edgeCount < MAX_EDGES) && (delta > MIN_GAP_US)) {
    timings[edgeCount++] = (uint16_t)delta;
  } else {
    // buffer full -> stop capturing (you can increase MAX_EDGES if needed)
    capturing = false;
  }

  lastEdge = now;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(1); }

  Serial.println();
  Serial.println(F("[CC1101] Initializing for direct OOK receive..."));

  // Typical Somfy-friendly settings:
  // freq MHz, bitrate (kbit/s), freqDev (kHz - N/A for OOK), RXBW (kHz), paRamp, preambleLen
  int state = radio.begin(433.42, 2.4, 0.0, 58.0, 10, 16);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print(F("radio.begin failed, code "));
    Serial.println(state);
    Serial.println(F("Try changing RXBW (e.g. 270.8) or check wiring/power."));
    while (true) { delay(10); }
  }

  // enable OOK demodulator
  state = radio.setOOK(true);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print(F("setOOK failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  // enter direct receive mode (GDO0 outputs demodulated data)
  state = radio.receiveDirect();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print(F("receiveDirect failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  // attach interrupt to GDO pin (both edges)
  pinMode(GDO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(GDO_PIN), handleEdge, CHANGE);

  Serial.println(F("Ready — press remote near antenna to capture pulses."));
}

void loop() {
  // If we are currently capturing, and there has been no edge for FRAME_GAP_US,
  // assume the burst/frame ended and dump.
  if (capturing) {
    uint32_t idle = micros() - lastEdge;
    if (idle >= FRAME_GAP_US && edgeCount > 2) {
      // Copy buffer into a local array while interrupts are disabled to avoid races.
      noInterrupts();
      uint16_t n = edgeCount;
      static uint16_t copyBuf[MAX_EDGES]; // static to avoid stack pressure
      for (uint16_t i = 0; i < n; i++) copyBuf[i] = timings[i];
      // reset capture state for next burst
      edgeCount = 0;
      capturing = false;
      interrupts();

      // print the captured burst
      Serial.println(F("---- New burst ----"));
      Serial.print(F("edges: "));
      Serial.println(n);
      // print as microsecond deltas
      for (uint16_t i = 0; i < n; i++) {
        Serial.print(copyBuf[i]);
        Serial.print(' ');
      }
      Serial.println();
      Serial.println(F("-------------------"));
    }
  }

  // small CPU relief
  delay(10);
}
