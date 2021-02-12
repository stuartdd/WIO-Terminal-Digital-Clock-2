/*
 * Copyright (C) 2020 Stuart Davies
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * lcdLog.h
 * lcdLog.cpp
 *
 * Created on 13 December 2020
 *
 * @file lcdLog.cpp
*/

#ifndef WIO_LOG_CPP__
#define WIO_LOG_CPP__

#include "lcdLog.h"

static TFT_eSPI log_tft;
static int fgColor = TFT_BLACK;
static int bgColor = TFT_GREEN;

static char logList[LOG_LEN][LOG_WIDTH + 1];  // The log lines
static int logListPos = -1;                   // The line position in logList for the next log line
static int logSameLineCount = 0;              // True is the last log line overwrote the one before it (log same line).
static bool logRequiresRedraw = true;         // The log screen will not be redrawn unless this is true.
static bool logIsVisible = false;
static bool echoToSerial = false;

Logger::Logger() {
}

Logger::~Logger() {
    clearLog();
}

void Logger::init(TFT_eSPI tft, int fg, int bg) {
    setTFT(tft);
    setFgColor(fg);
    setBgColor(bg);
    clearLog();
}

void Logger::setLogIsVisible(bool isVisible) {
    logIsVisible = isVisible;
    logRequiresRedraw = isVisible;
}

void Logger::setTFT(TFT_eSPI tft) {
    log_tft = tft;
}

TFT_eSPI Logger::getTFT() {
    return log_tft;
}

int Logger::setFgColor(int color) {
    fgColor = color;
    return getFgColor();
}

int Logger::setBgColor(int color) {
    bgColor = color;
    return getBgColor();
}

int Logger::getFgColor() {
    return fgColor;
}

int Logger::getBgColor() {
    return bgColor;
}

bool Logger::getRequiresRedraw() {
    return logRequiresRedraw;
}

void Logger::setRequiresRedraw(bool isRedraw) {
    logRequiresRedraw = isRedraw;
}

bool Logger::getLogIsVisible() {
    return logIsVisible;
}

void Logger::setEchoToSerial(bool isEcho) {
    echoToSerial = isEcho;
}

bool Logger::getEchoSerial() {
    return echoToSerial;
}

void Logger::clearLog() {
    for (int i = 0; i < LOG_LEN; i++) {
        for (int j = 0; j < LOG_WIDTH; j++) {
            logList[i][j] = 32;
        }
        logList[i][LOG_WIDTH] = 0;
    }
    logListPos = -1;
    logSameLineCount = 0;
    logRequiresRedraw = true;
}

void Logger::logLine(String mess) {
    _logLine(mess, true);
    logSameLineCount = 0;
}

void Logger::logSameLine(String mess) {
    if (logSameLineCount == 0) {
        _logLine(mess, true);
    } else {
        _logLine(mess, false);
    }
    logSameLineCount++;
}

void Logger::display() {
    if (logRequiresRedraw) {
        logRequiresRedraw = false;
        log_tft.setTextColor(fgColor, bgColor);
        log_tft.setFreeFont(LOG_FONT);
        int pos = (logListPos + 1) % LOG_LEN;
        for (int i = 0; i < LOG_LEN; i++) {
            log_tft.drawString(logList[pos], LOG_MARGIN_LEFT, LOG_MARGIN_TOP + (i * LOG_LINE_HEIGHT));
            pos++;
            pos = pos % LOG_LEN;
        }
    }
}

/*
   Write a String to the log buffer. Optionally advance to the next line.
   Long strings will be truncated! Short Strings are padded with spaces.
   LOG_WIDTH is the number of chars that can be displayed per line.
   LOG_LEN is the number of lines in the log buffer.
   Log lines are lost when they go off the top of the screen.
*/
void Logger::_logLine(String mess, bool advance) {
    if (advance) {
        logListPos = (logListPos + 1) % LOG_LEN;
    }
    //
    // Copy the chars to the current line buffer 'logListAddLine'.
    // Note LOG_WIDTH is the screen width + 1 for a ZERO at the end
    // making it a C String. Also pad any trailng chars with a Space
    // to ensure it overwrites any previous screen data.
    //
    int len = mess.length() - 1;
    for (int i = 0; i < LOG_WIDTH; i++) {
        if (i > len) {
            logList[logListPos][i] = ' ';
        } else {
            logList[logListPos][i] = mess.charAt(i);
        }
        logList[logListPos][LOG_WIDTH] = 0;
    }
    //
    // If serial interface is available then echo the log.
    //
    if (echoToSerial) {
        Serial.println(mess);
    }
    //
    // Log has changed so flag that it needs a redraw.
    //
    logRequiresRedraw = true;
    //
    // Only display if the log is visible
    //
    if (logIsVisible) {
        display();
    }
}

#endif
