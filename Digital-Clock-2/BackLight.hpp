#include <Arduino.h>

#include "lcd_backlight.hpp"
#define LIGHT_SAMPLES 20

class BackLight {
private:
    unsigned int analogPin;
    unsigned long delayMs;
    unsigned long nextSampleMs;

    LCDBackLight backLight;
    int lightAverages[LIGHT_SAMPLES];  // Recorded list of light levels
    int lightAveragesPos = 0;          // The position in lightAverages for the next reading
    int average;
    int base;
    int cap;

public:
    BackLight() {
    }
    ~BackLight() {
    }

    int getAverage() {
        return average;
    }

    void update() {
        if (millis() > nextSampleMs) {
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
        update();
    }
};