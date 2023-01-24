#include <Arduino.h>

#include "BluetoothSerial.h"

int count = 0, count_t = 0, totalpackets = 0;
byte tdata[200];
bool initial = true;
int buzPin = 12, delay_buz, intensity;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

BluetoothSerial SerialBT;

/*
Buz : D32
SDA : D21
SCL : D22
D27 : B1
D26 : B2
D25 : B3
D33 : B4
*/

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial2.begin(115200);
  delay(1000);
  SerialBT.begin();
  Serial.println("Bluetooth Started! Ready to pair...");
  ledcSetup(ledChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(buzPin, ledChannel);
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
              rssi_int[i - 1] = tdata[index];

              Serial.printf("\nRSSI of card %d is %d", i, rssi[i - 1]);
              SerialBT.print(rssi[i - 1]);
              rssi_int[i - 1] = map(rssi_int[i - 1], 36, 70, 1, 5);
              Serial.print(rssi_int[i - 1]);
              SerialBT.print(" ");
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
    // do the task that is related to variable;
  }

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
  Serial.print("delay_buzz");
  Serial.println(delay_buz);

  ledcWrite(ledChannel, 255);
  delay(delay_buz);
  ledcWrite(ledChannel, 0);
  delay(delay_buz);
}