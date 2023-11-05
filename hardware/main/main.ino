#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"
#include <string.h>
#include <Crypto.h>
#include <SHA256.h>
#include <AES.h>
#include <string.h>
#include "Adafruit_EEPROM_I2C.h"
#include "Adafruit_FRAM_I2C.h"
#include "pio_encoder.h"
#include <Adafruit_Fingerprint.h>
#include <Keyboard.h>
#include "RTClib.h"

RTC_DS1307 rtc;

#define mySerial Serial1

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

PioEncoder encoder(2);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCROLL_SPEED 1
#define WIDTH_IN_CHARS 10

#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define EEPROM_ADDR 0x50
#define KEY_SIZE 16

Adafruit_EEPROM_I2C i2ceeprom;

#define HASH_SIZE 32
#define KEY_SIZE 16

byte key_hash[HASH_SIZE];
byte decryptedText[KEY_SIZE];

SHA256 sha256;
AES256 aes256;

struct cipherVector {
  char name[16];          //name of id, password
  char id[32];            //contains userid or email
  byte cipher[KEY_SIZE];  //encrypted password
};

cipherVector const cipher1 = {
  //to test master password
  "Test",
  "123456789012345",
  { 0x5D, 0x16, 0x26, 0x6E, 0x10, 0xB8, 0xE5, 0xFE,
    0x76, 0x4C, 0x3A, 0x2B, 0xD6, 0x80, 0xAE, 0xF6
  }  //123456789012345
};

byte buffer[sizeof(cipherVector)];
cipherVector f;

cipherVector data[64];

void copyArray(char array1[100][32], char array2[100][32], int arraySize);

char userInput[32];

int numItems = 1;
int state = 0;
char menuItems[100][32] = { "Passwords", "Long message", "Very long message", "Very very long message", "short" };

char tempPass[16] = "";
uint8_t numBlocks = 0;

int numState0 = 1;
char state0[1][32] = { "Place your finger to auth" };

int numState1 = 1;
char state1[1][32] = {""};
;
char state1Input[32];


int numState2 = 1;
char state2[1][32] = { "Enter OTP" };

int numState3 = 5;
char state3[5][32] = { "Login", "Add", "Backup", "Reset", "Exit" };
int value = 0;

int numState4 = 0;
char state4[100][32] = {};

int numState5 = 1;
char state5[1][32] = { "Enter website name" };

int numState6 = 39;
char state6[1][32] = { "" };

int numState7 = 1;
char state7[1][32] = { "Enter user name" };

int numState8 = 39;
char state8[1][32] = { "" };
int numState9 = 1;
char state9[1][32] = { "Enter password" };

int numState10 = 39;
char state10[1][32] = { "" };

int numState11 = 1;
char state11[1][32] = { "Password saved" };
/*
  0 "Enter master pass"
  1 Take master pass input. onclick check master
  2 "Incorrect" (one more variable for keeping track of number of incorrect)
  3 "login", "add", "backup", "reset", "exit"
  4 website name (login menu)
  5 "enter website"
  6 take website
  7 "enter uname"
  8 take uname input
  9 "enter pass"
  10 take pass

*/
bool testMaster(AES256* aes256, byte* key_hash, const struct cipherVector* testCipher) {
  //test
  aes256->decryptBlock(decryptedText, testCipher->cipher);
  bool valid = true;
  for (int i = 0; i < KEY_SIZE; i++) {
    valid &= decryptedText[i] == testCipher->id[i];
    Serial.println(decryptedText[i] + ": ");
  }
  Serial.print("Master password is ");
  Serial.print(int(valid));
  return valid;
}
int redPin = 16;
int greenPin = 17;
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  Keyboard.begin();
  pinMode(redPin, OUTPUT);  //red
  pinMode(greenPin, OUTPUT);  // green
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);

  // set the data rate for the sensor serial port
  mySerial.setTX(12);
  mySerial.setRX(13);
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }
//  while (!Serial);

  setupDisplay();

  //setupRotaryEncoder();
  setupFlash();
  if (!rtc.begin(&Wire)) {
    Serial.println("Couldn't find RTC");
    return;
  }
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("RTC Initialized");
  encoder.begin();
  pinMode(4, INPUT_PULLUP);
  copyArray(menuItems, state0, numState0);
  delay(2000);
}



