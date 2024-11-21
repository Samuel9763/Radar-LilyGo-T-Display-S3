#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page4.h"
#include "RadarData.h"


/*
void showPage4(TFT_eSPI &tft, TFT_eSprite &background) {
    static TFT_eSprite needle = TFT_eSprite(&tft);
    //static TFT_eSprite background = TFT_eSprite(&tft);  // Sprite object
    static int angle = 0;

    tft.fillScreen(TFT_BLACK);
    tft.setPivot(85, 85);
    needle.createSprite(20, 80);
    //background.createSprite(170, 170);

    background.fillSprite(TFT_BLACK);
    background.fillCircle(85, 85, 85, TFT_SILVER);
    needle.fillSprite(TFT_BLACK);
    needle.drawWedgeLine(10, 0, 10, 40, 1, 10, TFT_RED);
    needle.drawWedgeLine(10, 40, 10, 80, 10, 1, TFT_BLUE);
    needle.fillCircle(10, 40, 10, TFT_WHITE);
    needle.pushRotated(&background,angle,TFT_BLACK);
    background.pushSprite(0, 0);
    angle++;
    //delay(10);
    if (angle > 360) angle = 0;
}
*/


void showPage4(TFT_eSPI &tft, TFT_eSprite &background,RadarData &radarData) {
    static int angle = 0;  // Track the rotation angle

    // Clear the background and set up a silver circle
    background.fillSprite(TFT_BLACK);  // Clear the screen
    int centerX = tft.width() / 2;
    int centerY = tft.height() / 2;
    background.fillCircle(centerX, centerY, 80, TFT_SILVER);  // Draw silver circle

    // Create a needle sprite and fill it with red color
    TFT_eSprite needle = TFT_eSprite(&tft);
    needle.createSprite(20, 80);  // Needle size: 20x80
    needle.fillSprite(TFT_BLACK);  // Transparent background
    needle.drawWedgeLine(10, 0, 10, 40, 1, 10, TFT_RED);
    needle.drawWedgeLine(10, 40, 10, 80, 10, 1, TFT_BLUE);
    needle.fillCircle(10, 40, 10, TFT_WHITE);
    //needle.fillRect(0, 0, 20, 80, TFT_RED); // Red needle

    // Rotate the needle around the center
    needle.pushRotated(&background,angle,TFT_BLACK);
    needle.deleteSprite();  // Delete needle sprite after pushing
    background.pushSprite(0, 0);
    // Increment angle for next frame
    angle += 5;  // Adjust rotation speed
    if (angle >= 360) angle = 0;  // Reset angle after a full rotation

    // Update screen with the modified background sprite
    //background.pushSprite(0, 0);
}