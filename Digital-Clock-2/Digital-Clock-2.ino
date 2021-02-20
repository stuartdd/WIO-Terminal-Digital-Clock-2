// #define TEST_SMALL__ X
//
// pyserial-miniterm /dev/ttyACM0 115200 --exit-char 32
// 320x240
//
//
// Uncomment the next line to build in TEST mode
//#define TEST
//

#include "BackLight.hpp"
#include "Free_Fonts.h"
#include "SAMCrashMonitor.h"
#include "SEG_S.h"
#include "TFT_eSPI.h"
#include "TimeServer.h"
#include "icons.h"
#include "lcdLog.h"
#include "properties.h"

#define SYNC_TIME_OUT_SHORT 1000
#define SYNC_TIME_OUT_LONG 15000
#define WIFI_TIME_OUT_SHORT 10000
#define WIFI_TIME_OUT_LONG 30000
#define BACK_LIGHT_LO_THRESH 3
#define BACK_LIGHT_HI_THRESH 3

#define TIME_SYNC_RETRY_TIME 15000
#ifdef TEST
#define TIME_SYNC_TIME 30000
#else
#define TIME_SYNC_TIME 3600000
#endif

#define ICON_BULB_X_POS 30
#define ICON_BULB_Y_POS 160
#define ICON_WIFI_X_POS 30
#define ICON_WIFI_Y_POS 105
#define DATE_BLOCK_Y_POS 240
#define DATE_BLOCK_W 240
#define DATE_BLOCK_H 80
#define STATUS_BLOCK_Y_POS 200
#define STATUS_BLOCK_W 240
#define STATUS_BLOCK_H 40

#define DST_OVERRIDE_PROPERTY_NAME "overrideDST"
#define SSID_PROPERTY_NAME "ssid"
#define PW_PROPERTY_NAME "pw"
#define BACKLIGHT_PROPERTY_NAME "backlight"

enum DisplayModes {
    DM_CLOCK,     // Primary display mode is the CLOCK
    DM_LOG,       // Primary display mode is the LOG
    DM_SETTINGS,  // Primary display mode is the SETTINGS
    DM_FAIL       // Primary display mode is the FAIL screen
};
static DisplayModes currentMode;
static TFT_eSPI dispBuffer;  // The screen (This should be a sprite but there is not enough memory)
static TimeServer timeServer;
static TimeStatus timeStatus;
static Logger logger = Logger();
static BackLight backLight = BackLight();
static Properties properties = Properties();
static ClockDisplaySmall segSmallHHMM = ClockDisplaySmall();
static ClockDisplaySmall segSmallSS = ClockDisplaySmall();
static IconManager wifiIcon = IconManager();
static IconManager dstIcon = IconManager();
static IconManager bulbIcon = IconManager();

const char* ntpServerName = "time.nist.gov";
const int offsets[]{3, 0, -5, 3};
const int logBgColor = dispBuffer.color565(0, 0, 255);
const int logFgColor = dispBuffer.color565(255, 255, 255);
const int bgColor = dispBuffer.color565(0, 0, 0);
const int segOffFgColor = dispBuffer.color565(0, 0, 50);
const int fgColorHi = dispBuffer.color565(0, 180, 0);
const int fgColorLo = dispBuffer.color565(0, 100, 0);
const int iconBusyColor = dispBuffer.color565(255, 255, 0);
const int iconConnectColor = dispBuffer.color565(0, 0, 150);
const int iconFailColor = dispBuffer.color565(255, 0, 0);
const int iconOffColor = dispBuffer.color565(80, 0, 0);
const int iconOnColor = dispBuffer.color565(0, 180, 0);

long millisNow = 0;
long timerSyncTime = 0;
long timerOneSecond = 0;
long timeButtonTest = 0;
long mainLoopMillis = 0;
long mainLoopStart = 0;
long mainLooptemp = 0;
bool useNTPForTime = false;
bool lowLightDisplay = false;
bool colonOn = true;

