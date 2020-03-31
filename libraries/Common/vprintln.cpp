#include <vprintln.h>

void _variad(size_t argc, ...) {
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
  
  Serial.println();
  va_end(ap);
}

void sprintln(int count, ...) {
  va_list argv;
  va_start(argv, count);

  for(int i=0; i<count; ++i) {
    int n = va_arg(argv, int);
    if (n > 2000000) {
      Serial.print((char *)n);
    } else {
      Serial.print(n);
    }
  }
  Serial.println();
  va_end(argv);
}
