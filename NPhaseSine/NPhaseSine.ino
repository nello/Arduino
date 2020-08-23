
#include <Arduino.h>
#include <analogWrite.h>
#include <vprintln.h>

#define OUTPUT_PIN 12
#define CHANNEL_COUNT 6
#define RESOLUTION_X2 90
#define FQ 10
#define MAP_CHANNEL(CHANNEL) ((CHANNEL) == 20 ? 25 : (CHANNEL))   // GPIO20 isn't exposed on an ESP-32

#define H_BRIDGE true

int wave_outputs[CHANNEL_COUNT][RESOLUTION_X2]; 
int fq_delay = FQ > 0 ? (1024 / FQ / RESOLUTION_X2) : 0;

void setupChannels() {
  // 2 h-bridge channels for each magnet
  for (int i=0; i < CHANNEL_COUNT * 2; ++ i) {
      pinMode(OUTPUT_PIN + i, OUTPUT);
  }
  pinMode(25, OUTPUT);
}

void setupWaveTable() {
  // store precalculations
  float x = 0;
  for (int j = 0; j < RESOLUTION_X2; ++j) { 
      for (int i=0; i < CHANNEL_COUNT; ++ i) {
          wave_outputs[i][j] = sin(x + (i * TWO_PI / CHANNEL_COUNT)) * 256;
      }
      x = x + (2 * PI / RESOLUTION_X2);  
  }

  // print out table
  vprint("\n");
  for (int i=0; i < CHANNEL_COUNT; ++ i) {
    for (int j = 0; j < RESOLUTION_X2; ++j) { 
      vprint(wave_outputs[i][j], " ");
    }
    vprint("\n");
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);

  setupChannels();
  setupWaveTable();
}

void loop()  {
  for (int j = 0; j < RESOLUTION_X2; ++j) {
      for (int i=0; i < CHANNEL_COUNT; ++ i) {
          int output = wave_outputs[i][j];
   
          if (H_BRIDGE) {
            int output_channel = OUTPUT_PIN + (i << 1);
            if (output > 0) {
                analogWrite(MAP_CHANNEL(output_channel), output);
                analogWrite(MAP_CHANNEL(output_channel + 1), 0);
            } else {
                analogWrite(MAP_CHANNEL(output_channel), 0);
                analogWrite(MAP_CHANNEL(output_channel + 1), -output);
            }
          } else {
            analogWrite(OUTPUT_PIN + i, output >> 1 + 127);
          }
      }
      if (fq_delay) delay(fq_delay); 
  }
}
