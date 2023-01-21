#include <Arduino.h>

#include "BluetoothSerial.h"

int count = 0, count_t = 0, totalpackets = 0;
byte tdata[200];
bool initial = true;

BluetoothSerial SerialBT;

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial2.begin(115200);
  delay(1000);
  SerialBT.begin();
  Serial.println("Bluetooth Started! Ready to pair...");
}

void loop()
{

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

          for (int i = 0; i < count_t; i++)
          {
            Serial.printf("%02X ", tdata[i]);
          }

          int num_cards = tdata[4];
          Serial.printf("\nnum of cards: %d", num_cards);
          // RSSI

          byte rssi[num_cards];

          for (int i = 1; i <= num_cards; i++)
          {
            int index = (13 * (i - 1)) + 5;
            rssi[i - 1] = tdata[index];
            Serial.printf("\nRSSI of card %d is %d", i, rssi[i - 1]);
          }

          // EPC
          byte epc[10][12];
          Serial.printf("\nData of Cards:");
          for (int i = 0; i < num_cards; i++)
          {
            for (int j = 0; j < 12; j++)
            {
              int index = (13 * (i)) + 7 + j;
              epc[i][j] = tdata[index];
              Serial.printf("%02X ", epc[i][j]);
            }

            Serial.printf("\n");
          }

          Serial.printf("\n");
          break;
        }
        catch (char *excp)
        {
          Serial.print("Error");
        }
      }
    }
    // do the task that is related to variable;
  }
}