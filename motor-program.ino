#include "Main.h"

void setup() {
  Serial.begin(9600);

  Runnable::setupAll();
  Main::setup();
}

void loop() {
  Runnable::loopAll();
  Main::loop();
}
