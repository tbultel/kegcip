#include <Arduino.h>


extern "C" {
__attribute__((weak)) int _write(int file, char *ptr, int len) {
  // STDIN: do nothing
  if (file == 0) ;
  // STDOUT/STDERR: Output to Serial
  else if (file == 1 || file == 2)
    Serial.write((uint8_t *)ptr, len);

  // Otherwise assume file is actually a pointer to a Print object and output to that.
  else
    ((class Print *)file)->write((uint8_t *)ptr, len);

  return 0;
}

}