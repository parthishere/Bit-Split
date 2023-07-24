#include "algorithms.h"

void Algorithm::swap(int *a, int *b, byte c[], byte d[])
{
    int t = *a;
    *a = *b;
    *b = t;

    byte temp[12];
    memcpy(temp, c, 12);
    memcpy(c, d, 12);
    memcpy(d, temp, 12);
}

int Algorithm::partition(int rssi[], byte epc[][12], int low, int high)
{

  int pivot = rssi[high];
  int i = (low - 1);

  for (int j = low; j < high; j++)
  {
    if (rssi[j] <= pivot)
    {

      i++;
      swap(&rssi[i], &rssi[j], epc[i], epc[j]);
    }
  }

  swap(&rssi[i + 1], &rssi[high], epc[i + 1], epc[high]);

  return (i + 1);
}

void Algorithm::quickSort(int rssi[], byte epc[][12], int low, int high)
{
    if (low < high)
    {
        int pi = partition(rssi, epc, low, high);

        quickSort(rssi, epc, low, pi - 1);

        quickSort(rssi, epc, pi + 1, high);
    }
}