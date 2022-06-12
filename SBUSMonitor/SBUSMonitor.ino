#include "sbus.h"

/* SbusRx object on Serial1 */
bfs::SbusRx sbus_rx(&Serial1);
/* Array for storing SBUS data */
std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  
  /* Begin the SBUS communication */
  sbus_rx.Begin(16, 17, false);   // UNINVERTED
}

void loop() {
  if (sbus_rx.Read()) {
    /* Grab the received data */
    sbus_data = sbus_rx.ch();
    
    /* Display the received data */
    for (int8_t i = 0; i < bfs::SbusRx::NUM_CH(); i++) {
      Serial.print(sbus_data[i]);
      Serial.print("\t");
    }
    /* Display lost frames and failsafe data */
    Serial.print(sbus_rx.lost_frame());
    Serial.print("\t");
    Serial.println(sbus_rx.failsafe());
  }
}
