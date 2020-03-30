#include <Arduino.h>
#include <smartblocks.h>

#define led 13

Smartblocks *sb;

void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);

  if(Serial) Serial.println("Trying to setup connection.");
  sb = new Smartblocks();
  if(Serial) Serial.println("Starting program.");
}


const uint16_t delayTime = 2000;

void loop() {
  
  uint64_t now = millis();
  static uint64_t lastRun = 0;

  if(now >= lastRun + delayTime){

    if(sb->read("led") == "1"){
      digitalWrite(led, true);
    } else {
      digitalWrite(led, false);
    }

    lastRun = now;
  }
  
}