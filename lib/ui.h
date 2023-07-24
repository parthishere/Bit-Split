#ifndef __ui_h__
#define __ui_h__


class UI{
    void ui(char *message, int index, int strength);
    void drawWiFiBars(int x, int y, int signal);
    void drawBatteryLevel(int x, int y, float adc_value);
    void modee();
};


#endif