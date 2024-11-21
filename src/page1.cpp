#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page1.h"
#include "RadarData.h"

void showPage1(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    static TFT_eSprite txtSprite = TFT_eSprite(&tft);
    char heartRateStr[10],lastHeartRateStr[10];
    char breathingRateStr[10],lastBreathingRateStr[10];
    char timeStr[10];
    
    // Define thresholds for "reasonable" values
    float heartRateMin = 30.0, heartRateMax = 180.0; // Acceptable heart rate range
    float breathingRateMin = 5.0, breathingRateMax = 40.0; // Acceptable breathing rate range

    // Format heart rate and breathing rate, check if values are reasonable
    if (radarData.heartRateEst < heartRateMin || radarData.heartRateEst > heartRateMax) {
        strcpy(heartRateStr, "");
    } else {
        snprintf(heartRateStr, sizeof(heartRateStr), "%.1f", radarData.heartRateEst);
        strcpy(lastHeartRateStr, heartRateStr);
    }
    if (radarData.breathingRateEst < breathingRateMin || radarData.breathingRateEst > breathingRateMax) {
        strcpy(breathingRateStr, lastBreathingRateStr);
    } else {
        snprintf(breathingRateStr, sizeof(breathingRateStr), "%.1f", radarData.breathingRateEst);
        strcpy(lastBreathingRateStr, breathingRateStr);
    }

    // Get current time (placeholder values for demonstration)
    int hour = 12, minute = 0, second = 0; // Replace with actual time logic if available
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hour, minute, second);

    // Sky blue background
    background.fillSprite(TFT_SKYBLUE);

    // Create text sprite for rendering
    txtSprite.createSprite(tft.width(), tft.height());
    txtSprite.setTextDatum(TC_DATUM); // Center text horizontally
    
    // Display heart rate (left side)
    txtSprite.fillRect(10, 50, 140, 100, TFT_DARKGREY); // Gray square background
    txtSprite.setTextColor(TFT_RED, TFT_DARKGREY); // Pink font with gray background
    txtSprite.drawString(heartRateStr, 80, 80, 7); // Position centered in the square
    //txtSprite.pushToSprite(&background, 10, 10, TFT_SKYBLUE);
    //background.pushSprite(0, 0);
    
    // Display breathing rate (right side)
    txtSprite.fillRect(tft.width() - 150, 50, 140, 100, TFT_DARKGREY); // Dark gray square background
    txtSprite.setTextColor(TFT_WHITE, TFT_DARKGREY); // Light green font with dark gray background
    txtSprite.drawString(breathingRateStr, tft.width() - 80, 80, 7); // Position centered in the square
    //txtSprite.pushToSprite(&background, 10, 10, TFT_SKYBLUE);
    
    // Display current time (top-right corner)
    txtSprite.setTextColor(TFT_WHITE, TFT_SKYBLUE); // White font on sky blue background
    //txtSprite.setTextSize(2); // Smaller font size for time
    txtSprite.drawString(timeStr, tft.width() - 60, 8, 2); // Top-right corner
    //txtSprite.pushToSprite(&background, 10, 10, TFT_SKYBLUE);
    
    // Push sprite to background and update screen
    txtSprite.pushToSprite(&background, 0, 0);
    background.pushSprite(0, 0);

    // Delete the sprite to free memory
    txtSprite.deleteSprite();
}


/*
void showPage1(TFT_eSPI &tft) {
    tft.fillScreen(TFT_RED);
    tft.fillCircle(tft.width() / 2, tft.height() / 2, 80, TFT_YELLOW);
    tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("1", tft.width() / 2, tft.height() / 2);
}

void showPage1(TFT_eSPI &tft, TFT_eSprite &background,RadarData &radarData) {
    background.fillSprite(TFT_RED);  // Set the background color to red
    background.fillCircle(tft.width() / 2, tft.height() / 2, 80, TFT_YELLOW);  // Yellow circle
    background.setTextColor(TFT_BLACK);  // Text color black
    background.setTextSize(12);
    background.setTextDatum(MC_DATUM);  // Center the text
    background.drawString("1", tft.width() / 2, tft.height() / 2);  // Draw "1" in the center

    // Push sprite to screen
    background.pushSprite(0, 0);
}
*/ 
/*  
void showPage1(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    static TFT_eSprite txtSprite = TFT_eSprite(&tft);

    background.fillSprite(TFT_BLUE);  // Set the background color to gray
    background.setTextColor(TFT_WHITE);    // Set text color to white
    background.setTextSize(2);             // Adjust text size for better readability
    background.setTextDatum(TL_DATUM);     // Set text alignment to top-left

    // Print the header
    //background.drawString("Page 1", 10, 10);  // Header in the first line
    txtSprite.createSprite(320,170);
    txtSprite.setTextColor(TFT_WHITE,TFT_BLACK);
    txtSprite.fillSprite(TFT_BLACK);
    txtSprite.drawString("Page 1",0,0,4);
    txtSprite.pushToSprite(&background,10,10,TFT_BLACK);

    // Display radar data information
    int lineHeight = 20;  // Define the line height for spacing
    int startY = 40;      // Start Y position for radar data display
    int x = 10;           // Left padding for text

    
    background.pushSprite(0, 0);
    
}*/