void loop() {
  //  Serial.print("TIME: ");
  //  Serial.println(getCurrentTime());

//  buffer[0] = 0;
//  buffer[1] = 0;
//  i2ceeprom.write((uint16_t)0, buffer, 2);

  //Serial.println(menuItems[value]);
  displayMessage(menuItems[value]);
  runCode();
  rotary_loop();
  delay(50);
}
void runCode()
{
  if (state == 1)
  {
    display.clearDisplay();
    delay(100);
    handleChange();
  }
}
void handleChange() {
  if (state == 0) {
    // takeInput();  //takes input and puts it in userInput
    updateState(1);
    return;
  }
  if (state == 1) {
    int fingerprintID = getFingerprint();
    if (fingerprintID > 0) {
      updateState(2);
      return;
    }
    else {
      updateState(0);
      return;
    }
    // Serial.println("CHECKING pass");
    // //Check Master Password
    // sha256.reset();
    // sha256.update(userInput, strlen(userInput));
    // sha256.finalize(key_hash, sizeof(key_hash));
    // aes256.setKey(key_hash, aes256.keySize());

    // byte ecrt[16];
    // char passwrd[16];
    // strcpy(passwrd, "lmao");
    // aes256.encryptBlock(f.cipher, (uint8_t*)passwrd);


    // if (testMaster(&aes256, key_hash, &cipher1)) {
    //   strcpy(f.id, "p gmail");
    //   strcpy(f.name, "lol");
    //   writeToFlash(&f, (uint16_t)64);
    //   updateState(3);
    //   return;
    // }

    // else {
    //   strcpy(userInput, "");
    //   updateState(0);
    //   return;
    // }


    // else {
    //   //append character to string
    //   strcat(state1Input, menuItems[value]);
    //   Serial.println(state1Input);
    //   displayMessage(state1Input);
    // }
  }
  if (state == 2)
  {
    //do ur calculation and update to state 3 if correct or go back to state 0
    unsigned long curTime = getCurrentTime();
    takeInput();
    //userInput has totp
    char masterPass[5];


    getMasterFromTotp(userInput, masterPass, curTime);
    Serial.println(String(userInput) + " " + String(masterPass));

    // check Master pass here
    Serial.println("CHECKING pass");
    //Check Master Password
    sha256.reset();
    sha256.update(masterPass, strlen(masterPass));
    sha256.finalize(key_hash, sizeof(key_hash));
    aes256.setKey(key_hash, aes256.keySize());

    byte ecrt[16];
    char passwrd[16];
    // strcpy(passwrd, "lmao");
    aes256.encryptBlock(f.cipher, (uint8_t*)passwrd);


    if (testMaster(&aes256, key_hash, &cipher1)) {
      //   strcpy(f.id, "p gmail");
      //   strcpy(f.name, "lol");
      //   writeToFlash(&f, (uint16_t)64);
      Serial.println("Master verified");
      updateState(3);
      return;
    }

    else {
      strcpy(userInput, "");
      updateState(0);
      return;
    }
  }
  if (state == 3) {
    if (value == 0) {
      updateState(4);
      return;
    }
    if (value == 1) {
      updateState(5);
      return;
    }
    if (value == 2){
        backup();
        return;
    }
  }
  if (state == 4) {
    if (value == numItems - 1) {
      updateState(3);
      return;
    }
    readFromFlash(&f, (1 + value) * 64);
    aes256.decryptBlock(decryptedText, (uint8_t*)f.cipher);
    Serial.println(String(f.id) + "'" + String((char*)decryptedText));
    Keyboard.print(String(f.id));
    Keyboard.press(KEY_RETURN);
    delay(10);
    Keyboard.release(KEY_RETURN);
    delay(7000);
    Keyboard.print(String((char*)decryptedText));
    Keyboard.press(KEY_RETURN);
    delay(10);
    Keyboard.release(KEY_RETURN);
    //Serial.println((char *)decryptedText);
    return;
  }

  if (state == 5) {
    clearStruct();
    takeInput();
    strcpy(f.name, userInput);
    updateState(7);
    return;
  }
  if (state == 6) {  ////BEDAAAAAAAAAAAAAAAAAAAAAAA
    updateState(7);
  }

  if (state == 7) {
    takeInput();
    strcpy(f.id, userInput);
    updateState(9);
    return;
  }
  if (state == 8) {
    updateState(9);
  }

  if (state == 9) {
    takeInput();
    //userInput has pass
    strcpy(tempPass, userInput);

    updateState(11);
    return;
  }
  if (state == 10) {
    updateState(11);
  }
  if (state == 11) {
    aes256.encryptBlock(f.cipher, (uint8_t*)tempPass);

    i2ceeprom.read((uint16_t)0, &numBlocks, (uint16_t)1);

    writeToFlash(&f, 64 * (numBlocks + 1));
    numBlocks++;
    i2ceeprom.write((uint16_t)0, &numBlocks, (uint16_t)1);
    updateState(3);
    Serial.println("numblocks " + String(numBlocks));
  }
}

