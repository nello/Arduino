//
// Yoke + pedals USB -> SBUS (Teensy 4.1)
//

#include "USBHost_t41.h"
#include "ps4Helpers.h"
#include "sbusHelpers.h"
#include "deviceMapper.h"

#define LED 13

USBHost myusb;

#define COUNT_HUBS 1
USBHub hubs[COUNT_HUBS](myusb);

#define COUNT_JOYSTICKS 4
JoystickController joysticks[COUNT_JOYSTICKS](myusb);

#define COUNT_HIDS 4
USBHIDParser hids[COUNT_HIDS](myusb);

#define COUNT_DEVICES (COUNT_HUBS + COUNT_JOYSTICKS + COUNT_HIDS)
USBDriver *drivers[COUNT_DEVICES] = {};
const char *driver_names[COUNT_DEVICES] = {};
bool driver_active[COUNT_DEVICES] = {false};

uint8_t joystick_left_trigger_value[COUNT_JOYSTICKS] = {0};
uint8_t joystick_right_trigger_value[COUNT_JOYSTICKS] = {0};
uint64_t joystick_full_notify_mask = (uint64_t) - 1;

int user_axis[64];
uint32_t buttons_prev = 0;
bool show_changed_only = false;

/* setup() */
void setup()
{  
  // wait for Arduino serial monitor
  //while (!Serial) {}
  Serial.println("USB to SBUS");

  // prepare outputs
  SBUS::sbus_init(&Serial1);        // SBUS output on Serial1
  Serial2.begin(115200);            // UART output on Serial2
  pinMode(LED, OUTPUT);             // blinking light

  // prepare hid drivers
  char driver_name[16];
  for (int i=0; i < COUNT_HUBS; ++i) {
    sprintf(driver_name, "hub[%d]", i);
    drivers[i] = &hubs[i]; 
    driver_names[i] = strdup(driver_name);
  }
  for (int i=0; i < COUNT_JOYSTICKS; ++i) {
    sprintf(driver_name, "joystick[%d]", i);
    drivers[i+COUNT_HUBS] = &joysticks[i]; 
    driver_names[i+COUNT_HUBS] = strdup(driver_name);
  }
  for (int i=0; i < COUNT_HIDS; ++i) {
    sprintf(driver_name, "hid[%d]", i);
    drivers[i+COUNT_HUBS+COUNT_JOYSTICKS] = &hids[i];
    driver_names[i+COUNT_HUBS+COUNT_JOYSTICKS] = strdup(driver_name);
  }

  // prepare input
  myusb.begin();                    // USB Host input
}

/* processDeviceListChanges() - info only */
void processDeviceListChanges() {
  for (uint8_t i = 0; i < COUNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial.printf("*** Device %s %x:%x - disconnected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = false;
      } else {
        Serial.printf("*** Device %s %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  serial: %s\n", psz);
      }
    }
  }
}

