#ifndef DRAWICONS_H
#define DRAWICONS_H

#include <TFT_eSPI.h>

struct FlashState {
    unsigned long lastFlashTime = 0;
    bool isHeartVisible = true;
};

void drawHeartIcon(TFT_eSprite &sprite, int centerX, int centerY, int radius, uint16_t baseColor, int flashRate, int rotation, FlashState &flashState);
void drawSingleFlashingHeart(TFT_eSprite &sprite, int heartRate);
void drawFourLeafClover(TFT_eSprite &cloverSprite, int breathingRate);

#endif
