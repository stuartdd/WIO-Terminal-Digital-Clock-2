#include <Arduino.h>

#include "Free_Fonts.h"
#include "TFT_eSPI.h"
/*
  Constants for the log screen
*/
#define LOG_LEN 13
#define LOG_WIDTH 30
#define LOG_MARGIN_TOP 2
#define LOG_MARGIN_LEFT 2
#define LOG_LINE_HEIGHT 18
#define LOG_FONT FF5

class Logger {
private:
    void _logLine(String mess, bool advance);

public:
    Logger();
    ~Logger();
    void init(TFT_eSPI tft, int fg, int bg);
    void clearLog();
    void logLine(String mess);
    void logLineDisplay(String mess);
    void logSameLine(String mess);
    void display();

    void setTFT(TFT_eSPI tft);
    int setFgColor(int color);
    int setBgColor(int color);
    void setLogIsVisible(bool isVisible);
    void setEchoToSerial(bool isEcho);
    void setRequiresRedraw(bool isRedraw);

    TFT_eSPI getTFT();
    int getFgColor();
    int getBgColor();
    bool getLogIsVisible();
    bool getEchoSerial();
    bool getRequiresRedraw();
};