void updateState(int newState) {
  switch (newState) {
    case 0:
      state = 0;
      numItems = numState0;
      copyArray(menuItems, state0, numItems);
      value = 0;
      break;
    case 1:
      state = 1;
      numItems = numState1;
      copyArray(menuItems, state1, numItems);
      value = 0;
      break;
    case 2:
      state = 2;
      numItems = numState2;
      copyArray(menuItems, state2, numItems);
      value = 0;
      break;
    case 3:
      state = 3;
      numItems = numState3;
      copyArray(menuItems, state3, numItems);
      value = 0;
      break;
    case 4:
      state = 4;
      i2ceeprom.read((uint16_t)0, &numBlocks, (uint16_t)1);


      for (int i = 0; i < numBlocks; i++) {
        readFromFlash(&data[0], (1 + i) * 64);
        strcpy(menuItems[i], data->id);
        strcat(menuItems[i], "   :   ");
        strcat(menuItems[i], data->name);
      }
      strcpy(menuItems[numBlocks], "back");
      numItems = numBlocks + 1;


      break;
    case 5:
      state = 5;
      numItems = numState5;
      copyArray(menuItems, state5, numItems);
      break;
    case 6:
      state = 6;
      numItems = numState6;
      //rotaryEncoder.setBoundaries(0, numItems - 1, true);
      copyArray(menuItems, state6, numItems);
      break;
    case 7:
      state = 7;
      numItems = numState7;
      //rotaryEncoder.setBoundaries(0, numItems - 1, true);
      copyArray(menuItems, state7, numItems);
      break;
    case 8:
      state = 8;
      numItems = numState8;
      //rotaryEncoder.setBoundaries(0, numItems - 1, true);
      copyArray(menuItems, state8, numItems);
      break;
    case 9:
      state = 9;
      numItems = numState9;
      //rotaryEncoder.setBoundaries(0, numItems - 1, true);
      copyArray(menuItems, state9, numItems);
      break;
    case 10:
      state = 10;
      numItems = numState10;
      copyArray(menuItems, state10, numItems);
      break;
    case 11:
      state = 11;
      numItems = numState11;
      copyArray(menuItems, state11, numItems);
      break;
  }
}

void displayMessage(char* message) {
  clearDisplay();
  int x = display.width();
  int msgLen = strlen(message);
  int minX = -12 * msgLen;

  if (msgLen <= WIDTH_IN_CHARS) {
    display.setCursor(0, 16);
    display.print(message);
    display.display();
    int mil = millis();
    if (!digitalRead(4) && millis() - mil < 200) {
      clearDisplay();
      handleChange();
      delay(500);
      return;
    }
    return;
  }
  while (x > minX) {

    display.setCursor(x, 16);
    display.print(message);
    int mil = millis();
    if (!digitalRead(4) && millis() - mil < 200) {
      clearDisplay();
      handleChange();
      delay(500);
      return;

    }
    display.display();
    x = x - SCROLL_SPEED;
    clearDisplay();
  }

  x = display.width();
}

void clearDisplay() {
  display.clearDisplay();
  topStuff();
}

void topStuff() {
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Sat:13  03:56:32  67%");
  display.setTextSize(2);
}

void setupDisplay(void) {
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.display();
  delay(500);  // Pause for 2 seconds

  display.clearDisplay();

  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
}



void copyArray(char array1[100][32], char array2[100][32], int arraySize) {
  for (int j = 0; j < arraySize; j++) {
    strcpy(array1[j], array2[j]);
  }
}

void setupFlash() {
  if (i2ceeprom.begin(0x50, &Wire)) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    Serial.println("Found I2C EEPROM");
  } else {
    Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
    while (1) delay(10);
  }
}

