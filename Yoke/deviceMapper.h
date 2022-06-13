#ifndef DEVICE_MAPPER
#define DEVICE_MAPPER

namespace Map {
  typedef struct {
    char const *name;
    int input;
    int low;
    int high;
    int output;
  } Mapping;

  typedef struct {
    char const *name;
    uint16_t vendor_id;
    uint16_t product_id;
    Mapping mappings[10];   // just some limit
  } Device;
  
  Device yoke = {
    "Saitek Pro Flight Yoke", 0x6a3, 0xbac, {
      {"up", 1, 512, 1023, 3},
      {"down", 1, 512, 0, 3},
      {"left", 0, 512, 0, 2},
      {"right", 0, 512, 1023, 2}
    }
  };

  Device pedals = {
    "Fanatec ClubSport USB Pedal", 0xeb7, 0x1a95, {
      {"acceleration", 0, 0, 255, 0},
      {"braking", 2, 0, 255, 1}
    }
  };

  Device ps4 = {
    "Sony PS4 Controller", 0x54c, 0x5c4, {
      {"acceleration", 4, 0, 255, 0},
      {"braking", 3, 0, 255, 1},
      {"left", 2, 134, 0, 2},
      {"right", 2, 137, 255, 2},
      {"up", 5, 130, 255, 3},
      {"down", 5, 127, 0, 3}
    }
  };

  Device devices[] = { yoke, pedals, ps4 };

  Device *find_device(uint16_t vendor_id, uint16_t product_id) {
    int n = sizeof(devices)/sizeof(Device);
    Serial.printf("%d devices\n", n);
    
    for (int i=0; i < n; ++i) {
      Device* d = &devices[i];
      if (d->vendor_id == vendor_id && d->product_id == product_id) {
        return d;
      }
    }
    return NULL;
  }

  // scale all values from 0..1023
  void map_device(Device* device, int16_t channels[], int input, int value) {
    for (int i=0; device->mappings[i].name != NULL; ++i) {
      Mapping* mapping = &device->mappings[i];
      if (mapping->input == input) {
        channels[mapping->output] = (value - mapping->low) * 1023 / (mapping->high - mapping->low);
      }
    }
  }
}

#endif // DEVICE_MAPPER
