#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page5.h"
#include "RadarData.h"
#include <math.h> // For sine wave calculations

struct FlashState {
    unsigned long lastFlashTime = 0; // Tracks last flash toggle time
    bool isHeartVisible = true;      // Current visibility of the heart
};

void drawHeartIcon(TFT_eSprite &sprite, int centerX, int centerY, int radius, uint16_t baseColor, int flashRate, int rotation, FlashState &flashState) {
    // Calculate flash interval based on flashRate
    unsigned long flashInterval = (flashRate > 0) ? (30000 / flashRate) : 1000; // Full period for one cycle (dark to light to dark)

    // Calculate the elapsed time within the current cycle
    unsigned long currentTime = millis();
    float cycleProgress = (currentTime % flashInterval) / (float)flashInterval; // Progress as a fraction (0 to 1)

    // Calculate sine wave intensity (0.3 to 1.0 range for smooth fade)
    float intensity = 0.3f + 0.7f * (sin(cycleProgress * 2 * PI) + 1.0f) / 2.0f;

    // Extract base color components (assuming RGB565 format)
    uint8_t r = (baseColor >> 11) & 0x1F; // Red component (5 bits)
    uint8_t g = (baseColor >> 5) & 0x3F;  // Green component (6 bits)
    uint8_t b = baseColor & 0x1F;         // Blue component (5 bits)

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

void drawSingleFlashingHeart(TFT_eSprite &sprite,int heartRate) {
    static FlashState singleHeartState; // Independent state for single heart
    // Constants for heart dimensions
    int centerX = sprite.width() / 2;
    int centerY = sprite.height() / 2;
    int radius = sprite.width() / 8; // Adjust for heart size
    // Draw a single heart icon flashing at 60 BPM
    drawHeartIcon(sprite, centerX, centerY, radius, TFT_RED, heartRate/3,0, singleHeartState); // Center (40,40), radius 20, red color, 60 BPM
    //drawHeartIcon(sprite, centerX, centerY, radius, TFT_RED, heartRate,0, singleHeartState); // Center (40,40), radius 20, red color, 60 BPM
}

void drawFourLeafClover(TFT_eSprite &cloverSprite,int breathingRate) {
    // Independent states for each heart in the clover
    static FlashState topHeartState;
    static FlashState rightHeartState;
    static FlashState bottomHeartState;
    static FlashState leftHeartState;

    // Clover center and radius
    int centerX = cloverSprite.width() / 2;
    int centerY = cloverSprite.height() / 2;
    int radius = cloverSprite.width() / 15; // Adjust for heart size
    // Draw four heart icons as a clover
    drawHeartIcon(cloverSprite, centerX, centerY - radius * 3, radius, TFT_DARKGREEN, breathingRate,0,topHeartState); // Top heart
    drawHeartIcon(cloverSprite, centerX + radius * 3, centerY, radius, TFT_DARKGREEN, breathingRate,90,rightHeartState); // Right heart
    drawHeartIcon(cloverSprite, centerX, centerY + radius * 3, radius, TFT_DARKGREEN, breathingRate,180,bottomHeartState); // Bottom heart
    drawHeartIcon(cloverSprite, centerX - radius * 3, centerY, radius, TFT_GREEN, breathingRate,270,leftHeartState); // Left heart
}


void showPage5
(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    // Clear the background completely when switching to this page
    background.fillSprite(TFT_BLACK);
    int lineWidth = 3; // Adjust for the desired thickness

    // Create sprites for combined waveforms and rate numbers
    static TFT_eSprite waveformsSprite = TFT_eSprite(&tft);
    static TFT_eSprite heartRateSprite = TFT_eSprite(&tft);
    static TFT_eSprite heartIconSprite = TFT_eSprite(&tft);
    static TFT_eSprite breathRateSprite = TFT_eSprite(&tft);
    static TFT_eSprite breathIconSprite = TFT_eSprite(&tft);
    static TFT_eSprite ledBarSprite = TFT_eSprite(&tft);
    static bool isInitialized = false;

    if (!isInitialized) {
        // Create combined waveforms sprite
        waveformsSprite.createSprite(background.width() * 3 / 5, background.height() * 8.5 / 10);
        waveformsSprite.fillSprite(TFT_BLACK);

        // Create range LED bar sprite
        ledBarSprite.createSprite(waveformsSprite.width()-10, 10);
        ledBarSprite.fillSprite(TFT_SKYBLUE);

        // Create heart rate number sprite
        heartRateSprite.createSprite(100, 75); // Adjust size to fit the number
        heartRateSprite.fillSprite(TFT_BLACK);

        heartIconSprite.createSprite(35, 38); // Adjust size as needed
        heartIconSprite.fillSprite(TFT_BLACK);

        // Create breathing rate number sprite
        breathRateSprite.createSprite(100, 75); // Adjust size to fit the number
        breathRateSprite.fillSprite(TFT_BLACK);

        breathIconSprite.createSprite(35, 38); // Adjust size as needed
        breathIconSprite.fillSprite(TFT_BLACK);

        isInitialized = true;
    }

    // Constants for layout
    const int waveformWidth = waveformsSprite.width();            // Width of the combined waveform
    const int waveformHeight = waveformsSprite.height();          // Height of the combined waveform
    const int waveBaseY = waveformHeight / 2;                     // Center line for the waveforms
    const float amplitude = 50.0;                                 // Amplitude for both waveforms
    const float timeStep = 1.0 / 60.0;                            // Time increment per frame (assume ~60 FPS)

    // Static variables to track waveform updates
    static float heartPhase = 0.0;        // Phase for heart waveform
    static float breathPhase = 0.0;      // Phase for breathing waveform
    static int x = 0;                    // Shared X-coordinate for both waveforms
    static float lastHeartY = waveBaseY; // Last Y-coordinate for heart waveform
    static float lastBreathY = waveBaseY; // Last Y-coordinate for breathing waveform

    // Update phases based on radarData
    float heartFrequency = radarData.heartRateEst;
    float breathFrequency = radarData.breathingRateEst;

    heartPhase += (2 * PI * heartFrequency * timeStep) / 60; // Slower visualization
    if (heartPhase > 2 * PI) heartPhase -= 2 * PI;

    breathPhase += (2 * PI * breathFrequency * timeStep) / 60; // Slower visualization
    if (breathPhase > 2 * PI) breathPhase -= 2 * PI;

    // Calculate current Y-coordinates
    float heartY = waveBaseY + amplitude * sin(heartPhase);
    //if(heartFrequency==0) {heartY=waveBaseY;}
    float breathY = waveBaseY + amplitude * sin(breathPhase);
    //if(breathFrequency==0) {breathY=waveBaseY+lineWidth;}

    // Update waveforms on the shared sprite
    //waveformsSprite.drawLine(x, 0, x + 1, waveformHeight, TFT_BLACK); // Overwrite old heart line
    //waveformsSprite.drawLine(x, lastHeartY, x + 1, heartY, TFT_RED);       // Draw new heart line
    if(x==0) waveformsSprite.fillRect(x, 0, lineWidth, waveformHeight, TFT_BLACK);// Overwrite old heart line
    waveformsSprite.fillRect(x+lineWidth, 0, 1, waveformHeight, TFT_BLACK);// Overwrite old heart line
    waveformsSprite.fillRect(x, lastHeartY - lineWidth / 2, 2, lineWidth, TFT_RED); // Draw a wider heart line
    //waveformsSprite.fillRect(x+1, lastHeartY - lineWidth / 2, 2, waveformHeight, TFT_BLACK);// Overwrite old heart line

    //waveformsSprite.drawLine(x, lastBreathY, x + 1, lastBreathY, TFT_BLACK); // Overwrite old breath line
    //waveformsSprite.drawLine(x, lastBreathY, x + 1, breathY, TFT_GREEN);     // Draw new breath line
    //waveformsSprite.fillRect(x+lineWidth, lastBreathY - lineWidth / 2, 2, waveformHeight, TFT_BLACK);// Overwrite old heart line
    waveformsSprite.fillRect(x, lastBreathY - lineWidth / 2, 2, lineWidth, TFT_GREEN); // Draw a wider heart line
    //waveformsSprite.fillRect(x+1, lastBreathY - lineWidth / 2, 2, waveformHeight, TFT_BLACK);// Overwrite old heart line

    lastHeartY = heartY;
    lastBreathY = breathY;
    x++;
    if (x >= waveformWidth) x = 0;

    // Draw boundary box
    waveformsSprite.drawRect(0, 0, waveformWidth, waveformHeight, TFT_WHITE);

    // Push waveforms sprite to the background
    waveformsSprite.pushToSprite(&background, 0, 25, TFT_BLACK); // Align to the top-left corner

    // Update heart rate number sprite
    String heartRateStr;
    if (radarData.heartRateEst < 10) {
        heartRateStr = "00" + String((int)radarData.heartRateEst);
    } else if (radarData.heartRateEst < 100) {
        heartRateStr = "0" + String((int)radarData.heartRateEst);
    } else {
        heartRateStr = String((int)radarData.heartRateEst);
    }

    // Update heart rate number sprite
    //heartRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    heartRateSprite.setTextColor(TFT_RED, TFT_BLACK);
    heartRateSprite.fillSprite(TFT_BLACK);
    //heartRateSprite.drawString(String((int)radarData.heartRateEst), heartRateSprite.width()*0.05, heartRateSprite.height()*0.2,7);
    heartRateSprite.drawString(heartRateStr, heartRateSprite.width()*0.05, heartRateSprite.height()*0.2,7);
    heartRateSprite.pushToSprite(&background, background.width() - 100, 5, TFT_BLACK); // Top-right corner

    heartIconSprite.fillSprite(TFT_BLACK);
    //heartIconSprite.pushToSprite(&background, background.width() - 150, 5, TFT_BLACK); // Top-right corner

    // Draw the heart icon based on heart rate
    //drawHeartIcon(heartIconSprite, (int)radarData.heartRateEst);
    drawSingleFlashingHeart(heartIconSprite, (int)radarData.heartRateEst);
    heartIconSprite.pushToSprite(&background, background.width() - 128, 5, TFT_BLACK); // Top-right corner


    // Update breathing rate number sprite
    String breathRateStr;
    if (radarData.breathingRateEst < 10) {
        breathRateStr = "00" + String((int)radarData.breathingRateEst);
    } else if (radarData.breathingRateEst < 100) {
        breathRateStr = "0" + String((int)radarData.breathingRateEst);
    } else {
        breathRateStr = String((int)radarData.breathingRateEst);
    }
    /*
    String breathRateStr = radarData.breathingRateEst < 10
                           ? "0" + String((int)radarData.breathingRateEst)
                           : String((int)radarData.breathingRateEst);*/

    //breathRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    breathRateSprite.setTextColor(TFT_GREEN, TFT_BLACK);
    breathRateSprite.fillSprite(TFT_BLACK);
    breathRateSprite.drawString(breathRateStr, breathRateSprite.width()*0.05, breathRateSprite.height()*0.2,7);
    breathRateSprite.pushToSprite(&background, background.width() - 100, background.height() - 85, TFT_BLACK); // Bottom-right corner

    breathIconSprite.fillSprite(TFT_BLACK);
    //drawLeafIcon(breathIconSprite, (int)radarData.breathingRateEst);
    drawFourLeafClover(breathIconSprite, (int)radarData.breathingRateEst);
    breathIconSprite.pushToSprite(&background, background.width() - 128, background.height() - 85, TFT_BLACK); // Top-right corner


    char rangeStr[10]; // For displaying the range value
    // Format range string
    float displayRange = radarData.range * 100; // Convert range to centimeters
    snprintf(rangeStr, sizeof(rangeStr), "%.2fcm", displayRange);

    
    // Display object range (top-left corner)
    //txtSprite.setTextColor(TFT_WHITE, TFT_DARKGREEN); // White font on sky blue background
    //txtSprite.drawString(rangeStr, 50, 10, 4); // Top-right corner

    // Create LED bar sprite
    int barWidth = ledBarSprite.width(); // Bar width is 3/4 of screen width
    int barHeight = 10; // Height of the LED bar
    int barX = (tft.width() - barWidth) / 2; // Center the bar horizontally
    int barY = 0; // Position at the top of the screen

    int totalLEDs = 20;
    int ledWidth = barWidth / totalLEDs; // Width of each LED segment

    int activeLEDs;
    // Calculate the number of active LEDs based on radarData.range
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
    // Push LED bar sprite to the background
    ledBarSprite.pushToSprite(&background, 0, 0, TFT_BLACK);
    //ledBarSprite.deleteSprite();

    // Push the background sprite to the display
    background.pushSprite(0, 0);

}