bool buttonANotPressed = true;
bool buttonBNotPressed = true;
bool buttonCNotPressed = true;
bool buttonMiddleNotPressed = true;
int count = 0;
int currentDayOfWeek = -1;
int currentClockColor = -1;
int syncTimeTimeout = SYNC_TIME_OUT_SHORT;
#ifdef TEST
bool displayStats = true;
#else
bool displayStats = false;
#endif
char message[50];
char strTime[40];
char ipStr[20];
char ssid[40];      // Read from a file (FILE_SSID or FILE_SSID_ALT) on the sd card.
char password[40];  // Read from a file (FILE_PW or FILE_PW_ALT) on the sd card.

void setup() {
    //
    // Set up the Serial port monitor first!
    //
    Serial.begin(115200);
    long tryFor = millis() + 2000;
    while (!Serial) {
        if (tryFor < millis()) {
            break;
        }
        delay(100);
    }

    SAMCrashMonitor::begin();
    SAMCrashMonitor::disableWatchdog();  // Make sure it is turned off during init.
    SAMCrashMonitor::dump();             // Dump watchdog reset data to the console.

    pinMode(WIO_LIGHT, INPUT);
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);
    pinMode(WIO_5S_UP, INPUT_PULLUP);
    pinMode(WIO_5S_DOWN, INPUT_PULLUP);
    pinMode(WIO_5S_LEFT, INPUT_PULLUP);
    pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);
    pinMode(WIO_BUZZER, OUTPUT);

    //
    // Set up the Display so we can see the logger
    //
    dispBuffer.begin();
    backLight.init(WIO_LIGHT, 3, 15, 500);
    //
    // Display the log page
    //
    setDisplayMode(DM_LOG);

    //
    // Set up the log
    //
    logger.init(dispBuffer, logFgColor, logBgColor);
    //
    // Logger needs to know if it can use the Serial port.
    //
    if (Serial) {
        logger.setEchoToSerial(true);
        logger.logLine("Serial OK: 115200");
    } else {
        logger.setEchoToSerial(false);
        logger.logLine("Serial not connected:");
    }

    //
    // Connect to the SD card. If not then notify and freeze!
    //
    if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
        freeze("SD init failed!", false);
    }
    //
    // Get the SSID and the password from the SD card.
    //   If either is not there then notify and freeze!
    //
    if (properties.readString(ssid, SSID_PROPERTY_NAME, "") == 0) {
        sprintf(message, "prop '%s' not found", SSID_PROPERTY_NAME);
        freeze(message, false);
    }
    if (properties.readString(password, PW_PROPERTY_NAME, "") == 0) {
        sprintf(message, "prop '%s' not found", PW_PROPERTY_NAME);
        freeze(message, false);
    }

    backLight.setFullBackLightOn(properties.readBool(BACKLIGHT_PROPERTY_NAME, false));
    logger.logLine("Backlight is full " + String(backLight.isFullBackLightOn() ? "YES" : "NO"));

    //
    // Check that the SSID is reachable.
    //  if not notify and freeze!
    //
    logger.logLine("Checking SSID:" + String(ssid));
    if (!timeServer.testForSsid(ssid)) {
        sprintf(message, "SSID '%s' not found", ssid);
        freeze(message, true);
    }
    logger.logLine("Found SSID:" + String(ssid));
    //
    // Time Server manages the connection.
    // Time status manages the time data.
    //
    timeServer = TimeServer();
    timeStatus = TimeStatus();
    //
    // Connect to the WiFi via TimeServer.
    // Use long timeout for initial connect.
    //   If fails then give up.
    //     This should cause the watchdog to reset the device.
    //
    logger.setEchoToSerial(Serial);
    logger.logLine("Connecting to:" + String(ssid));
    timeServer.connect(ssid, password, WIFI_TIME_OUT_LONG);
    if (timeServer.isNotConnected()) {
        freeze(timeServer.getMessageStr(message), true);
    }
    logger.logLine(timeServer.getMessageStr(message));
    logger.setEchoToSerial(Serial);
    logger.logLine("Initial Time Fetch");
    //
    // Fetch the time!
    // Use long timeout for initial fetches.
    // Prefer worldtimeapi as we get the daylight saving info as well.
    //
    if (!timeServer.getTimeFromWWW(timeStatus, SYNC_TIME_OUT_LONG)) {
        //
        // If worldtimeapi is not there then use NTP
        //
        if (!timeServer.getNTPTime(timeStatus, ntpServerName, SYNC_TIME_OUT_LONG)) {
            //
            // If NTP is not there then give up!
            // This should cause the watchdog to reset the device.
            //
            freeze(timeStatus.getMessageStr(message), true);
        }
    }
    logger.logLine(timeStatus.getMessageStr(message));

    timeStatus.setDSTOverriden(properties.readBool(DST_OVERRIDE_PROPERTY_NAME, false));
    logger.logLine("Override DST is " + String(timeStatus.isDSTOverriden() ? "ON" : "OFF"));
    //
    // Init the seven segment displays
    //
    segSmallHHMM.init(dispBuffer, 10, 10, getClockColor(), segOffFgColor, 5, offsets);
    segSmallSS.init(dispBuffer, 115, 105, getClockColor(), segOffFgColor, 2, offsets);
    bulbIcon.init(dispBuffer, bulb_icon_bits, ICON_BULB_X_POS, ICON_BULB_Y_POS + (wifi_icon_height - bulb_icon_height), bulb_icon_width, bulb_icon_height, TFT_RED, bgColor);
    dstIcon.init(dispBuffer, dst_icon_bits, ICON_WIFI_X_POS + (wifi_icon_width - dst_icon_width), ICON_WIFI_Y_POS + (wifi_icon_height - dst_icon_height), dst_icon_width, dst_icon_height, TFT_RED, bgColor);
    wifiIcon.init(dispBuffer, wifi_icon_bits, ICON_WIFI_X_POS, ICON_WIFI_Y_POS, wifi_icon_width, wifi_icon_height, TFT_RED, bgColor);
    wifiIcon.setOverlay(&dstIcon);

    int timeout = SAMCrashMonitor::enableWatchdog(20000);
    logger.logLine("Watchdog is " + String(timeout) + "ms");
    //
    // Display the clock face.
    //
    setDisplayMode(DM_CLOCK);
    //
    // Set time for 'time sync' in the main loop.
    //
    timerSyncTime = millis() + TIME_SYNC_TIME;
}  // END setup

