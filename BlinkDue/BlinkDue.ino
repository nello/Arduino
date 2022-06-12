/*
 HIDMouse
*/

#include <MouseController.h>

// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
MouseController mouse(usb);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial1.begin(19200);
  Serial.println("Bing!");
  Serial1.println("Bong!");
}

void mouseMoved() {
  Serial.print("MOVE: ");
  Serial.print(mouse.getXChange());
  Serial.print(", ");
  Serial.println(mouse.getYChange());
}

void mouseDragged() {
  Serial.print("DRAG: ");
  Serial.print(mouse.getXChange());
  Serial.print(", ");
  Serial.println(mouse.getYChange());
}

void mousePressed(){
  if (mouse.getButton(LEFT_BUTTON))
    Serial.print("LEFT");
  if (mouse.getButton(MIDDLE_BUTTON))
    Serial.print("MIDDLE");
  if (mouse.getButton(RIGHT_BUTTON))
    Serial.print("RIGHT");
}

void mouseReleased(){
  Serial.print("RELEASE");
}

void loop() {
  usb.Task();
}
