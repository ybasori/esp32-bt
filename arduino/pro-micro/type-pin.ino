#include <Keyboard.h>

void setup() {
  delay(20000); // wait for Windows boot/login screen

  Keyboard.begin();

  // Wake/login screen
  Keyboard.write(KEY_RETURN);

  delay(1000);

  // Type PIN
  Keyboard.print("1234");

  delay(300);

  // Submit
  Keyboard.write(KEY_RETURN);

  Keyboard.end();
}

void loop() {
}
