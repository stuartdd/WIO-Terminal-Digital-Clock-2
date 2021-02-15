#include "icons.h"

static TFT_eSPI tft;

IconManager::IconManager() {
}

IconManager::~IconManager() {
}

void IconManager::init(TFT_eSPI _tft, unsigned char const *_bits, const unsigned int _x, const unsigned int _y, const unsigned int _w, const unsigned int _h, const unsigned int _color, const unsigned int _bgColor) {
    tft = _tft;
    bits = _bits;
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    color = _color;
    bgColor = _bgColor;
    subIcon = NULL;
    requiresRedraw = true;
    visible = true;
}

void IconManager::draw() {
    if (subIcon) {
        if (subIcon->isNotVisible()) {
            subIcon->eraseRaw();
        }
    }
    drawRaw();
    if (subIcon) {
        if (subIcon->isVisible()) {
            subIcon->drawRaw();
        }
    }
}

void IconManager::drawWithColor(const unsigned int _color) {
    if ((color != _color) || (getRequiresRedraw())) {
        color = _color;
        draw();
    }
}

void IconManager::erase() {
    if (subIcon) {
        subIcon->eraseRaw();
    }
    eraseRaw();
}

void IconManager::setOverlay(IconManager *_subIcon) {
    subIcon = _subIcon;
    requiresRedraw = true;
}

void IconManager::setOverlayVisible(bool _visible) {
    if (subIcon) {
        subIcon->setVisible(_visible);
    }
}

bool IconManager::isOverlayVisible() {
    if (subIcon) {
        return subIcon->isVisible();
    }
    return false;
}

void IconManager::setOverlayColor(const unsigned int _color) {
    if (subIcon) {
        subIcon->setColor(_color);
    }
}

bool IconManager::getRequiresRedraw() {
    if (requiresRedraw) {
        return true;
    }
    if (subIcon) {
        return subIcon->getRequiresRedraw();
    }
    return false;
}

void IconManager::setVisible(bool _visible) {
    if (visible != _visible) {
        requiresRedraw = true;
    }
    visible = _visible;
}

bool IconManager::isVisible() {
    return visible;
}

bool IconManager::isNotVisible() {
    return !visible;
}

void IconManager::setColor(const unsigned int _color) {
    if (color != _color) {
        requiresRedraw = true;
    }
    color = _color;
}

int IconManager::getColor() {
    return color;
}

int IconManager::getBgColor() {
    return bgColor;
}

void IconManager::invalidate() {
    requiresRedraw = true;
}

void IconManager::eraseRaw() {
    tft.drawXBitmap(x, y, bits, w, h, bgColor);
    requiresRedraw = false;
}

void IconManager::drawRaw() {
    tft.drawXBitmap(x, y, bits, w, h, color);
    requiresRedraw = false;
}