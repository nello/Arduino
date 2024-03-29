//
// USB to SBUS device mapper
//

#ifndef DEVICE_MAPPER
#define DEVICE_MAPPER

#include "sbusHelpers.h"

#define MIN(x, y)     (((x) < (y)) ? (x) : (y))
#define MAX(x, y)     (((x) > (y)) ? (x) : (y))
#define ABS(x)        (((x) < 0) ? -(x) : (x))
#define DEFAULT(x, y) (((x) == 0) ? (y) : (x))    

namespace Mapper {
  #define RESOLUTION    1024
  #define RESOLUTION_1  ((RESOLUTION) - 1)
  
  typedef struct {
    char const *name;       // name of input
    int input;              // USB input channel
    int low;                // start of range for input values
    int high;               // end of range for input values
    int output;             // SBUS output channel
    int resolution;         // SBUS resolution for this output component (defaults to 1023)
    int offset;             // zero value for SBUS channel (defaults to 0)
  } Mapping;

  typedef struct {
    char const *name;       // device name
    uint16_t vendor_id;     // vendor id we will match to identify device
    uint16_t product_id;    // product id we will match to identify device
    Mapping mappings[10];   // max number of mappings for this device (arbitrary)
  } Device;
  
  Device yoke = {
    "Saitek Pro Flight Yoke", 0x6a3, 0xbac, {
      {"up", 1, 512, 1023, 3, 511, 512},
      {"down", 1, 512, 0, 3, 512, 512},
      {"left", 0, 512, 0, 2, 512, 512},
      {"right", 0, 512, 1023, 2, 511, 512},
      {"black", 2, 255, 0, 6, 1023, 0},
      {"red", 3, 255, 0, 7, 1023, 0},
      {"blue", 4, 255, 0, 8, 1023, 0},
    }
  };

  Device pedals = {
    "Fanatec ClubSport USB Pedal", 0xeb7, 0x1a95, {
      {"acceleration", 0, 0, 255, 0},
      {"braking", 2, 0, 255, 1},
    }
  };

  Device ps4 = {
    "Sony PS4 Controller", 0x54c, 0x5c4, {
      {"acceleration", 4, 0, 245, 0},
      {"braking", 3, 0, 255, 1},
      {"left", 2, 133, 0, 2, 512, 512},
      {"right", 2, 138, 255, 2, 511, 512},
      {"up", 1, 125, 255, 3, 511, 512},
      {"down", 1, 120, 0, 3, 512, 512},
    }
  };

  Device joystick = {
    "Default Joystick (Logitech Attack 3)", 0x46d, 0xc214, {
      {"up", 1, 130, 255, 3, 511, 512},
      {"down", 1, 126, 0, 3, 512, 512},
      {"left", 0, 126, 0, 2, 512, 512},
      {"right", 0, 130, 255, 2, 511, 512},
    }
  };

  Device devices[] = { yoke, pedals, ps4 };

  Device *find_device(uint16_t vendor_id, uint16_t product_id) {
    int num_devices = sizeof(devices)/sizeof(Device);
    //Serial.printf("%d devices\n", n);
    
    for (int i=0; i < num_devices; ++i) {
      Device* device = &devices[i];
      if (device->vendor_id == vendor_id && device->product_id == product_id) {
        return device;
      }
    }

    // default
    return &joystick;
  }

  // scale all values from 0..RESOLUTION-1
  void map_outputs(Device* device, int16_t outputs[], int input, int value) {
    // zero output channels
    for (int i=0; device->mappings[i].name != NULL; ++i) {
      Mapping* mapping = &device->mappings[i];
      if (mapping->input == input && mapping->output < SBUS::NUM_CHANNELS) {
        if (MIN(mapping->low, mapping->high) <= value && value <= MAX(mapping->low, mapping->high)) {
          outputs[mapping->output] = mapping->offset;
        } else if (outputs[mapping->output] == 0) {
          outputs[mapping->output] = mapping->offset;
        }
      }
    }

    // add scaled outputs
    for (int i=0; device->mappings[i].name != NULL; ++i) {
      Mapping* mapping = &device->mappings[i];
      if (mapping->input == input && mapping->output < SBUS::NUM_CHANNELS) {
        // scale values by range and resolution
        if (MIN(mapping->low, mapping->high) <= value && value <= MAX(mapping->low, mapping->high)) {
          outputs[mapping->output] += (value - mapping->low) * DEFAULT(mapping->resolution, RESOLUTION_1) / ABS(mapping->high - mapping->low);
        }

        // flip any negative values
        outputs[mapping->output] = ABS(outputs[mapping->output]);
      }
    }
  }
}

#endif // DEVICE_MAPPER
