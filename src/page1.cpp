#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page1.h"
#include "RadarData.h"

void showPage1(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    static TFT_eSprite txtSprite = TFT_eSprite(&tft);
    static TFT_eSprite ledBarSprite = TFT_eSprite(&tft);
    char heartRateStr[10], lastHeartRateStr[10];
    char breathingRateStr[10], lastBreathingRateStr[10];
    char timeStr[10];
    char rangeStr[10]; // For displaying the range value

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

    // Format range string
    float displayRange = radarData.range * 100; // Convert range to centimeters
    snprintf(rangeStr, sizeof(rangeStr), "%.2fcm", displayRange);


    // Sky blue background
    background.fillSprite(TFT_SKYBLUE);

    // Create text sprite for rendering
    txtSprite.createSprite(tft.width(), tft.height());
    txtSprite.setTextDatum(TC_DATUM); // Center text horizontally

    // Display heart rate (left side)
    txtSprite.fillRect(10, 50, 140, 100, TFT_DARKGREY); // Gray square background
    txtSprite.setTextColor(TFT_RED, TFT_DARKGREY); // Pink font with gray background
    txtSprite.drawString(heartRateStr, 80, 80, 7); // Position centered in the square

    // Display breathing rate (right side)
    txtSprite.fillRect(tft.width() - 150, 50, 140, 100, TFT_DARKGREY); // Dark gray square background
    txtSprite.setTextColor(TFT_WHITE, TFT_DARKGREY); // Light green font with dark gray background
    txtSprite.drawString(breathingRateStr, tft.width() - 80, 80, 7); // Position centered in the square

    // Display current time (top-right corner)
    txtSprite.setTextColor(TFT_WHITE, TFT_SKYBLUE); // White font on sky blue background
    txtSprite.drawString(timeStr, tft.width() - 60, 10, 2); // Top-right corner

    // Display object range (top-left corner)
    txtSprite.setTextColor(TFT_WHITE, TFT_DARKGREEN); // White font on sky blue background
    txtSprite.drawString(rangeStr, 50, 10, 4); // Top-right corner

    // Create LED bar sprite
    int barWidth = tft.width() * 2 / 4; // Bar width is 3/4 of screen width
    int barHeight = 10; // Height of the LED bar
    int barX = (tft.width() - barWidth) / 2; // Center the bar horizontally
    int barY = 0; // Position at the top of the screen

    ledBarSprite.createSprite(barWidth, barHeight);
    ledBarSprite.fillSprite(TFT_SKYBLUE); // Clear sprite with background color

    int totalLEDs = 20;
    int ledWidth = barWidth / totalLEDs; // Width of each LED segment

    int activeLEDs;
    if (radarData.range < 0.3) {
    activeLEDs = radarData.range / 0.3 * (totalLEDs * 0.2); // First 25% of LEDs
} else if (radarData.range < 0.5) {
    activeLEDs = (0.25 * totalLEDs) + (radarData.range - 0.3) / 0.2 * (totalLEDs * 0.25); // Next 25%
} else if (radarData.range < 0.7) {
    activeLEDs = (0.5 * totalLEDs) + (radarData.range - 0.5) / 0.2 * (totalLEDs * 0.25); // Next 25%
} else {
    activeLEDs = (0.75 * totalLEDs) + (radarData.range - 0.7) / 0.3 * (totalLEDs * 0.25); // Final 25%
}

// Ensure activeLEDs doesn't exceed totalLEDs
if (activeLEDs > totalLEDs) activeLEDs = totalLEDs;

// Loop through all LEDs to determine their colors and fill the sprite
for (int i = 0; i < totalLEDs; ++i) {
    int ledColor;

    // Assign colors based on the LED position
    if (i < totalLEDs * 0.25) {
        ledColor = TFT_LIGHTGREY; // Dark green
    } else if (i < totalLEDs * 0.5) {
        ledColor = TFT_GREEN; // Light green
    } else if (i < totalLEDs * 0.75) {
        ledColor = TFT_YELLOW; // Yellow
    } else {
        ledColor = TFT_RED; // Red
    }

    // Fill active LEDs with their colors, inactive LEDs with black
    if (i < activeLEDs) {
        ledBarSprite.fillRect(i * ledWidth, 0, ledWidth - 1, barHeight, ledColor); // Active LED
    } else {
        ledBarSprite.fillRect(i * ledWidth, 0, ledWidth - 1, barHeight, TFT_BLACK); // Inactive LED
    }
}



    // Push text sprite to background and update screen
    txtSprite.pushToSprite(&background, 0, 0);
    txtSprite.deleteSprite();

    // Push LED bar sprite to the background
    ledBarSprite.pushToSprite(&background, 0, 0);
    ledBarSprite.deleteSprite();

    // Push text sprite to background and update screen
    //txtSprite.pushToSprite(&background, 0, 0);
    background.pushSprite(0, 0);

    // Delete the text sprite to free memory
    txtSprite.deleteSprite();
}



/*
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
*/
