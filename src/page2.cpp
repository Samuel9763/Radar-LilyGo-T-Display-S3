#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page2.h"
#include "RadarData.h"

void showPage2(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    static TFT_eSprite txtSprite = TFT_eSprite(&tft);

    // Set the background to sky blue
    background.fillSprite(TFT_SKYBLUE);

    // Set text properties
    background.setTextDatum(TL_DATUM); // Align text to the top-left corner
    txtSprite.setTextColor(TFT_WHITE, TFT_BLACK);
    
    // Header: "Page 2"
    txtSprite.createSprite(320, 30);
    txtSprite.fillSprite(TFT_SKYBLUE);
    txtSprite.setTextColor(TFT_RED, TFT_SKYBLUE);
    txtSprite.drawString("Page 2", 0, 0, 4);
    txtSprite.pushToSprite(&background, 10, 10, TFT_SKYBLUE);
    txtSprite.deleteSprite();

    // Radar data details
    txtSprite.createSprite(320, 200);
    txtSprite.fillSprite(TFT_SKYBLUE);
    txtSprite.setTextColor(TFT_WHITE, TFT_SKYBLUE);
    int startY = 10;  // Start Y position for radar data display
    int lineHeight = 20; // Increased line height for larger font size

    // Print RadarData information
    txtSprite.setTextFont(2);
    txtSprite.drawString("Max RCS: " + String(radarData.maxRCS, 2), 0, startY);
    startY += lineHeight;
    txtSprite.drawString("Heart Rate: " + String(radarData.heartRateEst, 2), 0, startY);
    startY += lineHeight;
    txtSprite.drawString("Breathing Rate: " + String(radarData.breathingRateEst, 2), 0, startY);
    startY += lineHeight;
    txtSprite.drawString("Energy Breath Wfm: " + String(radarData.sumEnergyBreathWfm, 2), 0, startY);
    startY += lineHeight;
    txtSprite.drawString("Energy Heart Wfm: " + String(radarData.sumEnergyHeartWfm, 2), 0, startY);
    
    // Push the sprite and update the display
    txtSprite.pushToSprite(&background, 10, 40, TFT_SKYBLUE);
    background.pushSprite(0, 0);
    txtSprite.deleteSprite();
}


/*
void showPage2(TFT_eSPI &tft, TFT_eSprite &background,RadarData &radarData) {
    // Set the background color to blue
    background.fillSprite(TFT_BLUE);  

    // Set the text color to black and the font size
    background.setTextColor(TFT_BLACK);
    background.setTextSize(3);  // Set text size to 3 for a larger font

    // Draw a yellow square (70x70) at the center of the screen
    int squareSize = 70;
    int x = (tft.width() - squareSize) / 2;  // Calculate the x position to center
    int y = (tft.height() - squareSize) / 2;  // Calculate the y position to center
    background.fillRect(x, y, squareSize, squareSize, TFT_YELLOW);

    // Draw the text "2" in the center of the square
    background.setTextDatum(MC_DATUM);  // Center the text
    background.drawString("2", x + squareSize / 2, y + squareSize / 2);  // Draw text centered in the square

    // Push the sprite to the screen
    background.pushSprite(0, 0);
    
}
*/