void writeToFlash(struct cipherVector* cipherVectorPtr, uint16_t addr) {
  memcpy(buffer, (void*)cipherVectorPtr, sizeof(cipherVector));  //to write
  i2ceeprom.write(addr, buffer, sizeof(buffer));
}

void readFromFlash(struct cipherVector* cipherVectorPtr, uint16_t addr) {
  i2ceeprom.read(addr, buffer, sizeof(cipherVector));  //to read
  memcpy((cipherVector*)cipherVectorPtr, buffer, sizeof(cipherVector));
}

void clearStruct() {
  strcpy(f.id, "");
  strcpy(f.name, "");
  strcpy(tempPass, "");
}

void rotary_loop()
{
  value = encoder.getCount() / 2;
  value = ((value % numItems) + numItems) % numItems;
}

void takeInput() {
  strcpy(userInput, "");
  int numChars = 40;
  char chars[50][32] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "@", ".", "<--", "OK" };
  char tempStr[32];
  delay(500);
  while (1) {
    //    Serial.print("START: ");
    //    Serial.println(userInput);
    value = encoder.getCount() / 2;
    value = ((value % numChars) + numChars) % numChars;
    //    Serial.println(value);
    strcpy(tempStr, userInput);
    strcat(tempStr, chars[value]);
    display.clearDisplay();
    display.setCursor(0, 0);
    topStuff();
    display.setCursor(0, 16);
    display.print(tempStr);
    Serial.print(tempStr);
    display.display();

    int mil = millis();
    if (!digitalRead(4) && millis() - mil < 200) {
      Serial.println("click");
      if (value == 39) {  //OK
        display.clearDisplay();
        return;
      }

      if (value == 38) {  //B.Space
        if (strlen(userInput) > 0)
          userInput[strlen(userInput) - 1] = '\0';
      } else {
        strcat(userInput, chars[value]);
      }


      delay(500);
    }
    //    Serial.print("END: ");
    //    Serial.println(userInput);
    delay(50);
  }
}

int getFingerprint() {
  int fingerprintID = getFingerprintID();
  Serial.println(fingerprintID);
  if (fingerprintID < 0) {
    //no match
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    delay(1000);
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
  } else if (fingerprintID > 0) {
    //match
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    delay(1000);
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    return 1;
  }
  // else{
  //   digitalWrite(2, HIGH);
  //   digitalWrite(3, HIGH);
  // }
  delay(50);  //don't ned to run this at full speed.
  return 0;
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

  if (finger.fingerID > 0 && finger.fingerID < 255) {
    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID;
  }


  return 0;
}

void getMasterFromTotp(char *totp, char *masterPass, unsigned long curTime) {
  Serial.print("TOTP:" );
  Serial.println(totp);
  Serial.print("CUR TIME: ");
  Serial.println(curTime);
  String totpStr = String(totp);
  Serial.println(totpStr);
  unsigned long num1 = strtoul(totpStr.c_str(), NULL, 16);

  curTime = curTime / 30;

  int twos[4];  // 56, 39, 42, 05
  int k = 0;
  while (curTime) {
    twos[k++] = curTime % 100;
    curTime /= 100;
  }

  unsigned long num2 = 0;
  for (int i = 0; i < 4; i++) {
    num2 += twos[i] << 8 * i;
  }

  unsigned long masterPassVal = num1 - num2;
  Serial.print("Master pass val: ");
  Serial.println(masterPassVal);

  masterPass[0] = (masterPassVal >> 24) & 0xFF;
  masterPass[1] = (masterPassVal >> 16) & 0xFF;
  masterPass[2] = (masterPassVal >> 8) & 0xFF;
  masterPass[3] = (masterPassVal >> 0) & 0xFF;
  masterPass[4] = 0;
  Serial.println(num1);
  Serial.println(num2);
  Serial.println(masterPass);

}

unsigned long getCurrentTime() {
  unsigned long t = rtc.now().unixtime();
  Serial.println(t);
  return t;
}

void backup(){
  i2ceeprom.read((uint16_t)0, &numBlocks, (uint16_t)1);
  
  uint8_t readbuf[64];
  for (int i=0; i<numBlocks+1; i++){
    i2ceeprom.read(64*i, readbuf, 64); 
    Serial.write(readbuf, 64);
  }
}
