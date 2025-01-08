#include "DrawIcons.h"
#include <Arduino.h>
#include <math.h>

void drawHeartIcon(TFT_eSprite &sprite, int centerX, int centerY, int radius, uint16_t baseColor, int flashRate, int rotation, FlashState &flashState) {
    // Calculate flash interval based on flashRate
    unsigned long flashInterval = (flashRate > 0) ? (30000 / flashRate) : 1000;

    // Calculate the elapsed time within the current cycle
    unsigned long currentTime = millis();
    float cycleProgress = (currentTime % flashInterval) / (float)flashInterval;

    // Calculate sine wave intensity (0.3 to 1.0 range for smooth fade)
    float intensity = 0.3f + 0.7f * (sin(cycleProgress * 2 * PI) + 1.0f) / 2.0f;

    // Extract base color components (assuming RGB565 format)
    uint8_t r = (baseColor >> 11) & 0x1F;
    uint8_t g = (baseColor >> 5) & 0x3F;
    uint8_t b = baseColor & 0x1F;

    // Apply intensity to each color component
    r = (uint8_t)(r * intensity);
    g = (uint8_t)(g * intensity);
    b = (uint8_t)(b * intensity);

    // Reconstruct the modulated color
    uint16_t modulatedColor = sprite.color565(r << 3, g << 2, b << 3);

    // Convert rotation angle from degrees to radians
    float angleRad = rotation * (PI / 180.0);

    // Helper function for rotation
    auto rotatePoint = [&](int x, int y) {
        int newX = centerX + cos(angleRad) * (x - centerX) - sin(angleRad) * (y - centerY);
        int newY = centerY + sin(angleRad) * (x - centerX) + cos(angleRad) * (y - centerY);
        return std::make_pair(newX, newY);
    };

    // Rotate and draw the two circles for the top part of the heart
    auto [leftX, leftY] = rotatePoint(centerX - radius, centerY - radius / 2);
    auto [rightX, rightY] = rotatePoint(centerX + radius, centerY - radius / 2);
    sprite.fillCircle(leftX, leftY, radius, modulatedColor);
    sprite.fillCircle(rightX, rightY, radius, modulatedColor);

    // Rotate and draw the triangle for the bottom part of the heart
    auto [bottomLeftX, bottomLeftY] = rotatePoint(centerX - radius * 2, centerY);
    auto [bottomRightX, bottomRightY] = rotatePoint(centerX + radius * 2, centerY);
    auto [bottomTipX, bottomTipY] = rotatePoint(centerX, centerY + radius * 2);
    sprite.fillTriangle(
        bottomLeftX, bottomLeftY,
        bottomRightX, bottomRightY,
        bottomTipX, bottomTipY,
        modulatedColor
    );
}

void drawSingleFlashingHeart(TFT_eSprite &sprite, int heartRate) {
    static FlashState singleHeartState;
    int centerX = sprite.width() / 2;
    int centerY = sprite.height() / 2;
    int radius = sprite.width() / 8;
    drawHeartIcon(sprite, centerX, centerY, radius, TFT_RED, heartRate/3, 0, singleHeartState);
}

void drawFourLeafClover(TFT_eSprite &cloverSprite, int breathingRate) {
    static FlashState topHeartState;
    static FlashState rightHeartState;
    static FlashState bottomHeartState;
    static FlashState leftHeartState;

    int centerX = cloverSprite.width() / 2;
    int centerY = cloverSprite.height() / 2;
    int radius = cloverSprite.width() / 15;
    drawHeartIcon(cloverSprite, centerX, centerY - radius * 3, radius, TFT_DARKGREEN, breathingRate, 0, topHeartState);
    drawHeartIcon(cloverSprite, centerX + radius * 3, centerY, radius, TFT_DARKGREEN, breathingRate, 90, rightHeartState);
    drawHeartIcon(cloverSprite, centerX, centerY + radius * 3, radius, TFT_DARKGREEN, breathingRate, 180, bottomHeartState);
    drawHeartIcon(cloverSprite, centerX - radius * 3, centerY, radius, TFT_GREEN, breathingRate, 270, leftHeartState);
}
