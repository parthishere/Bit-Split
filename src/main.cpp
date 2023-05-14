#include <Arduino.h>

#include "BluetoothSerial.h"
#include "esp_system.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

#include <Ticker.h>
#include <list>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define buzChannel 0
#define FREQ 4950
#define RESOLUTION 8

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI 21
#define OLED_CLK 22
#define OLED_DC 5
#define OLED_CS 19
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// #define DEBUG 1

// #if DEBUG == 1
// #define debug(x) Serial.print(x)
// #define debugln(x) Serial.println(x)
// #else
// #define debug(x)
// #define debugln(x)
// #endif

int buzPin = 32, potPin = 34, b1pin = 27, b2pin = 26, b3pin = 25, b4pin = 33, batPin = 4, audioPin = 33, jackIsConnectedPin = 15;

// #define LCD_CS A3 // Chip Select goes to Analog 3
// #define LCD_CD A2 // Command/Data goes to Analog 2
// #define LCD_WR A1 // LCD Write goes to Analog 1
// #define LCD_RD A0 // LCD Read goes to Analog 0

// #define BLACK 0x0000
// #define BLUE 0x001F
// #define RED 0xF800
// #define GREEN 0x07E0
// #define CYAN 0x07FF
// #define MAGENTA 0xF81F
// #define YELLOW 0xFFE0
// #define WHITE 0xFFFF
// #define PINK 0xEB34
// #define LIGHT_PINK 0x9f3b
// #define LIGHT_PURPLE 0x8594
// #define LIGHT_BLUE 0x93ff
// #define CYANN 0xE536F3

const char message[] = "card detail";
int charge_length = 50, is_charging;

void ui(char *message = nullptr, int index = -1, int strength = -1);

void drawWiFiBars(int x, int y, int signal);
void drawBatteryLevel(int x, int y, float adc_value);
void batteryDraw(int analog_value);
void modee();
void charge();

int change_delay(int intensity);

bool use_audio_jack = false;
int count = 0, count_t = 0, totalpackets = 0;
byte tdata[200];
bool initial = true;
int delay_buz = -1, intensity;
int temp_delay_buz{};
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
bool on;
long int last_millis_to_on_battery_draw, last_millis_to_on, last_millis_to_off;
bool do_not_buz;
int upper_range{30}, lower_range{70};
long int last_millis_for_printing;
BluetoothSerial SerialBT;

// TFT_eSPI tft = TFT_eSPI();
Ticker periodicTicker;
Ticker onceTicker;

// QueueHandle_t xQueue;

bool b1_pin_as_mode = false;
bool b2_pin_as_mode = false;
bool b3_pin_as_mode = false;

unsigned long buzzerStartMillis = 0;
int buzzerState = 0;
int buzzerDuration = 1000;
int buzzerEndMillis;

/*
Buz : D32
SDA : D21
SCL : D22
D27 : B1
D26 : B2
D25 : B3
D33 : B4
pot : D4
*/
int mode = 1;

static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1;
void periodicClear()
{
  display.fillRect(1, 17, 128 - 1, 64 - 17, BLACK);
  display.display();
}

void swap(int *a, int *b, byte c[], byte d[])
{
  int t = *a;
  *a = *b;
  *b = t;

  byte temp[12];
  memcpy(temp, c, 12);
  memcpy(c, d, 12);
  memcpy(d, temp, 12);
}

int partition(int rssi[], byte epc[][12], int low, int high)
{

  int pivot = rssi[high];
  int i = (low - 1);

  for (int j = low; j < high; j++)
  {
    if (rssi[j] <= pivot)
    {

      i++;
      swap(&rssi[i], &rssi[j], epc[i], epc[j]);
    }
  }

  swap(&rssi[i + 1], &rssi[high], epc[i + 1], epc[high]);

  return (i + 1);
}

void quickSort(int rssi[], byte epc[][12], int low, int high)
{
  if (low < high)
  {
    int pi = partition(rssi, epc, low, high);

    quickSort(rssi, epc, low, pi - 1);

    quickSort(rssi, epc, pi + 1, high);
  }
}

