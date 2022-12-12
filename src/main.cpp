#include <Arduino.h>

#include "BluetoothSerial.h"

#define LENGTH_CMD 2       // Bytes
#define LENGTH_ECODE 2     // Bytes
#define LENGTH_TOTALCARD 1 // Bytes
#define LENGTH_RSSI 1      // Bytes
#define LENGTH_EPC_LEN 1   // Bytes
#define LENGTH_EPC 12      // Bytes
#define LENGTH_CRC 2       // Bytes

char receivedByte[9], *p;
int temp, intNumber, count = 0, count_t, totalpackets;
byte tdata[] = {};
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

  while (Serial2.available())
  {
    if (initial)
    {
      totalpackets = Serial2.read();
      Serial.printf("%d", totalpackets);
      count_t = totalpackets;
      byte tdata[totalpackets];
      initial = false;
      Serial.printf("%d ", count);
    }
    else
    {
      tdata[totalpackets] = Serial2.read();
      Serial.printf("%02X ", tdata[count - 1]);
      totalpackets--;

      if (totalpackets == -1)
      {

        initial = true;
        Serial.println();
        for (int i = 0; i < count_t + 1; i++)
        {
          Serial.printf("%02X ", tdata[i]);
        }
        Serial.println();
        // Serial.printf("\n%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n\n", tdata[0], tdata[1], tdata[2], tdata[3], tdata[4], tdata[5], tdata[6], tdata[7], tdata[8], tdata[9], tdata[10], tdata[11], tdata[12], tdata[13], tdata[14], tdata[15], tdata[16], tdata[17], tdata[18], tdata[19], tdata[20]);
        break;
      }
    }
  }

  // do the task that is related to variable;
}