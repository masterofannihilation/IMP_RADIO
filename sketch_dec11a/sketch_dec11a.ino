#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED displej - definovanie rozmerov
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Definovanie pinov pre SPI pripojenie OLED displeja
#define OLED_CS   5   // Chip Select
#define OLED_DC   17   // Data/Command
#define OLED_RESET 16 // Reset

// Definovanie pin pre zvukový výstup
#define SOUND_PIN 18 // Tento pin bude generovať zvuk cez tone() funkciu

// Vytvorenie objektu pre OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED initialization failed!"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("KOKOT");
  display.display();

}


void loop() {
  // Prázdny loop, kód beží len v setup()
}