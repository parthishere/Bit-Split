#include <Arduino.h>

#include "BluetoothSerial.h"

#define LENGTH_CMD 2       // Bytes
#define LENGTH_ECODE 2     // Bytes
#define LENGTH_TOTALCARD 1 // Bytes
#define LENGTH_RSSI 1      // Bytes
#define LENGTH_EPC_LEN 1   // Bytes
#define LENGTH_EPC 12      // Bytes
#define LENGTH_CRC 2       // Bytes

int count = 0, count_t = 0, totalpackets = 0;
byte tdata[100];
bool initial = true;

// initial_sequance = {2, 2, 1};

BluetoothSerial SerialBT;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  delay(1000);
  Serial1.begin(9600, SERIAL_8N1, 4, 2); // RX, TX
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
        Serial.printf("\nnum of cards: %d\n", num_cards);
        Serial1.write("CARDS_NUM");
        Serial1.write(num_cards);

        // RSSI
        byte rssi[num_cards];
        Serial1.write("RSSI");
        for (int i = 1; i <= num_cards; i++)
        {
          int index = (13 * (i - 1)) + 5;
          rssi[i - 1] = tdata[index];
          Serial.printf("\nRSSI of card %d is %d", i, rssi[i - 1]);
        }
        Serial1.write(rssi, sizeof(rssi) / sizeof(byte));

        // EPC
        byte epc[10][12];
        Serial.printf("\nData of Cards: ");
        for (int i = 0; i < num_cards; i++)
        {
          Serial1.write("CARD_DATA");
          for (int j = 0; j < 12; j++)
          {
            int index = (13 * (i)) + 7 + j;
            epc[i][j] = tdata[index];
            Serial.printf("%02X ", epc[i][j]);
            Serial1.write(epc[i][j]);
          }
          Serial.printf("\n");
        }

        Serial.printf("\n");
        break;
      }
    }
    // do the task that is related to variable;
  }
}