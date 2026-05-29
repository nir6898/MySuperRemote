// CC1101 auto-scan spectrometer for shutter remotes
// Requires RadioLib: https://github.com/jgromes/RadioLib
// Wiring: CS->10, GDO0->2 (optional), MOSI->11, MISO->12, SCK->13
//
// This program scans 433.05..434.79 MHz in steps, sampling RSSI rapidly at each step.
// It prints per-step max RSSI and top candidates after each sweep.

#include <RadioLib.h>

CC1101 radio = new Module(10, 2, RADIOLIB_NC, RADIOLIB_NC);

// ----- user-tunable parameters -----
const float F_START_MHZ = 863.0;   // band start (MHz)
const float F_STOP_MHZ  = 870.0;   // band stop (MHz)
const float F_STEP_MHZ  = 0.05;     // step (MHz) ~50 kHz
const unsigned int SAMPLE_WINDOW_MS = 120;  // how long to sample per step (ms)
const unsigned int SAMPLE_INTERVAL_US = 300; // sample rate while in step (us)
const float MIN_HIT_DB = -73.0;     // ignore bins with max RSSI below this (dBm)
const int TOP_K = 8;                // how many top candidates to print
// ----------------------------------

struct Bin {
  float freq;
  float maxRssi;
  float avgRssi;
  unsigned int samples;
};

void setup() {
  Serial.begin(115200);
  delay(150);

  Serial.println();
  Serial.println("CC1101 Auto-Scan Spectrometer (RadioLib)");
  Serial.println("Initializing radio...");

  int state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("radio.begin() failed, code: ");
    Serial.println(state);
    while (true) { delay(1000); } // halt
  }

  // Configure for FSK (many shutter remotes are FSK/GFSK)
  radio.setOOK(false);
  radio.setBitRate(0.8);       // try 4.8 kbps; adjust if you know different
  radio.setBitRateTolerance(3);
  radio.setFrequencyDeviation(5.0);       // 10 kHz deviation — common for narrow FSK remotes
  radio.setRxBandwidth(325.0);    // widest RX BW allowed on CC1101 (kHz) for capturing bursts
  radio.setFrequency(F_START_MHZ); // initial
  delay(10);

  Serial.println("Radio ready.");
  Serial.print("Scanning ");
  Serial.print(F_START_MHZ, 5);
  Serial.print(" MHz -> ");
  Serial.print(F_STOP_MHZ, 5);
  Serial.print(" MHz  step ");
  Serial.print(F_STEP_MHZ, 6);
  Serial.println(" MHz");
  Serial.println("Press your remote during the scan.");
  Serial.println();
}

void loop() {
  // compute number of steps
  int steps = (int) ( (F_STOP_MHZ - F_START_MHZ) / F_STEP_MHZ ) + 1;
  if (steps <= 0) {
    Serial.println("Invalid step configuration.");
    while (true) delay(1000);
  }

  // allocate bins on stack (small number: about 35)
  Bin bins[256];
  if (steps > (int)(sizeof(bins)/sizeof(bins[0]))) {
    Serial.println("Too many steps for internal array. Increase F_STEP_MHZ.");
    while (true) delay(1000);
  }

  // scan each frequency bin
  int idx = 0;
  for (float f = F_START_MHZ; f <= F_STOP_MHZ + 0.000001; f += F_STEP_MHZ) {
    if (idx >= steps) break;
    bins[idx].freq = f;
    bins[idx].maxRssi = -200.0;
    bins[idx].avgRssi = 0.0;
    bins[idx].samples = 0;

    int s = radio.setFrequency(f);
    if (s != RADIOLIB_ERR_NONE) {
      // setFrequency sometimes returns non-zero on small step changes; ignore but print once
      // Serial.print("setFrequency err: "); Serial.println(s);
    }
    // allow PLL & filters to settle
    delay(6);

    unsigned long windowStart = millis();
    unsigned long windowEnd = windowStart + SAMPLE_WINDOW_MS;

    // fast sampling loop for this frequency bin
    unsigned long nextSample = micros();
    while ((int)(millis() - windowStart) < (int)SAMPLE_WINDOW_MS) {
      if ((long)(micros() - nextSample) >= 0) {
        float rssi = radio.getRSSI(); // in dBm (RadioLib reads scaled RSSI)
        bins[idx].samples++;
        bins[idx].avgRssi += rssi;
        if (rssi > bins[idx].maxRssi) bins[idx].maxRssi = rssi;

        nextSample += SAMPLE_INTERVAL_US;
      }
      // yield to let radio do background tasks if needed
      yield();
    }
    // finalize avg
    if (bins[idx].samples > 0) bins[idx].avgRssi /= bins[idx].samples;
    idx++;
  }

  // print top candidates (sorted by maxRssi)
  // simple selection sort for small N
  for (int i = 0; i < steps; i++) {
    for (int j = i+1; j < steps; j++) {
      if (bins[j].maxRssi > bins[i].maxRssi) {
        Bin tmp = bins[i]; bins[i] = bins[j]; bins[j] = tmp;
      }
    }
  }

  Serial.println("=== Sweep results (top candidates) ===");
  int printed = 0;
  for (int i = 0; i < steps && printed < TOP_K; i++) {
    if (bins[i].maxRssi < MIN_HIT_DB) continue; // ignore very weak bins
    Serial.print(printed+1);
    Serial.print(": ");
    Serial.print(bins[i].freq, 5);
    Serial.print(" MHz  maxRSSI=");
    Serial.print(bins[i].maxRssi, 1);
    Serial.print(" dBm  avg=");
    Serial.print(bins[i].avgRssi, 1);
    Serial.print(" dBm  samples=");
    Serial.println(bins[i].samples);
    printed++;
  }
  if (printed == 0) {
    Serial.println("No strong signals detected above threshold.");
    Serial.print("Try: increase SAMPLE_WINDOW_MS, reduce step, or press remote during scan.");
    Serial.println();
  }
  /*
  Serial.println("--- full table (freq, maxRSSI dBm, avg dBm) ---");
  // print a compact table for post-processing/plotting
  for (int i = 0; i < steps; i++) {
    Serial.print(bins[i].freq, 5); Serial.print(",");
    Serial.print(bins[i].maxRssi, 1); Serial.print(",");
    Serial.println(bins[i].avgRssi, 1);
  }
  */
  Serial.println("=== Sweep complete ===");
  Serial.println();

  // pause a little before next sweep
  delay(800);
}
