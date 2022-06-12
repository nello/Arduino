#include <Arduino.h>

#define CHANNELS 16

int16_t channel[CHANNELS] = {0};
int j;

void setup() {
      Serial.begin(115200);
      while (!Serial) {}
      Serial2.begin(19200);    // RX2 - 100,000 for SBUS
      j = 0;
}

void loop() {
  uint8_t avail = Serial2.available();
  if (avail > 0) {    
    for(uint8_t i=0; j < CHANNELS && i < avail; i++) {
        if (Serial2.available() > 0) {
            channel[j++] = Serial2.read();        // raw data
        }
    }

    if (j == CHANNELS) {          
        Serial.print("Channels");
        for(uint8_t i=0; i < CHANNELS; i++) {
            Serial.print(" ");
            Serial.print(i);
            Serial.print(":");
            Serial.print(channel[i], DEC);
        }
        Serial.println();
        j = 0;
    }
  }
}
