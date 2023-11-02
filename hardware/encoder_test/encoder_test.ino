#include "pio_encoder.h"

PioEncoder encoder(2); // encoder is connected to GPIO2 and GPIO3

void setup() {
  encoder.begin();
  Serial.begin(115200);
  while(!Serial);

}

void loop() {
  Serial.println(encoder.getCount());
  delay(10);
}
