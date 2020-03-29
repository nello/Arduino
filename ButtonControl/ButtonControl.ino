#include <vprintln.h>

// controls
#define BUTTON_PIN 34
#define POT_PIN 35
#define CHANNEL_COUNT 3

// setting PWM properties
#define LED_PIN 16    // 16 corresponds to GPIO16
#define FQ 5000
#define LED_CHANNEL 0
#define RESOLUTION 8

// current channel being controlled by pot
volatile int channelNumber = 0;

void setup() {
  Serial.begin(112500);
  delay(1000);

  // configure LED PWM details
  for (int i=0; i < CHANNEL_COUNT; i++) {
    ledcSetup(LED_CHANNEL + i, FQ, RESOLUTION);
    ledcAttachPin(LED_PIN + i, LED_CHANNEL + i);
  }

  // create tasks
  xTaskCreate(
                    buttonTask,       /* Task function. */
                    "ButtonTask",     /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
 
  xTaskCreate(
                    potTask,          /* Task function. */
                    "PotTask",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
 
}
 
void loop() {
  delay(1000);
}
 
void buttonTask(void *parameter)
{
    bool lastState = HIGH;
    
    pinMode(BUTTON_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
    
    while (1) {
        bool currentState = digitalRead(BUTTON_PIN);
        bool currentMillis = millis();

        if (currentState == HIGH && lastState == LOW) {
            channelNumber = (channelNumber + 1) % CHANNEL_COUNT;
            vprintln("CHANNEL ", channelNumber);
        }
        lastState = currentState;
        delay(5);
    }
}
 
void potTask(void *parameter)
{
    int lastPotValue = 0;
    const int MARGIN = 5;

    pinMode(POT_PIN, INPUT);
    
    while (1) {
        int potValue = analogRead(POT_PIN) >> 4;
        
        if (potValue < lastPotValue - MARGIN || potValue > lastPotValue + MARGIN) {
            vprintln("CHANNEL ", channelNumber, ": ", potValue);
            ledcWrite(channelNumber, potValue);
            lastPotValue = potValue; 
        }
        delay(5);
    }
}