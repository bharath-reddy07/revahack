#include "pio_encoder.h"

PioEncoder encoder(2); // encoder is connected to GPIO2 and GPIO3

void setup() {
  encoder.begin();
  Serial.begin(115200);
  pinMode(4, INPUT_PULLUP);
  while(!Serial);

}

void loop() {
  Serial.println(encoder.getCount()/2);
  int mil = millis();
  if(!digitalRead(4) && millis() - mil <200){
    Serial.println("Click");
    delay(500);
  }

  delay(100);
} 
