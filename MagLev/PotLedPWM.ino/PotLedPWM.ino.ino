// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int POT_PIN = 34;
const int LED_PIN = 16;  // 16 corresponds to GPIO16

// setting PWM properties
const int FQ = 5000;
const int LED_CHANNEL = 0;
const int RESOLUTION = 8;

void setup() {
  Serial.begin(115200);
  
  // configure LED PWM functionalitites
  ledcSetup(LED_CHANNEL, FQ, RESOLUTION);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(LED_PIN, LED_CHANNEL);
  
  delay(1000);
}

void loop() {
  // Reading potentiometer value
  int potValue = analogRead(POT_PIN) * 256 / 4096;
  ledcWrite(LED_CHANNEL, potValue);
  Serial.println(potValue);
  delay(50);
}