/* processJoystickInputChanges() - we only use joystick bits of this right now, buttons to come */
void processJoystickInputChanges() {  
  for (int joystick_index = 0; joystick_index < COUNT_JOYSTICKS; joystick_index++) {
    if (joysticks[joystick_index].available()) {
      uint64_t axis_mask = joysticks[joystick_index].axisMask();
//      uint64_t axis_changed_mask = joysticks[joystick_index].axisChangedMask();
      uint32_t buttons = joysticks[joystick_index].getButtons();
      
      Serial.printf("Joystick(%d): buttons = %x", joystick_index, buttons);
      //Serial.printf(" AMasks: %x %x:%x", axis_mask, (uint32_t)(user_axis_mask >> 32), (uint32_t)(user_axis_mask & 0xffffffff));
      //Serial.printf(" M: %lx %lx", axis_mask, joysticks[joystick_index].axisChangedMask());

      Mapper::Device* device = Mapper::find_device(joysticks[joystick_index].idVendor(), joysticks[joystick_index].idProduct());
      if (device) {       
        Serial.printf("\nDEVICE MAPPER: %s\n", device->name);
        for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
          if (axis_mask & 1) {
            int value = joysticks[joystick_index].getAxis(i);
            Serial.printf(" %d:%d", i, value);
            Mapper::map_outputs(device, SBUS::channels, i, value);
          }
        }

        // send changes
        if (SBUS::sbus_send(true)) {
          SBUS::sbus_print();       // tell us what was sent
          SBUS::uart_send(&Serial); // sent to UART
        }
      }

      /*
      if (show_changed_only) {
        for (uint8_t i = 0; axis_changed_mask != 0; i++, axis_changed_mask >>= 1) {
          if (axis_changed_mask & 1) {
            Serial.printf(" %d:%d", i, joysticks[joystick_index].getAxis(i));
          }
        }
      } else {
        // default
        for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
          if (axis_mask & 1) {
            Serial.printf(" %d:%d", i, joysticks[joystick_index].getAxis(i));
          }
        }
      }
      */
      /*
      // game controllers
      for (uint8_t i = 0; i<64; i++) {
          PS4::psAxis[i] = joysticks[joystick_index].getAxis(i);
      }
      switch (joysticks[joystick_index].joystickType()) {
        uint8_t ltv;
        uint8_t rtv;
      
        default:
          break;
          
        case JoystickController::PS4:
          PS4::printAngles();
          ltv = joysticks[joystick_index].getAxis(3);
          rtv = joysticks[joystick_index].getAxis(4);
          if ((ltv != joystick_left_trigger_value[joystick_index]) || (rtv != joystick_right_trigger_value[joystick_index])) {
            joystick_left_trigger_value[joystick_index] = ltv;
            joystick_right_trigger_value[joystick_index] = rtv;
            //joysticks[joystick_index].setRumble(ltv, rtv); <- works, but is just annoying
            //Serial.printf(" Set Rumble %d %d", ltv, rtv);
          }
          break;

        case JoystickController::PS3:
          ltv = joysticks[joystick_index].getAxis(18);
          rtv = joysticks[joystick_index].getAxis(19);
          if ((ltv != joystick_left_trigger_value[joystick_index]) || (rtv != joystick_right_trigger_value[joystick_index])) {
            joystick_left_trigger_value[joystick_index] = ltv;
            joystick_right_trigger_value[joystick_index] = rtv;
            joysticks[joystick_index].setRumble(ltv, rtv,50);
          }
          break;

        case JoystickController::XBOXONE:
        case JoystickController::XBOX360:
          ltv = joysticks[joystick_index].getAxis(3);
          rtv = joysticks[joystick_index].getAxis(4);
          if ((ltv != joystick_left_trigger_value[joystick_index]) || (rtv != joystick_right_trigger_value[joystick_index])) {
            joystick_left_trigger_value[joystick_index] = ltv;
            joystick_right_trigger_value[joystick_index] = rtv;
            joysticks[joystick_index].setRumble(ltv, rtv,50);
            Serial.printf(" Set Rumble %d %d", ltv, rtv);
          }
          break;
      }
      */
      
      // buttons
      if (buttons != buttons_prev) {
        if (joysticks[joystick_index].joystickType() == JoystickController::PS3) {
          //joysticks[joystick_index].setLEDs((buttons >> 12) & 0xf); //  try to get to TRI/CIR/X/SQuare
          uint8_t leds = 0;
          if (buttons & 0x8000) leds = 1;   //Srq
          if (buttons & 0x2000) leds = 2;   //Cir
          if (buttons & 0x1000) leds = 4;   //Tri
          if (buttons & 0x4000) leds = 8;   //X  //Tri
          joysticks[joystick_index].setLEDs(leds);
        } else {
          uint8_t lr = (buttons & 1) ? 0xff : 0;
          uint8_t lg = (buttons & 2) ? 0xff : 0;
          uint8_t lb = (buttons & 4) ? 0xff : 0;
          joysticks[joystick_index].setLEDs(lr, lg, lb);
        }
          buttons_prev = buttons;
      }

      Serial.println();
      joysticks[joystick_index].joystickDataClear();
    } 
  }
}

/* main() */
int tick = 0;
void loop()
{
  // process USB inputs
  myusb.Task();
  processDeviceListChanges();
  processJoystickInputChanges();

  // wait 10ms
  delay(10);

  // SBUS keep-alive (100ms)
  if (tick % 10 == 0) {
     SBUS::sbus_send();
  }

  // flash led (500ms)
  if (tick == 50) {
    digitalWrite(LED, HIGH);
  } else if (tick == 100) {
    digitalWrite(LED, LOW);
    tick = 0;
  }
  
  tick++;
}
