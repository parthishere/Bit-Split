// #include "ui.h"

// int change_delay(int intensity)
// {
//   switch (intensity)
//   {

//   case -1:
//     digitalWrite(buzPin, 0);
//     return -1;

//   case 1:
//     temp_delay_buz = 600; // OFF Time
//     return 100;           // ON TIME

//   case 2:
//     temp_delay_buz = 320; // OFF Time
//     return 180;           // ON TIME

//   case 3:
//     temp_delay_buz = 160; // OFF Time
//     return 240;           // ON TIME

//   case 4:
//     temp_delay_buz = 299; // OFF Time
//     return 1;             // ON TIME

//   default:
//     return -1;
//   }
// }

// void ui(char *message, int index, int strength) // number = number of box we want to print at screen , strength = signal strength of wifi
// {
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 0);
//   display.print("MODE : ");
//   display.print(String(mode));
//   display.drawLine(0, 13, 128, 13, WHITE);
//   display.display();
//   drawBatteryLevel(96, 0, analogRead(potPin));

//   const int MESSAGE_WIDTH = 126;
//   const int MESSAGE_HEIGHT = 15;
//   const int MESSAGE_SPACING = 2;
//   const int WIFI_WIDTH = 20;
//   const int WIFI_HEIGHT = 10;
//   display.fillRect(1, 17 + (MESSAGE_HEIGHT * (index - 1)) + (MESSAGE_SPACING * (index - 1)), 128 - 1, (MESSAGE_HEIGHT + MESSAGE_SPACING), BLACK);
//   display.display();
//   // Calculate the x and y coordinates of the message and wifi icon
//   int x = 1;
//   int y = 17;
//   switch (index)
//   {
//   case 1:
//     x = 1;
//     y = 17;
//     break;
//   case 2:
//     x = 1;
//     y = 17 + MESSAGE_HEIGHT + MESSAGE_SPACING;
//     break;
//   case 3:
//     x = 1;
//     y = 17 + (MESSAGE_HEIGHT * 2) + (MESSAGE_SPACING * 2);
//     break;
//   default:
//     break;
//   }

//   // Draw the wifi signal strength icon
//   drawWiFiBars(MESSAGE_WIDTH - WIFI_WIDTH - 5 - 1, y, strength);

//   // Draw the message text
//   display.setTextSize(1);
//   display.setTextColor(WHITE);

//   // Serial.println(((index + 1) * (MESSAGE_HEIGHT + MESSAGE_SPACING)) + 17);
//   display.setCursor(x, y);
//   display.print(message);

//   display.display();
// }

// void drawWiFiBars(int x, int y, int signal)
// {

//   // Fixed width and height of each bar
//   const int BAR_WIDTH = 4;
//   const int BAR_HEIGHT = 2;

//   display.fillRect(x, y, 4 * 4 + 4, 10, BLACK);

//   // Draw the bars
//   for (int i = 0; i < 4; i++)
//   {
//     int numBars = i + 1;
//     int barX = x + i * (BAR_WIDTH + 1);
//     int barY = y + BAR_HEIGHT * 3;
//     if (signal >= numBars)
//     {
//       display.fillRect(barX, barY - (numBars - 1) * BAR_HEIGHT, BAR_WIDTH, numBars * BAR_HEIGHT, WHITE);
//     }
//     else
//     {
//       display.drawRect(barX, barY - (numBars - 1) * BAR_HEIGHT, BAR_WIDTH, numBars * BAR_HEIGHT, WHITE);
//     }
//   }
//   display.display();
// }

// void drawBatteryLevel(int x, int y, float adc_value)
// {
//   const int BAR_SPACING = 2;
//   const int BAR_MAX_WIDTH = 25 - (BAR_SPACING * 2);
//   display.fillRect(x, y, 25 + 3, 10, BLACK);

//   // Calculate the battery level as a percentage
//   int level = map(adc_value, 0, 4095, 0, 100);

//   // Calculate the width of the filled bar
//   int filledWidth = map(level, 0, 100, 0, BAR_MAX_WIDTH);

//   // Draw the battery icon
//   display.drawRect(x, y, 25, 10, WHITE);
//   display.fillRect(x + BAR_SPACING, y + BAR_SPACING, filledWidth, 10 - (BAR_SPACING * 2), WHITE);
//   display.fillRect(x + 25, y + 3, 3, 4, WHITE);
//   display.display();
// }

// void modee()
// {
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.fillRect(0, 0, 64, 12, BLACK);
//   display.setCursor(0, 0);
//   display.print("MODE : ");
//   display.print(String(mode));
//   display.drawLine(0, 13, 128, 13, WHITE);

//   drawBatteryLevel(96, 0, analogRead(potPin));
//   display.fillRect(1, 17, 128 - 1, 64 - 17, BLACK);
// }
