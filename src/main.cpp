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

      // Serial.printf("total packets ");
      // Serial.printf("%d ", totalpackets);
      // Serial.printf("Count ");
      // Serial.printf(" %d ", count_t);
      // Serial.printf("Data ");
      // Serial.printf("%02X ", tdata[totalpackets]);
      // Serial.printf("\n");

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

        // Number of cards
        int num_cards = tdata[4];
        Serial.printf("num of cards: %d\n", num_cards);
        // RSSI
        int rssi[10];
        for (int i = 1; i <= num_cards; i++)
        {
          int index = (13 * (num_cards - 1)) + 5 + num_cards - i;
          Serial.printf("index %d \n", index);
          rssi[i - 1] = tdata[index];
        }
        for (int i = 0; i < num_cards; i++)
        {
          Serial.printf("RSSI of card %d is %02X", i + 1, rssi[i]);
        }
        Serial.printf("\n");

        // EPC
        byte epc[12][10];

        Serial.printf("\n");
        break;
      }
    }
    // do the task that is related to variable;
  }
}