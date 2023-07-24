#ifndef __crc_h__
#define __crc_h__

#include <Arduino.h>

class Crc{

    private:
        

    public:
        unsigned short calcCRC(unsigned char data[], unsigned char len);
        void sendHexSerial(unsigned short value);

};

#endif