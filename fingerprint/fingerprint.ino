#include <Adafruit_Fingerprint.h>

#define mySerial Serial1

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  delay(100);

  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  mySerial.setTX(12);
  mySerial.setRX(13);
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
}

void loop()  // run over and over again
{
  int fingerprintID = getFingerprintID();
  Serial.println(fingerprintID);
  if(fingerprintID < 0){
    //no match
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    delay(1000);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
  }
  else if(fingerprintID > 0){
    //match
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
    delay(1000);
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
  }
  // else{
  //   digitalWrite(2, HIGH);
  //   digitalWrite(3, HIGH);
  // }
  delay(50);  //don't ned to run this at full speed.
}

int getFingerprintID() {

  while (!(finger.getImage() == FINGERPRINT_OK && finger.image2Tz() == FINGERPRINT_OK))
    ;

  int p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return -1;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return -1;
  }

  if(finger.fingerID > 0 && finger.fingerID < 255){
    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID;
  }


  return 0;
}
