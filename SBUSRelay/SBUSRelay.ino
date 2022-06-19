#include <Arduino.h>
#include "sbus.h"

#define CHANNELS 16
//#define SCALE_BASE=988         // Taranis
//#define SCALE_TOP=2012         // Taranis
#define SCALE_BASE 172           // FrSky, TX16S
#define SCALE_TOP 1811           // FrSky, TX16S
#define SCALE_RANGE (SCALE_TOP - SCALE_BASE)


//int16_t channel[CHANNELS] = {0};
std::array<int16_t, CHANNELS> channel;
int j;

/* SbusTx object on Serial1 */
bfs::SbusTx sbus_tx(&Serial1);

int scale(int value) {
  return ((int)(value * SCALE_RANGE / 255) + SCALE_BASE) & 0x7ff;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
#if defined(ESP32)  
  sbus_tx.Begin(18, 19, false);  // output - UNINVERTED!
#else
  sbus_tx.Begin(false);          // works for Teensy (now)
#endif

  Serial2.begin(100000);    // RX2
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
        /*    
        Serial.print("Channels");
        for(uint8_t i=0; i < CHANNELS; i++) {
            Serial.print(" ");
            Serial.print(i);
            Serial.print(":");
            Serial.print(channel[i], DEC);
        }
        Serial.println(); */
        j = 0;

        for (uint8_t i=0; i < CHANNELS; i++) {
            channel[i] = scale(channel[i]);
        }

        /* Set the SBUS TX data to the received data */
        sbus_tx.ch(channel);
            
        /* Write the data to the servos */
        sbus_tx.Write();
    }
  }
}
