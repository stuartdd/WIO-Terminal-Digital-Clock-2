
#ifndef __CLOCK_ICONS_CPP__
#define __CLOCK_ICONS_CPP__

#include "TFT_eSPI.h"

#define wifi_icon_width 70
#define wifi_icon_height 75
const unsigned char wifi_icon_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x1f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xe0, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff,
    0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x0f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfc, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x00,
    0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x01, 0x00, 0xf8, 0x00,
    0x00, 0x00, 0x0f, 0x80, 0xff, 0x03, 0x00, 0xf8, 0x01, 0x00, 0xc0, 0xff,
    0x00, 0xff, 0x03, 0x00, 0xfc, 0x03, 0x00, 0xc0, 0xff, 0x03, 0xfe, 0x07,
    0x00, 0xfe, 0x07, 0x00, 0xe0, 0xff, 0x0f, 0xfc, 0x07, 0x00, 0xfe, 0x0f,
    0x00, 0xe0, 0xff, 0x1f, 0xf8, 0x07, 0x00, 0xff, 0x1f, 0x00, 0xc0, 0xff,
    0x3f, 0xf8, 0x0f, 0x00, 0xff, 0x3f, 0x00, 0xc0, 0xff, 0x7f, 0xf0, 0x0f,
    0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0xf0, 0x0f, 0x00, 0xff, 0xff,
    0x00, 0x00, 0xe0, 0xff, 0xe0, 0x1f, 0x80, 0xff, 0xff, 0x01, 0x00, 0x80,
    0xff, 0xe0, 0x1f, 0x80, 0xff, 0xff, 0x03, 0x00, 0x00, 0xff, 0xe0, 0x1f,
    0x80, 0xff, 0xff, 0x07, 0x00, 0x00, 0xff, 0xe1, 0x1f, 0x80, 0xff, 0xff,
    0x1f, 0x00, 0x00, 0xfe, 0xe1, 0x1f, 0x80, 0xff, 0xff, 0x3f, 0x00, 0x0f,
    0xfe, 0xe1, 0x1f, 0x80, 0xff, 0xff, 0x7f, 0x80, 0x1f, 0xfe, 0xc1, 0x1f,
    0x80, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xfe, 0xc1, 0x1f, 0x80, 0xff, 0xff,
    0xff, 0xe1, 0x1f, 0xfe, 0xc1, 0x1f, 0x80, 0xff, 0xff, 0xff, 0xf3, 0x0f,
    0xfc, 0xc1, 0x1f, 0x80, 0xff, 0xff, 0xff, 0xff, 0x07, 0xfc, 0xc0, 0x0f,
    0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0x78, 0x80, 0x07, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff,
    0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x03,
    0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00,
    0x00, 0xfc, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
    0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00,
    0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff,
    0x03, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00,
    0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xff,
    0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff,
    0x3f, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00,
    0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xc0, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x03, 0x00, 0x00, 0xe0, 0x9f, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00,
    0x00, 0xe0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xf0, 0x0f,
    0xfe, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xf8, 0x1f, 0xf8, 0xff, 0xff,
    0xff, 0x07, 0x00, 0x00, 0xfe, 0xff, 0xe0, 0xff, 0xff, 0xff, 0x03, 0x00,
    0x80, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0x00, 0x00, 0xc0, 0xff, 0xff,
    0x03, 0xfe, 0xff, 0x3f, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x07, 0xe0, 0xff,
    0x0f, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x0f, 0x00, 0x7c, 0x00, 0x00, 0x00,
    0xf8, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff,
    0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff,
    0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff,
    0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00};

#define dst_icon_width 41
#define dst_icon_height 37
const unsigned char dst_icon_bits[] = {
    0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x01, 0x00, 0x00,
    0x00, 0xfe, 0xff, 0x07, 0x00, 0x00, 0x00, 0xff, 0xff, 0x1f, 0x00, 0x00,
    0xc0, 0xff, 0xff, 0x3f, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x7f, 0x00, 0x00,
    0xf0, 0x1f, 0x80, 0xff, 0x00, 0x00, 0xf8, 0x07, 0x00, 0xfe, 0x01, 0x00,
    0xf8, 0x03, 0x00, 0xfc, 0x01, 0x00, 0xfc, 0x01, 0x00, 0xf8, 0x03, 0x00,
    0xfc, 0x00, 0x0e, 0xf0, 0x07, 0x00, 0x7e, 0x00, 0x1f, 0xe0, 0x07, 0x00,
    0x7e, 0x00, 0x1f, 0xc0, 0x07, 0x00, 0x3e, 0x00, 0x1f, 0xf8, 0xef, 0x00,
    0x3f, 0x00, 0x1f, 0xfc, 0xff, 0x01, 0x3f, 0x00, 0x1f, 0xfc, 0xff, 0x01,
    0x1f, 0x00, 0x1f, 0xfc, 0xff, 0x01, 0x1f, 0x00, 0x1f, 0xfc, 0xff, 0x00,
    0x1f, 0x00, 0x1f, 0xf8, 0xff, 0x00, 0x1f, 0x80, 0x1f, 0xf0, 0x7f, 0x00,
    0x1f, 0xc0, 0x1f, 0xe0, 0x3f, 0x00, 0x3f, 0xe0, 0x1f, 0xc0, 0x1f, 0x00,
    0x3f, 0xf0, 0x0f, 0x80, 0x0f, 0x00, 0x3e, 0xf0, 0x07, 0x00, 0x02, 0x00,
    0x7e, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x7e, 0xe0, 0x01, 0x00, 0x00, 0x00,
    0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x30, 0x00, 0x00,
    0xf8, 0x03, 0x00, 0x7c, 0x00, 0x00, 0xf8, 0x07, 0x00, 0xfe, 0x00, 0x00,
    0xf0, 0x1f, 0x80, 0x7f, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x7f, 0x00, 0x00,
    0xc0, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xff, 0xff, 0x1f, 0x00, 0x00,
    0x00, 0xfe, 0xff, 0x07, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x01, 0x00, 0x00,
    0x00, 0x80, 0x3f, 0x00, 0x00, 0x00};

class IconManager {
private:
    unsigned char const *bits;
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
    unsigned int color;
    unsigned int bgColor;
    bool requiresRedraw;
    bool visible;
    IconManager *subIcon;
    void eraseRaw();
    void drawRaw();

public:
    IconManager();
    ~IconManager();
    void init(TFT_eSPI tft, unsigned char const *_bits, const unsigned int x, const unsigned int y, const unsigned int _w, const unsigned int _h, const unsigned int _color, const unsigned int _bgColor);
    void draw();
    void erase();
    void drawWithColor(const unsigned int _color);
    void setColor(const unsigned int _color);
    void setOverlay(IconManager *_subIcon);
    void setVisible(bool isVisible);
    bool isVisible();
    void setOverlayColor(const unsigned int _color);
    void setOverlayVisible(bool isVisible);
    bool isOverlayVisible();
    bool isNotVisible();
    bool getRequiresRedraw();
    int getColor();
    int getBgColor();
    void invalidate();
};

#endif