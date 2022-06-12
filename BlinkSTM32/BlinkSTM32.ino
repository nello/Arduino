/*
  BlinkSTM32
*/
//include <uart.h>

// the setup function runs once when you press reset or power the board
void setup() {
//  HardwareSerial Serial2(USART2);
//  HardwareSerial Serial3(USART3);

  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PC13, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(19200);
} 

// the loop function runs over and over again forever
void loop() {
  digitalWrite(PC13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(PC13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  Serial.println("Bing!");
  Serial1.println("Bong!");
}
