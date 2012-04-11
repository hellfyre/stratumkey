#include <sha256.h>

int main(void) {
  uint8_t dest[32];
  char *message = "foo";

  sha256(&dest, message, 24);
}
