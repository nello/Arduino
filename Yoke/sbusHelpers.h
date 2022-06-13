#ifndef SBUS_HELPERS
#define SBUS_HELPERS

#include <Arduino.h>
#include "sbus.h"

namespace SBUS {
  #define CHANNELS 16
  
  /* radio definitions */
  typedef struct {
    int low;
    int high;
  } Radio;
  
  Radio Taranis { 988, 2012 };
  Radio Frsky { 172, 1811 };
  Radio Tx16s { 172, 1811 };
  
  /* create SBUS transmitter */
  bfs::SbusTx sbus_tx(&Serial1);
  
  /* SBUS output channels */
  std::array<int16_t, CHANNELS> sbus_channels;

  
  /* _scale_for_radio() - scales input values in range [0..1) for our radio */
  int _scale_for_radio(int value, Radio radio=Tx16s, int step=255) {
    return ((int)(value * (radio.high - radio.low) / step) + radio.low) & 0x7ff;
  }

  
  /* sbus_init() - start SBUS */
  void sbus_init() {
    sbus_tx.Begin(false);  // output - UNINVERTED!
  }

  
  /* sbus_send() - send 16 channels of data to the radio */
  void sbus_send(int16_t channels[]) {   
    /*    
    Serial.print("Channels");
    for(uint8_t i=0; i < CHANNELS; i++) {
        Serial.print(" ");
        Serial.print(i);
        Serial.print(":");
        Serial.print(channels[i], DEC);
    }
    Serial.println();
    */
  
    for (uint8_t i=0; i < CHANNELS; i++) {
        sbus_channels[i] = _scale_for_radio(channels[i], Tx16s);
    }
  
    sbus_tx.ch(sbus_channels);
    sbus_tx.Write();
  }
}

#endif // SBUS_HELPERS