void setup()
{

  Serial.begin(115200);
  Serial2.begin(115200);
  delay(1000);

  SerialBT.begin();
  esp_log_level_set("*", ESP_LOG_ERROR);
  esp_log_level_set("heap_init", ESP_LOG_WARN);
  esp_log_level_set("heap_caps", ESP_LOG_WARN);
  esp_log_level_set("spi_master", ESP_LOG_WARN);

  esp_log_set_vprintf(vprintf);
  // esp_log_set_putchar(esp_log_putchar);

  Serial.println("Reader Started");

  Serial.println("Bluetooth Started! Ready to pair...");

  pinMode(potPin, INPUT);

  pinMode(b1pin, INPUT_PULLUP);
  pinMode(b2pin, INPUT_PULLUP);
  pinMode(b3pin, INPUT_PULLUP);
  pinMode(b4pin, INPUT_PULLUP);
  pinMode(jackIsConnectedPin, INPUT_PULLUP);
  pinMode(audioPin, OUTPUT);
  pinMode(batPin, INPUT);

  ledcSetup(buzChannel, FREQ, RESOLUTION);
  ledcAttachPin(buzPin, buzChannel);

  // attach the channel to the GPIO to be controlled

  /* TFT */
  // tft.init();
  // // uint16_t identifier = tft.readID();
  // tft.begin();
  // tft.setRotation(2);
  // tft.fillScreen(WHITE);
  // tft.setCursor(30, 120);
  // tft.setTextSize(10);
  // tft.setTextColor(LIGHT_BLUE);
  // tft.print("SGL");
  // delay(3000);
  // tft.fillScreen(BLACK);
  // modee();

  // tft.setCursor(90, 4);
  // tft.setTextSize(3);
  // tft.setTextColor(CYANN);
  // tft.print("SGL");

  last_millis_for_printing = millis();

  if (digitalRead(jackIsConnectedPin) == 0)
  {
    use_audio_jack = true;
  }
  else
  {
    use_audio_jack = false;
  }

  if (use_audio_jack == true)
  {
    Serial.println("wont buzz");
    digitalWrite(audioPin, LOW);
    delay(2000);
    digitalWrite(audioPin, HIGH);
    delay(100);
    digitalWrite(audioPin, LOW);
    delay(100);
    digitalWrite(audioPin, HIGH);
    delay(100);
    digitalWrite(audioPin, LOW);
    delay(100);
  }
  else
  {
    Serial.println("will buzz");
    ledcWrite(buzChannel, LOW);
    delay(2000);
    ledcWrite(buzChannel, 128);
    delay(100);
    ledcWrite(buzChannel, LOW);
    delay(100);
    ledcWrite(buzChannel, 128);
    delay(100);
    ledcWrite(buzChannel, LOW);
    delay(100);
  }
  int count;
  byte buf[5] = {0x03, 0xf0, 0x02, 0x8b, 0x2f};
  Serial2.write(buf, 5);
  while (!Serial2.available())
  {
    count++;
    if (count > 200)
    {
      ledcWrite(buzChannel, 128);
    }
    Serial.print(".");
  }
  Serial.println();
  while (Serial2.available())
  {
    
    
    Serial.printf("%02X", Serial2.read());
  }

  while (!Serial)
  {
    ledcWrite(buzChannel, 128);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
      ledcWrite(buzChannel, 128);
    } // Don't proceed, loop forever
  }

  display.display();

  // Draw a single pixel in white

  while (!Serial2)
  {
    ledcWrite(buzChannel, 128);
  }
  delay_buz = change_delay(-1);

  display.clearDisplay();

  // drawWiFiBars(0,0, 2);
  // drawBatteryLevel(25, 50,3.7);
  modee();

  display.display();
}

