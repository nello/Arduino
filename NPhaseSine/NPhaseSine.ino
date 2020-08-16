
#include <Arduino.h>
#include <analogWrite.h>
#include <vprintln.h>

#define OUTPUT_PIN 12
#define CHANNEL_COUNT 6
#define RESOLUTION 30
#define FQ 1

#define H_BRIDGE true

const float STEP = PI/RESOLUTION;
float wave_offsets[CHANNEL_COUNT]; 
int fq_delay = FQ > 0 ? (1000 / FQ / RESOLUTION) : 0;

void setup() {
  delay(1000);
  Serial.begin(115200);

  for (int i=0; i < CHANNEL_COUNT * 2; ++ i) {
      pinMode(OUTPUT_PIN + i, OUTPUT);
  }
  pinMode(25, OUTPUT);
  for (int i=0; i < CHANNEL_COUNT; ++ i) {
      wave_offsets[i] = i * TWO_PI / CHANNEL_COUNT;
  }
}

void loop()  {
  for (float x = 0; x < TWO_PI; x = x + STEP) {
      //int sum = 0;
      for (int i=0; i < CHANNEL_COUNT; ++ i) {
          int output = sin(x + wave_offsets[i]) * 256;   // better to build a wave table in setup()!!!
   
          if (H_BRIDGE) {
            int output_channel = OUTPUT_PIN + (i << 1);
            if (output > 0) {
                analogWrite(output_channel == 20 ? 25 : output_channel, output);
                analogWrite(output_channel + 1, 0);
            } else {
                analogWrite(output_channel == 20 ? 25 : output_channel, 0);
                analogWrite(output_channel + 1, -output);
            }
          } else {
            analogWrite(OUTPUT_PIN + i, output >> 1 + 127);
          }
          //vprint(output, " "); sum = sum + output;
      }
      //vprintln("= ", sum);
      if (fq_delay) delay(fq_delay); 
  }
}
