#include <Arduino.h>
#include <smartblocks.h>

Smartblocks *sb;

void setup() {
  Serial.begin(115200);

  if(Serial) Serial.println("Trying to setup connection.");
  sb = new Smartblocks();
  if(Serial) Serial.println("Starting program.");
}


const uint16_t delayTime = 2000;

void loop() {
  
  uint64_t now = millis();
  static uint64_t lastRun = 0;

  if(now >= lastRun + delayTime){

    sb->write("random", String(random(-100, 101)));


    lastRun = now;
  }

}