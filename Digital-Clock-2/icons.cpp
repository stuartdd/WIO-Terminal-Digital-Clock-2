#include "icons.h"

static TFT_eSPI tft;

IconManager::IconManager() {
}

IconManager::~IconManager() {
}

void IconManager::init(TFT_eSPI _tft, unsigned char const *_bits, const unsigned int _x, const unsigned int _y, const unsigned int _w, const unsigned int _h, const unsigned int _bgColor) {
    tft = _tft;
    bits = _bits;
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    bgColor = _bgColor;
}

void IconManager::draw(const unsigned int _color) {
    if (color != _color) {
        tft.drawXBitmap(x, y, bits, w, h, _color);
        color = _color;
    }
}

void IconManager::off() {
    tft.drawXBitmap(x, y, bits, w, h, bgColor);
}

void IconManager::invalidate() {
    color = 0;
}