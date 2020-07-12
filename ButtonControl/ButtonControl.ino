#include <vprintln.h>

// controls
#define BUTTON_PIN 33
#define POT_PIN 35
#define CHANNEL_COUNT 5

// setting PWM properties
#define LED_PIN 14    // GPIO14 (GPIO13 is broken)
#define FQ 3000
#define LED_CHANNEL 0
#define RESOLUTION 8

// config
#define H_BRIDGE true

// current channel being controlled by pot
volatile int channelNumber = 0;
int channelValues[CHANNEL_COUNT];

void setup() {
    Serial.begin(112500);
    delay(1000);
  
    // configure LED PWM details
    if (H_BRIDGE) {
        for (int i=0; i < (CHANNEL_COUNT * 2); i++) {
            ledcSetup(LED_CHANNEL + i, FQ, RESOLUTION);
            ledcAttachPin(LED_PIN + i, LED_CHANNEL + i);
        }
    } else {
        for (int i=0; i < CHANNEL_COUNT; i++) {
            ledcSetup(LED_CHANNEL + i, FQ, RESOLUTION);
            ledcAttachPin(LED_PIN + i, LED_CHANNEL + i);
        }
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

void report() {
  vprint("[");
  for (int i=0; i < CHANNEL_COUNT-1; ++i) { 
      if (channelNumber == i) vprint("*");
      vprint(channelValues[i], ", "); 
  }
  if (channelNumber == CHANNEL_COUNT-1) vprint("*");
  vprintln(channelValues[CHANNEL_COUNT-1], "]");
}
 
void buttonTask(void *parameter) {
    bool lastState = HIGH;
    
    pinMode(BUTTON_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP); // doesn't work for pins 34 and above
    
    while (1) {
        bool currentState = digitalRead(BUTTON_PIN);
        bool currentMillis = millis();

        if (currentState == HIGH && lastState == LOW) {
            channelNumber = (channelNumber + 1) % CHANNEL_COUNT;

            report();
        }
        lastState = currentState;
        delay(10);
    }
}
 
void potTask(void *parameter) {
    int lastPotValue = 0;
    const int MARGIN = 5;

    pinMode(POT_PIN, INPUT);

    if (H_BRIDGE) {
        while (1) {
            int potValue = (analogRead(POT_PIN) >> 3) - 255;
            if (potValue > 255) potValue = 255;

            if (potValue < lastPotValue - MARGIN || potValue > lastPotValue + MARGIN) {                
                if (potValue > 0) {
                    ledcWrite(2 * channelNumber, 0);
                    ledcWrite(2 * channelNumber + 1, potValue);
                } else {
                    ledcWrite(2 * channelNumber, -potValue);
                    ledcWrite(2 * channelNumber + 1, 0);
                }
                
                channelValues[channelNumber] = potValue;
                report();
                lastPotValue = potValue; 
            }
            delay(10);
        }
    } else {
        while (1) {
            int potValue = analogRead(POT_PIN) >> 4;
            
            if (potValue < lastPotValue - MARGIN || potValue > lastPotValue + MARGIN) {
                ledcWrite(channelNumber, potValue);
                
                channelValues[channelNumber] = potValue;
                report();
                lastPotValue = potValue; 
            }
            delay(10);
        }
        
    }
}
