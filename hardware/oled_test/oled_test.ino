#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define SCROLL_SPEED 1
#define WIDTH_IN_CHARS 10

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(9600);
  setupDisplay();      

}

void loop() {
  displayMessage("posterskens");
  delay(200);
}


void setupDisplay(void) {
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }

  display.display();
  delay(500); // Pause for 2 seconds

  display.clearDisplay();

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
}


void displayMessage(char* message) {
  int x = display.width();
  int msgLen = strlen(message);
  int minX = -12 * msgLen;

  if (msgLen <= WIDTH_IN_CHARS){
    display.setCursor(0, 16);
    display.print(message);
    display.display();
    return;
  }
  while (x > minX) {

    display.setCursor(x, 16);
    display.print(message);

    display.display();
    x = x - SCROLL_SPEED;
    display.clearDisplay();
  }
  x = display.width();
}

// Scroll based on length
// use outer loop. 
