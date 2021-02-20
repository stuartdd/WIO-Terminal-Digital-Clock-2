#ifndef _LCD_BACK_LIGHT_CPP__
#define _LCD_BACK_LIGHT_CPP__
#include <Arduino.h>

#include "lcd_backlight.hpp"
#define LIGHT_SAMPLES 20

class BackLight {
private:
    LCDBackLight backLight;
    unsigned int analogPin;
    unsigned long delayMs;
    unsigned long nextSampleMs;
    bool fullBacklightOn = false;

    int average;
    int base;
    int cap;
    int lightAverages[LIGHT_SAMPLES];  // Recorded list of light levels
    int lightAveragesPos = 0;          // The position in lightAverages for the next reading

public:
    BackLight() {
    }
    ~BackLight() {
    }

    int getAverage() {
        return average;
    }

    void toggleFullBackLight() {
        fullBacklightOn = !fullBacklightOn;
    }

    void setFullBackLightOn(bool isOn) {
        fullBacklightOn = isOn;
    }

    bool isFullBackLightOn() {
        return fullBacklightOn;
    }

    void update() {
        if (millis() > nextSampleMs) {
            if (fullBacklightOn) {
                backLight.setBrightness(cap);
            } else {
                nextSampleMs = millis() + delayMs;
                lightAverages[lightAveragesPos] = analogRead(analogPin);
                lightAveragesPos++;
                if (lightAveragesPos >= LIGHT_SAMPLES) {
                    lightAveragesPos = 0;
                }
                int acc = 0;
                int count = 0;
                int v = 0;
                for (int i = 0; i < LIGHT_SAMPLES; i++) {
                    v = lightAverages[i];
                    if (v >= 0) {
                        count++;
                        acc += v;
                    }
                }
                average = (acc / count) - base;
                if (average < 1) {
                    average = 1;
                };
                if (average > cap) {
                    average = cap;
                };
                backLight.setBrightness(average);
            }
        }
    }

    void init(unsigned int _analogPin, int _base, int _cap, unsigned long _delayMs) {
        backLight.initialize();
        analogPin = _analogPin;
        base = _base;
        cap = _cap;
        delayMs = _delayMs;
        lightAveragesPos = 0;
        for (int i = 0; i < LIGHT_SAMPLES; i++) {
            lightAverages[i] = -1;
        }
        nextSampleMs = 0;
        average = 100;
        fullBacklightOn = false;
        update();
    }
};

#endif