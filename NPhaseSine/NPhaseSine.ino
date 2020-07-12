
#include <Arduino.h>
#include <analogWrite.h>
#include <vprintln.h>

#define OUTPUT_PIN 12
#define CHANNEL_COUNT 3
#define RESOLUTION 120

const float STEP = PI/RESOLUTION;
float wave_offsets[CHANNEL_COUNT];

void setup() {
  delay(1000);
//  Serial.begin(115200);

  for (int i=0; i < CHANNEL_COUNT; ++ i) {
      pinMode(OUTPUT_PIN + i, OUTPUT);
      wave_offsets[i] = i * TWO_PI / CHANNEL_COUNT;
  }
}

void loop()  {
  for (float x = 0; x < TWO_PI; x = x + STEP) {
      //int sum = 0;
      for (int i=0; i < CHANNEL_COUNT; ++ i) {
          int output = sin(x + wave_offsets[i]) * 128;
          analogWrite(OUTPUT_PIN + i, output + 127);
          //vprint(output, " "); sum = sum + output;
      }
      //vprintln("= ", sum);
      //delay(1); 
  }
}
