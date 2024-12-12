#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RDA5807.h>
#include "KY040rotary.h"

// I2C bus pin on ESP32
#define ESP32_I2C_SDA 21
#define ESP32_I2C_SCL 22

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SPI pin definitions for OLED
#define OLED_CS   5   // Chip Select
#define OLED_DC   17  // Data/Command
#define OLED_RESET 16 // Reset

// Rotary Encoder Pins
#define ENCODER_CLK 34 // CLK pin
#define ENCODER_DT  35 // DT pin
#define ENCODER_SW  25 // SW pin (Button)

bool isVolumeMode = false; // Track whether encoder changes volume or frequency

// Create OLED object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

RDA5807 rx; // RDA5807 object

// Create KY040 rotary encoder object
KY040 myEncoder(ENCODER_CLK, ENCODER_DT, ENCODER_SW);

// Function to show help on serial monitor
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

// Function to update the OLED display
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

// Function to show the current radio status
void showStatus()
{
  char aux[80];
  sprintf(aux, "\nYou are tuned on %u MHz | RSSI: %3.3u dBuV | Vol: %2.2u | %s ",
          rx.getFrequency(), rx.getRssi(), rx.getVolume(), (rx.isStereo()) ? "Yes" : "No");
  Serial.print(aux);
  updateDisplay();
}

// Rotary Encoder Callback for Button Press
void onButtonPress()
{
  Serial.println("button press");
  isVolumeMode = !isVolumeMode; // Toggle mode
  updateDisplay();
}

// Rotary Encoder Callback for Right Turn
void onEncoderRight()
{
  Serial.println("clockwise");
  if (isVolumeMode) {
    int newVolume = constrain(rx.getVolume() + 1, 0, 15);
    rx.setVolume(newVolume);
  } else {
    int newFrequency = constrain(rx.getFrequency() + 10, 8700, 10800); // FM range in 100kHz steps
    rx.setFrequency(newFrequency);
  }
  showStatus();
}

// Rotary Encoder Callback for Left Turn
void onEncoderLeft()
{
  Serial.println("counter clockwise");
  if (isVolumeMode) {
    int newVolume = constrain(rx.getVolume() - 1, 0, 15);
    rx.setVolume(newVolume);
  } else {
    int newFrequency = constrain(rx.getFrequency() - 10, 8700, 10800); // FM range in 100kHz steps
    rx.setFrequency(newFrequency);
  }
  showStatus();
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  // Initialize OLED display
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

  // Initialize RDA5807
  Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);
  rx.setup();

  rx.setVolume(0); // Set initial volume
  delay(500);

  // Set initial station
  Serial.print("\nStation 106.5MHz");
  rx.setFrequency(10650); // Frequency * 100

  // Enable RDS
  rx.setRDS(true);

  showHelp();
  showStatus();

  // Initialize KY040 Rotary Encoder
  myEncoder.Begin();
  myEncoder.OnButtonClicked(onButtonPress);
  myEncoder.OnButtonRight(onEncoderRight);
  myEncoder.OnButtonLeft(onEncoderLeft);
}

void loop()
{
  myEncoder.Process(millis()); // Handle encoder updates
  delay(5); // Small delay to prevent excessive CPU usage
}
