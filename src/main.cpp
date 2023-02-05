#include <Arduino.h>

#include "BluetoothSerial.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

#include <Ticker.h>

// #define DEBUG 1

// #if DEBUG == 1
// #define debug(x) Serial.print(x)
// #define debugln(x) Serial.println(x)
// #else
// #define debug(x)
// #define debugln(x)
// #endif

int buzPin = 32, potPin = 34, b1pin = 27, b2pin = 26, b3pin = 25, b4pin = 33;

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

void inputs(char *message, int number, int strength);
void wifi(int x, int y, int number);
void batteryDraw(int percentage, bool charge, int charging_percent);
void modee();
void charge();

void change_delay(int intensity);

int count = 0, count_t = 0, totalpackets = 0;
byte tdata[200];
bool initial = true;
int delay_buz = 20000, intensity;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
bool on;
long int last_millis_to_on, last_millis_to_off;

int upper_range{30}, lower_range{70};
long int last_millis_for_printing;
BluetoothSerial SerialBT;

TFT_eSPI tft = TFT_eSPI();
Ticker periodicTicker;
Ticker onceTicker;

bool b1_pin_as_mode = false;
bool b2_pin_as_mode = false;
bool b3_pin_as_mode = false;
bool b4_pin_as_mode = false;

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
void periodicClear()
{
  Serial.flush();
  tft.fillScreen(BLACK);
  modee();
  tft.setCursor(90, 4);
  tft.setTextSize(3);
  tft.setTextColor(CYANN);
  tft.print("SGL");

  ledcWrite(ledChannel, 0);
}

void setup()
{

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
  // uint16_t identifier = tft.readID();
  tft.begin();
  // tft.setRotation();
  tft.fillScreen(WHITE);
  tft.setCursor(30, 120);
  tft.setTextSize(10);
  tft.setTextColor(LIGHT_BLUE);
  tft.print("SGL");
  delay(3000);
  tft.fillScreen(BLACK);
  modee();

  tft.setCursor(90, 4);
  tft.setTextSize(3);
  tft.setTextColor(CYANN);
  tft.print("SGL");

  ledcWrite(ledChannel, 0);
  last_millis_for_printing = millis();
}

void loop()
{
  if(digitalRead(b1pin) == 0){
    b1_pin_as_mode = true;
    b2_pin_as_mode = false;
    b2_pin_as_mode = false;
    b2_pin_as_mode = false;
    mode = 1;
    periodicClear();
    modee();
  }else if(digitalRead(b2pin) == 0){
    b1_pin_as_mode = false;
    b2_pin_as_mode = true;
    b2_pin_as_mode = false;
    b2_pin_as_mode = false;
    mode = 2;
    periodicClear();
    modee();
  }else if(digitalRead(b3pin) == 0){
    b1_pin_as_mode = false;
    b2_pin_as_mode = false;
    b2_pin_as_mode = true;
    b2_pin_as_mode = false;
    periodicClear();
    mode = 3;
    modee();
  }else if(digitalRead(b4pin) == 0){
    b1_pin_as_mode = false;
    b2_pin_as_mode = false;
    b2_pin_as_mode = false;
    b2_pin_as_mode = true;
    periodicClear();
    mode = 4;
  }


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
          // Serial.printf("\nnum of cards: %d", num_cards);
          SerialBT.print("NUM ");
          SerialBT.print(static_cast<int>(num_cards));
          // RSSI
          if (b1_pin_as_mode){
            upper_range = 30;
            lower_range = 70;
          }
          else if(b2_pin_as_mode){
            upper_range = 40;
            lower_range = 70;
          }
          else if(b3_pin_as_mode){
            upper_range = 50;
            lower_range = 70;
          }
          else if(b4_pin_as_mode){
            upper_range = 60;
            lower_range = 70;
          }

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
              // Serial.printf("\nRSSI of card %d is %d", i, rssi[i - 1]);
              SerialBT.print(rssi[i - 1]);
              SerialBT.print(" ");
            }

            if (num_cards >= 1)
            {
              intensity = map(rssi[0], upper_range,  lower_range, 4, 0);
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
            
             char buf[100];
              sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X", epc[i][1], epc[i][2], epc[i][3], epc[i][4], epc[i][5], epc[i][6], epc[i][7]);
              char *buf_temp = buf;
              inputs(buf_temp, i, map(rssi[i], upper_range,  lower_range, 4, 0));
              SerialBT.print(" ");
              last_millis_for_printing = millis();
            
            
            // Serial.printf("\n");
          }
          SerialBT.println("\n");
          // Serial.printf("\n");
          // for (int i=0; i<num_cards; i++){
              
          // }
           
          break;
        }
        catch (...)
        {
          Serial.print("Error");
        }
      }
    }
  }

  if (((millis() - last_millis_to_off) > delay_buz) && on == true)
  {
    on = false;
    Serial.println("THis wont noise");
    Serial.println(delay_buz);
    last_millis_to_off = millis();
    Serial.print(analogRead(potPin));
    ledcWrite(ledChannel, 0);
  }
  else if (((millis() - last_millis_to_off) > delay_buz) && on == false)
  {
    Serial.println("THis will noise");
    Serial.println(delay_buz);
    last_millis_to_off = millis();
    on = true;
    Serial.print(analogRead(potPin));
    ledcWrite(ledChannel, map(map(analogRead(potPin), 0, 4095, 0, 255), 0, 255, 0, 50));
  }

  if ((millis() - last_millis_to_on > 5000))
  {
    periodicClear();
    last_millis_to_on = millis();
  }
}

void change_delay(int intensity)
{
  switch (intensity)
  {

  case 0:
    delay_buz = 20000;
    break;

  case 1:
    delay_buz = 2000;
    break;

  case 2:
    delay_buz = 800;
    break;

  case 3:
    delay_buz = 200;
    break;

  case 4:
    delay_buz = 50;
    break;

  default:
    delay_buz = 20000;
    break;
  }
}

int calculate_distance(int rssi)
{

  return 0;
}
void inputs(char message[], int number, int strength) // number = number of box we want to print at screen , strength = signal strength of wifi
{
  tft.fillRect(5, 90 + number * 30, 220, 25, BLACK);
  tft.drawRect(5, 90 + number * 30, 220, 25, LIGHT_PINK); // to print UID string rect 1
  tft.setCursor(10, 95 + number * 30);
  tft.setTextColor(CYAN);
  tft.setTextSize(2);
  tft.print(message);

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
  tft.setTextColor(LIGHT_PINK);
  tft.setTextSize(2.8);
  tft.print("MODE");
  tft.drawRect(4, 34, 225, 40, MAGENTA);
  tft.setCursor(50, 10);
  tft.setTextColor(WHITE);
   tft.setTextSize(2.8);
  tft.print(String(mode));
}