void loop()
{
  if (digitalRead(jackIsConnectedPin) == 0)
  {
    use_audio_jack = true;
  }
  else
  {
    use_audio_jack = false;
  }

  if (digitalRead(b1pin) == 0)
  {
    b1_pin_as_mode = true;
    b2_pin_as_mode = false;
    b2_pin_as_mode = false;
    b2_pin_as_mode = false;
    mode = 1;
    // display.clearDisplay();
    modee();
    upper_range = 30;
    lower_range = 70;
  }
  else if (digitalRead(b2pin) == 0)
  {
    b1_pin_as_mode = false;
    b2_pin_as_mode = true;
    b2_pin_as_mode = false;
    b2_pin_as_mode = false;
    mode = 2;
    // tft.fillRect(50, 10, 10, 12, BLACK);
    modee();
    upper_range = 45;
    lower_range = 70;
  }
  else if (digitalRead(b3pin) == 0)
  {
    b1_pin_as_mode = false;
    b2_pin_as_mode = false;
    b2_pin_as_mode = true;
    b2_pin_as_mode = false;
    // tft.fillRect(50, 10, 10, 10, BLACK);
    mode = 3;
    // tft.fillRect(50, 10, 10, 12, BLACK);
    modee();
    upper_range = 60;
    lower_range = 70;
  }

  while (Serial2.available() > 0)
  {
    if (initial)
    {
      totalpackets = Serial2.read();
      if (totalpackets != 34 && totalpackets != 20 && totalpackets != 48)
      {
        break;
      }
      totalpackets += 1;
      count_t = totalpackets;
      tdata[count_t];
      initial = false;
    }
    else
    {
      totalpackets--;
      if (totalpackets >= 0 && totalpackets < count_t)
      {
        tdata[totalpackets] = Serial2.read();
      }

      if (totalpackets == 0)
      {
        try
        {
          initial = true;
          for (int i = 0; i < count_t / 2; i++)
          {
            byte temp = tdata[i];
            tdata[i] = tdata[count_t - i - 1];
            tdata[count_t - i - 1] = temp;
          }

          int num_cards = tdata[4];
          // Serial.printf("\nnum of cards: %d", num_cards);
          SerialBT.print("NUM ");
          SerialBT.print(static_cast<int>(num_cards));
          // RSSI

          byte rssi[num_cards];
          int rssi_int[num_cards];

          SerialBT.print(" ");
          SerialBT.print(" RSSI ");
          for (int i = 1; i <= num_cards; i++)
          {
            int index = (13 * (i - 1)) + 5 + (i - 1);
            if (index < (count_t - 1))
            {
              rssi[i - 1] = tdata[index];
              rssi_int[i - 1] = (int)tdata[index];
              // Serial.printf("\nRSSI of card %d is %d", i, rssi[i - 1]);
              SerialBT.print(rssi[i - 1]);
              SerialBT.print(" ");
            }
          }

          // EPC
          byte epc[num_cards][12];
          // Serial.printf("\nData of Cards:");
          SerialBT.print(" DATA ");
          for (int i = 0; i < num_cards; i++)
          {
            for (int j = 0; j < 12; j++)
            {
              int index = (13 * (i)) + 7 + j + i;
              if (index < (count_t - 1))
              {
                epc[i][j] = tdata[index];
                // Serial.printf("%02X ", epc[i][j]);
                SerialBT.print(epc[i][j]);
              }
            }
            // Serial.printf("\n");
          }
          SerialBT.println("\n");
          // Serial.printf("\n");
          if (!(num_cards <= 1))
          {
            quickSort(rssi_int, epc, 0, num_cards - 1);
          }
          else
            ;

          if ((millis() - last_millis_for_printing) > 400)
          {

            for (int i = 0; i < num_cards; i++)
            {

              char buf[100];
              sprintf(buf, "02X%02X%02X", epc[i][10], epc[i][11], epc[i][12]);
              char *buf_temp = buf;
              //==> tft.fillRect(5, 90 + i * 30, 220, 25, BLACK);
              Serial.print("RSSI right now.. ");
              Serial.println(rssi_int[i]);
              int rssi_for_print_in_tft = map(rssi_int[i], upper_range, lower_range, 4, 1);
              ui(buf_temp, i+1, rssi_for_print_in_tft);
              last_millis_for_printing = millis();

              if (i == 0)
              {

                if (rssi_for_print_in_tft == 4)
                {
                  if (use_audio_jack == true)
                  {
                    digitalWrite(audioPin, HIGH);
                  }
                  else
                  {
                    ledcWrite(buzChannel, 128);
                  }
                  do_not_buz = true;
                }
                else
                {
                  delay_buz = change_delay(rssi_for_print_in_tft);
                  do_not_buz = false;
                }
              }
            }
          }

          break;
        }
        catch (...)
        {
          Serial.print("Error");
        }
      }
    }
  }

  if (delay_buz != -1 && do_not_buz == false)
  {

    if (buzzerState == 0 && millis() - buzzerEndMillis >= delay_buz)
    {
      if (use_audio_jack == true)
      {
        digitalWrite(audioPin, HIGH);
      }
      else
      {
        ledcWrite(buzChannel, 128);
      }
      buzzerState = 1;
      buzzerStartMillis = millis();
    }

    if (buzzerState == 1 && millis() - buzzerStartMillis >= temp_delay_buz)
    {
      if (use_audio_jack == true)
      {
        digitalWrite(audioPin, LOW);
      }
      else
      {
        ledcWrite(buzChannel, LOW);
      }
      buzzerState = 0;
      buzzerEndMillis = millis();
      delay_buz = change_delay(-1);
    }
  }

  else if (do_not_buz == false)
  {
    if (use_audio_jack == true)
    {
      digitalWrite(audioPin, LOW);
    }
    else
    {
      ledcWrite(buzChannel, LOW);
    }
    buzzerState = 0;
  }

  if ((millis() - last_millis_to_on > 5000))
  {
    Serial.println("will clear the display");
    periodicClear();
    last_millis_to_on = millis();
  }


  Serial.flush();
}

