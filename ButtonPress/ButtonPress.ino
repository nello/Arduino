#include <vprintln.h>

#define BUTTON_PIN 34

void setup() {
  Serial.begin(112500);
  delay(1000);
 
  xTaskCreate(
                    taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
 
  xTaskCreate(
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
 
}
 
void loop() {
  delay(1000);
}
 
void taskOne(void *parameter)
{
    bool lastState = HIGH;
    
    vprintln("Reading button");
    pinMode (BUTTON_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
    
    while (1) {
        bool currentState = digitalRead(BUTTON_PIN);
        bool currentMillis = millis();
        
//        vprintln("currentState: ", currentState);

        if (currentState == HIGH && lastState == LOW) {
            Serial.println("CLICK");
        }
        
        lastState = currentState;
        delay(5);
    }
}
 
void taskTwo(void *parameter)
{
    for(int i = 0; i < 10; i++){
        Serial.println("Hello from task 2");
        delay(1000);
    }
    
    Serial.println("Ending task 2");
    vTaskDelete( NULL );
}
