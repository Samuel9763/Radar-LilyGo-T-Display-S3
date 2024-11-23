#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page4.h"
#include "RadarData.h"
#include <math.h> // For sine wave calculations


void drawLeafIcon(TFT_eSprite &sprite, int breathingRate) {
    // Constants for leaf dimensions
    int centerX = sprite.width() / 2;
    int centerY = sprite.height() / 2;
    int leafWidth = sprite.width() / 4;
    int leafHeight = sprite.height() / 4;

    // Pulse logic
    static unsigned long lastUpdateTime = 0;
    static float pulsePhase = 0.0f;  // Phase of the pulse (0.0 to 2π)

    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;  // Convert to seconds
    lastUpdateTime = currentTime;

    // Prevent divide-by-zero for breathing rate and ensure a reasonable pulse interval
    float pulseInterval = (breathingRate > 0) ? max(60.0f / breathingRate, 1.0f) : 2.0f;  // Seconds per pulse

    // Update pulse phase
    pulsePhase += (deltaTime / pulseInterval) * 2 * PI;
    if (pulsePhase > 2 * PI) pulsePhase -= 2 * PI;

    // Calculate scaling factor based on sine wave (0.9 to 1.1 range)
    float scale = 0.9f + 0.2f * (sin(pulsePhase) + 1.0f) / 2.0f;

    // Create pulsing green color
    uint8_t red = uint8_t(30 * scale);  // Slight red component for richness
    uint8_t green = uint8_t(200 * scale);
    uint8_t blue = uint8_t(30 * scale);
    uint16_t leafColor = sprite.color565(red, green, blue);

    // Clear the sprite for redrawing
    sprite.fillSprite(TFT_BLACK);

    // Draw the leaf (scaled ellipse)
    sprite.fillEllipse(centerX, centerY, leafWidth * scale, leafHeight * scale, leafColor);

    // Draw the central vein of the leaf
    int veinHeight = leafHeight * scale;
    sprite.drawLine(centerX, centerY - veinHeight, centerX, centerY + veinHeight, TFT_WHITE);

    // Push updated sprite to the display
    //sprite.pushSprite(10, 10);  // Adjust position as needed
}


void drawHeartIcon(TFT_eSprite &sprite, int heartRate) {
    // Clear the sprite for redrawing
    //sprite.fillSprite(TFT_BLACK);

    // Constants for heart dimensions
    int centerX = sprite.width() / 2;
    int centerY = sprite.height() / 2;
    int radius = sprite.width() / 6; // Adjust for heart size

    // Colors and flashing logic
    uint16_t heartColor = TFT_RED;
    static unsigned long lastFlashTime = 0;
    static bool isHeartVisible = true;
    
    //heartRate=200;
    // Calculate flash interval based on heart rate, with bounds checking
    unsigned long flashInterval = (heartRate > 0) ? (30000 / heartRate) : 1000; // Half the period for both on and off states
    
    // Control flashing with smoother transitions
    if (millis() - lastFlashTime > flashInterval) {
        isHeartVisible = !isHeartVisible;
        lastFlashTime = millis();
    }
    // Draw the heart icon if visible
    if (isHeartVisible) {
        // Two circles for the top part of the heart
        sprite.fillCircle(centerX - radius, centerY - radius / 2, radius, heartColor);
        sprite.fillCircle(centerX + radius, centerY - radius / 2, radius, heartColor);

        // Triangle for the bottom part of the heart
        sprite.fillTriangle(centerX - radius * 2, centerY,
                            centerX + radius * 2, centerY,
                            centerX, centerY + radius * 2, heartColor);
    }
    
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
    static bool isInitialized = false;

    if (!isInitialized) {
        // Create combined waveforms sprite
        waveformsSprite.createSprite(background.width() * 1 / 2, background.height());
        waveformsSprite.fillSprite(TFT_BLACK);

        // Create heart rate number sprite
        heartRateSprite.createSprite(70, 75); // Adjust size to fit the number
        heartRateSprite.fillSprite(TFT_BLACK);

        heartIconSprite.createSprite(70, 75); // Adjust size as needed
        heartIconSprite.fillSprite(TFT_BLACK);

        // Create breathing rate number sprite
        breathRateSprite.createSprite(70, 75); // Adjust size to fit the number
        breathRateSprite.fillSprite(TFT_BLACK);

        breathIconSprite.createSprite(70, 75); // Adjust size as needed
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
    waveformsSprite.pushToSprite(&background, 0, 0, TFT_BLACK); // Align to the top-left corner

    // Update heart rate number sprite
    String heartRateStr = radarData.heartRateEst < 10
                          ? "0" + String((int)radarData.heartRateEst)
                          : String((int)radarData.heartRateEst);

    // Update heart rate number sprite
    //heartRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    heartRateSprite.setTextColor(TFT_RED, TFT_YELLOW);
    heartRateSprite.fillSprite(TFT_YELLOW);
    heartRateSprite.drawString(String((int)radarData.heartRateEst), heartRateSprite.width()*0.05, heartRateSprite.height()*0.2,7);
    heartRateSprite.pushToSprite(&background, background.width() - 70, 5, TFT_BLACK); // Top-right corner

    heartIconSprite.fillSprite(TFT_WHITE);
    //heartIconSprite.pushToSprite(&background, background.width() - 150, 5, TFT_BLACK); // Top-right corner

    // Draw the heart icon based on heart rate
    drawHeartIcon(heartIconSprite, (int)radarData.heartRateEst);
    heartIconSprite.pushToSprite(&background, background.width() - 150, 5, TFT_BLACK); // Top-right corner


    // Update breathing rate number sprite
    String breathRateStr = radarData.breathingRateEst < 10
                           ? "0" + String((int)radarData.breathingRateEst)
                           : String((int)radarData.breathingRateEst);

    //breathRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    breathRateSprite.setTextColor(TFT_GREEN, TFT_RED);
    breathRateSprite.fillSprite(TFT_RED);
    breathRateSprite.drawString(breathRateStr, breathRateSprite.width()*0.05, breathRateSprite.height()*0.2,7);
    breathRateSprite.pushToSprite(&background, background.width() - 70, background.height() - 85, TFT_BLACK); // Bottom-right corner

    breathIconSprite.fillSprite(TFT_WHITE);
    drawLeafIcon(breathIconSprite, (int)radarData.breathingRateEst);
    breathIconSprite.pushToSprite(&background, background.width() - 150, background.height() - 85, TFT_BLACK); // Top-right corner


    // Push the background sprite to the display
    background.pushSprite(0, 0);
}