void loop() {
    millisNow = millis();
    mainLoopStart = millisNow;

    SAMCrashMonitor::iAmAlive();

    backLight.update();
    //
    // Call millis() once to get the time at the start of the loop.
    //
    if (millisNow > timeButtonTest) {
        //
        // Time to check those buttons
        //
        timeButtonTest = millisNow + 100;
        if (digitalRead(WIO_KEY_A) == LOW) {
            if (buttonANotPressed) {
                buttonANotPressed = false;
                if (currentMode == DM_CLOCK) {
                    setDisplayMode(DM_LOG);
                } else {
                    setDisplayMode(DM_CLOCK);
                }
            }
        } else {
            buttonANotPressed = true;
        }

        if (digitalRead(WIO_KEY_B) == LOW) {
            if (buttonBNotPressed) {
                buttonBNotPressed = false;
                bool overriden = !timeStatus.isDSTOverriden();
                properties.writeBool(DST_OVERRIDE_PROPERTY_NAME, overriden);
                timeStatus.setDSTOverriden(overriden);
                logger.logLine("Override DST is " + String(timeStatus.isDSTOverriden() ? "ON" : "OFF"));
                mainLoopMillis = 0;
                updateDstIcon();
            }
        } else {
            buttonBNotPressed = true;
        }

        if (digitalRead(WIO_KEY_C) == LOW) {
            if (buttonCNotPressed) {
                buttonCNotPressed = false;
                backLight.toggleFullBackLight();
                properties.writeBool(BACKLIGHT_PROPERTY_NAME, backLight.isFullBackLightOn());
                logger.logLine("Backlight is full " + String(backLight.isFullBackLightOn() ? "YES" : "NO"));
            }
        } else {
            buttonCNotPressed = true;
        }

        if (digitalRead(WIO_5S_PRESS) == LOW) {
            if (buttonMiddleNotPressed) {
                buttonMiddleNotPressed = false;
                displayStats = !displayStats;
                if (currentMode == DM_CLOCK) {
                    currentMode = DM_LOG;
                    setDisplayMode(DM_CLOCK);
                }
            }
        } else {
            buttonMiddleNotPressed = true;
        }
    }

    if (millisNow > timerSyncTime) {
        //
        // Time to synchronise with the time servers
        //
        timerSyncTime = millisNow + TIME_SYNC_TIME;
        wifiIcon.drawWithColor(iconBusyColor);

        logger.setEchoToSerial(Serial);
        logger.logSameLine(String(WiFi.status()) + ":TIME:" + String(timeStatus.getTimeStr(message, TF_HHMMSS)));
        //
        // If for some reason we are no longer connected to the WiFi.
        //    Re-connect.
        //
        if (timeServer.isNotConnected()) {
            wifiIcon.drawWithColor(iconConnectColor);
            logger.logLine("Reconnecting:" + String(timeStatus.getTimeStr(message, TF_HHMMSS)));
            //
            // Connect.
            //
            timeServer.connect(ssid, password, WIFI_TIME_OUT_SHORT);
        }
        //
        // Do not fail if the connect fails.
        // Just bring forward the next time sync to 30 seconds from now.
        // We have a time so we can carry on displaying it.
        //
        if (timeServer.isNotConnected()) {
            wifiIcon.drawWithColor(iconFailColor);
            logger.logLine(timeServer.getMessageStr(message));
            timerSyncTime = millis() + TIME_SYNC_RETRY_TIME;
        } else {
            if (useNTPForTime) {
                timeServer.getNTPTime(timeStatus, ntpServerName, syncTimeTimeout);
            } else {
                timeServer.getTimeFromWWW(timeStatus, syncTimeTimeout);
            }
            //
            // Do not fail if the getTimeFromWWW or getNTPTime fails.
            // Just bring forward the next time sync to 30 seconds from now.
            // If it timed out then use NTP Next time.
            //
            if (timeStatus.timeCode != TC_TIME_FOUND) {
                useNTPForTime = true;
                syncTimeTimeout = SYNC_TIME_OUT_LONG;
                timerSyncTime = millis() + TIME_SYNC_RETRY_TIME;
                wifiIcon.drawWithColor(iconFailColor);
            } else {
                useNTPForTime = false;
                syncTimeTimeout = SYNC_TIME_OUT_SHORT;
                wifiIcon.drawWithColor(getClockColor());
            }
            logger.logLine(timeStatus.getMessageStr(message));
        }
    }

    if (millisNow > timerOneSecond) {
        timerOneSecond = millisNow + 1000;
        if (currentMode == DM_CLOCK) {
            dispBuffer.setFreeFont(FF7);
            if (currentClockColor != getClockColor()) {
                currentClockColor = getClockColor();
                currentDayOfWeek = -1;  // Force day data to redisplay in the new colour.
            }
            //
            // Load up the time and display it
            //
            const byte* time = timeStatus.updateDateTime();
            segSmallHHMM.setValue(0, time[0]);
            segSmallHHMM.setValue(1, time[1]);
            colonOn = segSmallHHMM.setColon(2, colonOn);
            segSmallHHMM.setValue(3, time[2]);
            segSmallHHMM.setValue(4, time[3]);
            segSmallHHMM.setFgColor(currentClockColor);
            segSmallSS.setValue(0, time[4]);
            segSmallSS.setValue(1, time[5]);
            segSmallSS.setFgColor(currentClockColor);

            segSmallSS.draw();
            segSmallHHMM.draw();
            if (currentDayOfWeek != timeStatus.dayOfWeek) {
                currentDayOfWeek = timeStatus.dayOfWeek;
                dispBuffer.fillRect(0, DATE_BLOCK_Y_POS, DATE_BLOCK_W, DATE_BLOCK_H, bgColor);
                dispBuffer.setTextColor(currentClockColor, bgColor);
                dispBuffer.drawString(timeStatus.getDayOfWeekStr(), 20, DATE_BLOCK_Y_POS + 5);
                dispBuffer.drawString(timeStatus.getMonthStr(), 20, DATE_BLOCK_Y_POS + 45);
                dispBuffer.drawString(timeStatus.getDayOfMonthStr(message), 100, DATE_BLOCK_Y_POS + 45);
            }
            updateDstIcon();
            wifiIcon.drawWithColor(timeServer.isNotConnected() ? iconConnectColor : currentClockColor);
            bulbIcon.drawWithColor(backLight.isFullBackLightOn() ? currentClockColor : iconOffColor);
            if (displayStats) {
                dispBuffer.setFreeFont(FF23);
                dispBuffer.setTextColor(logFgColor, bgColor);
                dispBuffer.fillRect(0, STATUS_BLOCK_Y_POS, STATUS_BLOCK_W, STATUS_BLOCK_H, bgColor);
                dispBuffer.drawString(String(backLight.getAverage()) + "|" + String(mainLoopMillis) + "|" + String((timerSyncTime - millis()) / 1000), 5, STATUS_BLOCK_Y_POS + 3);
            }
        }
        if (currentMode == DM_LOG) {
            timeStatus.updateDateTime();
            logger.logSameLine(String(WiFi.status()) + ":TIME:" + String(timeStatus.getTimeStr(message, TF_HHMMSS)));
        }
    }
    mainLooptemp = millis() - mainLoopStart;
    if (mainLooptemp > mainLoopMillis) {
        mainLoopMillis = mainLooptemp;
    }
}  // END loop

