#ifndef __algorithms_h__
#define __algorithms_h__

#include <Arduino.h>

class Algorithm{

    private:
        void swap(int *a, int *b, byte c[], byte d[]);
        int partition(int rssi[], byte epc[][12], int low, int high);

    public:
        void quickSort(int rssi[], byte epc[][12], int low, int high);

};

#endif