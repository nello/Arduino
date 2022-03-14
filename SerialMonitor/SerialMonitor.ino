#include <Arduino.h>

int incomingByte = 0;   // for incoming serial data
uint8_t FrameErrorRate = 0;
int16_t channel[25] = {0};
int j;

void setup() {
      Serial.begin(115200);
      Serial2.begin(100000);    // RX2
      j = 0;
}

void loop() {
  uint8_t avail = Serial2.available();
  if (avail > 0) {
//    Serial.print("Avail: ");
//    Serial.println(avail);
    
    for(uint8_t i=0; j < 25 && i < avail; i++) {
        if (Serial2.available() > 0) {
            channel[j++] = Serial2.read();        // raw data
        }
    }

    if (j == 25) {          
        Serial.print("Channels");
        for(uint8_t i=0; i < 25; i++) {
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
