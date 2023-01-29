#include <Arduino.h>

#include "BluetoothSerial.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

int buzPin = 32, potPin = 4, b1pin = 27, b2pin = 26, b3pin = 25, b4pin = 33;

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define PINK 0xEB34
#define LIGHT_PINK 0x9f3b
#define LIGHT_PURPLE 0x8594
#define LIGHT_BLUE 0x93ff
#define CYANN 0xE536F3

const char message[] = "card detail";
int charge_length = 50, is_charging;

void inputs(char *message, int number, int strength = 0);
void wifi(int x, int y, int number);
void batteryDraw(int percentage, bool charge, int charging_percent);
void modee();
void charge();

void change_delay(int intensity);

int count = 0, count_t = 0, totalpackets = 0;
byte tdata[200];
bool initial = true;
int delay_buz, intensity;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

long int last_millis;

BluetoothSerial SerialBT;

TFT_eSPI tft = TFT_eSPI();

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

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial2.begin(115200);
  delay(1000);
  SerialBT.begin();
  Serial.println("Bluetooth Started! Ready to pair...");
  pinMode(buzPin, OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(b1pin, INPUT_PULLUP);
  pinMode(b2pin, INPUT_PULLUP);
  pinMode(b3pin, INPUT_PULLUP);
  pinMode(b4pin, INPUT_PULLUP);

  ledcSetup(ledChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(buzPin, ledChannel);

  /* TFT */

  tft.init();
  tft.begin();
  tft.setRotation(2);

  Serial.println(F("OK!"));

  // tft.setRotation();
  tft.fillScreen(WHITE);
  tft.setCursor(30, 120);
  tft.setTextSize(10);
  tft.setTextColor(LIGHT_BLUE);
  tft.print("SGL");
  delay(3000);
  tft.fillScreen(BLACK);
  modee();
}

void loop()
{

  while (Serial2.available() > 0)
  {
    if (initial)
    {
      totalpackets = Serial2.read();
      if (totalpackets != 34 && totalpackets != 20 && totalpackets != 48 && totalpackets != 62 && totalpackets != 76 && totalpackets != 90 && totalpackets != 104)
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
          Serial.printf("\nnum of cards: %d", num_cards);
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
              Serial.printf("\nRSSI of card %d is %d", i, rssi[i - 1]);
              SerialBT.print(rssi[i - 1]);
              SerialBT.print(" ");
            }
            if (num_cards >= 1)
            {
              intensity = map(rssi[0], 30, 70, 1, 5);
              change_delay(intensity);
            }
          }

          // EPC
          byte epc[num_cards][12];
          Serial.printf("\nData of Cards:");
          SerialBT.print(" DATA ");
          for (int i = 0; i < num_cards; i++)
          {
            for (int j = 0; j < 12; j++)
            {
              int index = (13 * (i)) + 7 + j + i;
              if (index < (count_t - 1))
              {
                epc[i][j] = tdata[index];
                Serial.printf("%02X ", epc[i][j]);
                SerialBT.print(epc[i][j]);
              }
            }
            SerialBT.print(" ");
            Serial.printf("\n");
          }
          SerialBT.println("\n");
          Serial.printf("\n");
          break;
        }
        catch (...)
        {
          Serial.print("Error");
        }
      }
    }
  }
  if ((millis() - last_millis) > delay_buz)
  {
    last_millis = millis();
    ledcWrite(ledChannel, map(analogRead(potPin), 0, 4095, 0, 255));
  }
  else if ((millis() - last_millis) > delay_buz)
  {
    last_millis = millis();
    ledcWrite(ledChannel, 0);
  }
  delay(1000);
}

void change_delay(int intensity)
{
  switch (intensity)
  {
  case -1:
    delay_buz = 10;
    break;

  case 1:
    delay_buz = 100;
    break;

  case 2:
    delay_buz = 400;
    break;

  case 3:
    delay_buz = 600;
    break;

  case 4:
    delay_buz = 700;
    break;

  case 5:
    delay_buz = 800;
    break;

  case 6:
    delay_buz = 900;
    break;

  case 7:
    delay_buz = 1000;
    break;

  case 8:
    delay_buz = 1200;
    break;

  case 9:
    delay_buz = 1500;
    break;

  case 10:
    delay_buz = 2000;
    break;
  }
}

int calculate_distance(int rssi)
{

  return 0;
}

void inputs(char *message, int number, int strength) // number = number of box we want to print at screen , strength = signal strength of wifi
{
  tft.fillRect(5, 90 + number * 30, 220, 25, BLACK);
  tft.drawRect(5, 90 + number * 30, 220, 25, PINK); // to print UID string rect 1
  tft.setCursor(10, 95 + number * 30);
  tft.setTextColor(CYAN);
  tft.setTextSize(2.5);
  tft.print(message);

  Serial.println("Strrngth");
  Serial.println(strength);
  switch (strength)
  {
  case 1: // to create 1 bar of wifi signal
    wifi(0, 0, number);
    break;
  case 2: // to create 2 bars
    wifi(0, 0, number);
    wifi(1, 1, number);
    break;
  case 3: // to create 3 bars
    wifi(0, 0, number);
    wifi(1, 1, number);
    wifi(2, 2, number);
    break;
  case 4: // to create 4 bars
    wifi(0, 0, number);
    wifi(1, 1, number);
    wifi(2, 2, number);
    wifi(3, 3, number);
    break;
  default:
    wifi(0, 0, number);
    break;
  }
}

void wifi(int x, int y, int number)
{

  tft.drawRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, BLACK);
  tft.fillRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, BLACK);

  tft.drawRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, WHITE);
  tft.fillRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, WHITE);
}

void batteryDraw(int percentage, bool charge, int charging_percent)
{
  if (charge_length < 0)
  {
    charge_length = 50;
  }
  if (charge)
  {
    tft.drawRect(190, 0, 50, 30, WHITE);
    tft.fillRect(190, 2, 50, 25, BLACK);
    tft.fillRect(190 + charge_length, 2, 50, 25, GREEN);
    tft.drawRect(180, 9, 10, 10, WHITE);
    tft.fillRect(180, 9, 10, 10, WHITE);
    charge_length -= 10;

    Serial.println(charge_length);
  }
  else
  {
    Serial.println("charging process is not going on ");
    int drawValue = map(percentage, 0, 100, 0, 50);
    int battery_x = 190 + (50 - (percentage * 50 / 100));

    if (percentage <= 25)
    {
      tft.drawRect(190, 0, 50, 30, WHITE);
      tft.fillRect(battery_x, 2, drawValue, 25, RED);
      tft.drawRect(180, 9, 10, 10, WHITE);
      tft.fillRect(180, 9, 10, 10, WHITE);
    }
    else if (percentage == 100)
    {
      tft.drawRect(190, 0, 50, 30, WHITE);
      tft.fillRect(battery_x, 2, drawValue, 25, PINK);
      tft.drawRect(180, 9, 10, 10, WHITE);
      tft.fillRect(180, 9, 10, 10, WHITE);
    }
    else
    {
      tft.drawRect(190, 0, 50, 30, WHITE);
      tft.fillRect(battery_x, 2, drawValue, 25, GREEN);
      tft.drawRect(180, 9, 10, 10, WHITE);
      tft.fillRect(180, 9, 10, 10, WHITE);
    }
  }
}

void modee()
{
  tft.setCursor(0, 10); // to write title "mode"
  tft.setTextColor(WHITE);
  tft.setTextSize(2.8);
  tft.print("MODE");
  tft.drawRect(4, 34, 225, 40, MAGENTA);
}