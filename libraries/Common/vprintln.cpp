#include <vprintln.h>

void _vprint(size_t argc, ...) {
  va_list ap;
  va_start(ap, argc);
  
  for(int i=0; i<argc; ++i) {
    int n = va_arg(ap, int);
    if (n > 2000000) {
      Serial.print((char *)n);
    } else {
      Serial.print(n);
    }
  }
  va_end(ap);
}

