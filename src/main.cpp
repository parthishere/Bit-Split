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
byte totalpackets, tdata[20];
;
int temp, intNumber, count = 0;
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
  byte tdata[] = {};
  while (Serial2.available())
  {
    if (initial)
    {
      Serial.println("\n\ninitialized");
      totalpackets = Serial2.read();
      Serial.printf("%02X\n", totalpackets);
      sprintf(receivedByte, "0x%02X", totalpackets);
      intNumber = strtol(receivedByte, &p, 16);
      count = intNumber + 1;
      byte tdata[count];
      initial = false;
    }
    else
    {
      tdata[count - 1] = Serial2.read();
      count--;
    }

    if (count == 0)
    {
      initial = true;
      // Just for printing the data we wont print the data hardcoded
      SerialBT.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X", tdata[0], tdata[1], tdata[2], tdata[3], tdata[4], tdata[5], tdata[6], tdata[7], tdata[8], tdata[9], tdata[10], tdata[11], tdata[12], tdata[13], tdata[14], tdata[15], tdata[16], tdata[17], tdata[18], tdata[19], tdata[20]);
      Serial.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X", tdata[0], tdata[1], tdata[2], tdata[3], tdata[4], tdata[5], tdata[6], tdata[7], tdata[8], tdata[9], tdata[10], tdata[11], tdata[12], tdata[13], tdata[14], tdata[15], tdata[16], tdata[17], tdata[18], tdata[19], tdata[20]);
      break;
    }
  }

  // do the task that is related to variable;
}