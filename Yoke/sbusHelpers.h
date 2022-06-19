#ifndef SBUS_HELPERS
#define SBUS_HELPERS

#include <Arduino.h>
#include "sbus.h"

namespace SBUS {
  /* SBUS transmitter */
  bfs::SbusTx *sbus_tx;
  
  /* SBUS output channels */
  const int NUM_CHANNELS = bfs::SbusTx::NUM_CH();
  int16_t channels[NUM_CHANNELS] = { 0 };
  std::array<int16_t, NUM_CHANNELS> _output;
  
  /* radio definitions */
  typedef struct {
    int low;
    int high;
  } Radio;
  
  Radio Taranis { 988, 2012 };
  Radio Frsky { 172, 1811 };
  Radio Tx16s { 172, 1811 };

  
  /* _scale_for_radio() - scales input values in range [0..1023) for our radio */
  int _scale_for_radio(int value, Radio radio=Tx16s, int step=1023) {
    return ((int)(value * (radio.high - radio.low) / step) + radio.low) & 0x7ff;
  }

  
  /* sbus_init() - start SBUS */
  void sbus_init(HardwareSerial *serial) {
    serial->begin(2000000);
    sbus_tx = new bfs::SbusTx(serial);
    sbus_tx->Begin(false);  // UNINVERTED!
  }

  
  /* sbus_send() - send 16 channels of data to the radio */
  void sbus_send(bool changed_only=false) {   
    bool changed = false;
    
    for (uint8_t i=0; i < NUM_CHANNELS; i++) {
      int16_t scaled_value = _scale_for_radio(channels[i], Tx16s);
      if (_output[i] != scaled_value) {
        changed = true;
      }
      _output[i] = scaled_value;
    }

    if (!changed_only || changed) {
      sbus_tx->ch(_output);
      sbus_tx->Write();
    }
  }

  /* sbus_print() - print it */
  void sbus_print() {
    Serial.print("\nSBUS: ");
    for (int i = 0; i < NUM_CHANNELS; ++i) {
      Serial.printf(" %d:%d", i, channels[i]);
    }
    Serial.println("");
  }
}

#endif // SBUS_HELPERS