int change_delay(int intensity)
{
  switch (intensity)
  {

  case -1:
    digitalWrite(buzPin, 0);
    return -1;

  case 1:
    temp_delay_buz = 600; // OFF Time
    return 100;           // ON TIME

  case 2:
    temp_delay_buz = 320; // OFF Time
    return 180;           // ON TIME

  case 3:
    temp_delay_buz = 160; // OFF Time
    return 240;           // ON TIME

  case 4:
    temp_delay_buz = 299; // OFF Time
    return 1;             // ON TIME

  default:
    return -1;
  }
}

void ui(char *message, int index, int strength) // number = number of box we want to print at screen , strength = signal strength of wifi
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("MODE : ");
  display.print(String(mode));
  display.drawLine(0, 13, 128, 13, WHITE);
  display.display();
  drawBatteryLevel(96, 0, analogRead(potPin));

  const int MESSAGE_WIDTH = 126;
  const int MESSAGE_HEIGHT = 15;
  const int MESSAGE_SPACING = 2;
  const int WIFI_WIDTH = 20;
  const int WIFI_HEIGHT = 10;
  Serial.print("index ");
  Serial.println(index);
  display.fillRect(1, 17 + (MESSAGE_HEIGHT * (index-1)) + (MESSAGE_SPACING * (index-1)), 128 - 1, (MESSAGE_HEIGHT + MESSAGE_SPACING), BLACK);
  display.display();
  // Calculate the x and y coordinates of the message and wifi icon
  int x = 1;
  int y = 17;
  switch (index)
  {
  case 1:
    x = 1;
    y = 17;
    break;
  case 2:
    x = 1;
    y = 17 + MESSAGE_HEIGHT + MESSAGE_SPACING;
    break;
  case 3:
    x = 1;
    y = 17 + (MESSAGE_HEIGHT * 2) + (MESSAGE_SPACING * 2);
    break;
  default:
    break;
  }

  // Draw the wifi signal strength icon
  drawWiFiBars(MESSAGE_WIDTH - WIFI_WIDTH - 5 - 1, y, strength);

  // Draw the message text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  Serial.print("X, y");
  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);

  Serial.print("clear disply ");
  Serial.println(((index + 1) * (MESSAGE_HEIGHT + MESSAGE_SPACING)) + 17);
  display.setCursor(x, y);
  display.print(message);

  display.display();
}

void drawWiFiBars(int x, int y, int signal)
{

  // Fixed width and height of each bar
  const int BAR_WIDTH = 4;
  const int BAR_HEIGHT = 2;

  display.fillRect(x, y, 4 * 4 + 4, 10, BLACK);

  // Draw the bars
  for (int i = 0; i < 4; i++)
  {
    int numBars = i + 1;
    int barX = x + i * (BAR_WIDTH + 1);
    int barY = y + BAR_HEIGHT * 3;
    if (signal >= numBars)
    {
      display.fillRect(barX, barY - (numBars - 1) * BAR_HEIGHT, BAR_WIDTH, numBars * BAR_HEIGHT, WHITE);
    }
    else
    {
      display.drawRect(barX, barY - (numBars - 1) * BAR_HEIGHT, BAR_WIDTH, numBars * BAR_HEIGHT, WHITE);
    }
  }
  display.display();
}

void drawBatteryLevel(int x, int y, float adc_value)
{
  const int BAR_SPACING = 2;
  const int BAR_MAX_WIDTH = 25 - (BAR_SPACING * 2);
  display.fillRect(x, y, 25 + 3, 10, BLACK);

  // Calculate the battery level as a percentage
  int level = map(adc_value, 0, 4095, 0, 100);

  // Calculate the width of the filled bar
  int filledWidth = map(level, 0, 100, 0, BAR_MAX_WIDTH);

  // Draw the battery icon
  display.drawRect(x, y, 25, 10, WHITE);
  display.fillRect(x + BAR_SPACING, y + BAR_SPACING, filledWidth, 10 - (BAR_SPACING * 2), WHITE);
  display.fillRect(x + 25, y + 3, 3, 4, WHITE);
  display.display();
}

void modee()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.fillRect(0, 0, 64, 12, BLACK);
  display.setCursor(0, 0);
  display.print("MODE : ");
  display.print(String(mode));
  display.drawLine(0, 13, 128, 13, WHITE);

  drawBatteryLevel(96, 0, analogRead(potPin));
  display.fillRect(1, 17, 128 - 1, 64 - 17, BLACK);
}