void setDisplayMode(DisplayModes newMode) {
    if (currentMode != newMode) {
        currentMode = newMode;
        logger.setLogIsVisible(currentMode == DM_LOG);
        switch (currentMode) {
        case DM_LOG:
            dispBuffer.setRotation(3);
            dispBuffer.fillScreen(bgColor);
            break;
        case DM_CLOCK:
            dispBuffer.setRotation(2);
            dispBuffer.fillScreen(bgColor);
            segSmallHHMM.invalidate();
            segSmallSS.invalidate();
            wifiIcon.invalidate();
            bulbIcon.invalidate();
            currentDayOfWeek = -1;
            mainLoopMillis = 0;
            currentClockColor = TFT_BLACK;
            break;
        }
    }
}

void freeze(const char* msg, bool restart) {
    setDisplayMode(DM_LOG);
    logger.setEchoToSerial(Serial);
    logger.logLine(String(msg));
    if (restart) {
        int timeout = SAMCrashMonitor::enableWatchdog(20000);
        logger.logLine("Reboot in " + String(timeout) + "ms");
    } else {
        logger.logLine("HALTED:");
    }
    while (1) {
    }
}

void updateDstIcon() {
    if (timeStatus.isDSTOverriden()) {
        dstIcon.setColor(TFT_RED);
        dstIcon.setVisible(true);
        return;
    }
    if (timeStatus.isDstOn()) {
        dstIcon.setColor(TFT_YELLOW);
        dstIcon.setVisible(true);
        return;
    }
    dstIcon.setVisible(false);
}

int getClockColor() {
    int avg = backLight.getAverage();
    if ((avg < BACK_LIGHT_LO_THRESH) && (!lowLightDisplay)) {
        lowLightDisplay = true;
    }
    if ((avg > BACK_LIGHT_HI_THRESH) && (lowLightDisplay)) {
        lowLightDisplay = false;
    }
    if (lowLightDisplay) {
        return fgColorLo;
    }
    return fgColorHi;
}
