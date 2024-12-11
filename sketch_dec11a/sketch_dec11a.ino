#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RDA5807.h>

// I2C bus pin on ESP32
#define ESP32_I2C_SDA 21
#define ESP32_I2C_SCL 22

// OLED displej - definovanie rozmerov
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Definovanie pinov pre SPI pripojenie OLED displeja
#define OLED_CS   5   // Chip Select
#define OLED_DC   17  // Data/Command
#define OLED_RESET 16 // Reset

// Vytvorenie objektu pre OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

#define MAX_DELAY_RDS 40   // 40ms - polling method

long rds_elapsed = millis();

RDA5807 rx;

void showHelp()
{
  Serial.println("Type U to increase and D to decrease the frequency");
  Serial.println("Type S or s to seek station Up or Down");
  Serial.println("Type + or - to volume Up or Down");
  Serial.println("Type 0 to show current status");
  Serial.println("Type ? to this help.");
  Serial.println("==================================================");
  delay(1000);
}

// Aktualizácia OLED displeja
void updateDisplay()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  char status[15];
  sprintf(status, "Frequency:");
  display.println(status);
  
  display.setTextSize(1);
  display.println("");

  display.setTextSize(2);
  char freq[32];
  sprintf(freq, "%u.%02u MHz", rx.getFrequency() / 100, rx.getFrequency() % 100);
  display.println(freq);

  display.setTextSize(1);
  display.println("");

  display.setTextSize(2);
  char volume[16];
  sprintf(volume, "Volume: %2.2u", rx.getVolume());
  display.println(volume);

  display.display();
}

void showStatus()
{
  char aux[80];
  sprintf(aux,"\nYou are tuned on %u MHz | RSSI: %3.3u dbUv | Vol: %2.2u | %s ",rx.getFrequency(), rx.getRssi(), rx.getVolume(), (rx.isStereo()) ? "Yes" : "No" );
  Serial.print(aux);
  updateDisplay();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) ;

    // Inicializácia OLED displeja
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("OLED initialization failed!"));
        for (;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Initializing...");
    display.display();

    // Inicializácia RDA5807
    Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);
    rx.setup();

    rx.setVolume(2);
    delay(500);

    // Zvolenie stanice s RDS
    Serial.print("\nStation 106.5MHz");
    rx.setFrequency(10550); // Frekvencia * 100

    // Povolenie RDS
    rx.setRDS(true);

    showHelp();
    showStatus();
}

void loop()
{
  if (Serial.available() > 0)
  {
    char key = Serial.read();
    switch (key)
    {
    case '+':
      rx.setVolumeUp();
      break;
    case '-':
      rx.setVolumeDown();
      break;
    case 'U':
    case 'u':
      rx.setFrequencyUp();
      break;
    case 'D':
    case 'd':
      rx.setFrequencyDown();
      break;
    case 'S':
      rx.seek(RDA_SEEK_WRAP, RDA_SEEK_UP);
      break;
    case 's':
      rx.seek(RDA_SEEK_WRAP, RDA_SEEK_DOWN);
      break;
    case '0':
      showStatus();
      break;
    case '?':
      showHelp();
      break;
    default:
      break;
    }
    delay(200);
    showStatus();
  } 
  delay(5);
}
