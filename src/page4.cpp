#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page4.h"
#include "RadarData.h"
#include <math.h> // For sine wave calculations

void showPage4
(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    // Clear the background completely when switching to this page
    background.fillSprite(TFT_BLACK);
    int lineWidth = 3; // Adjust for the desired thickness

    // Create sprites for combined waveforms and rate numbers
    static TFT_eSprite waveformsSprite = TFT_eSprite(&tft);
    static TFT_eSprite heartRateSprite = TFT_eSprite(&tft);
    static TFT_eSprite breathRateSprite = TFT_eSprite(&tft);
    static bool isInitialized = false;

    if (!isInitialized) {
        // Create combined waveforms sprite
        waveformsSprite.createSprite(background.width() * 3 / 4, background.height());
        waveformsSprite.fillSprite(TFT_BLACK);

        // Create heart rate number sprite
        heartRateSprite.createSprite(70, 75); // Adjust size to fit the number
        heartRateSprite.fillSprite(TFT_BLACK);

        // Create breathing rate number sprite
        breathRateSprite.createSprite(70, 75); // Adjust size to fit the number
        breathRateSprite.fillSprite(TFT_BLACK);

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
    heartRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    heartRateSprite.setTextColor(TFT_RED, TFT_BLACK);
    heartRateSprite.fillSprite(TFT_BLACK);
    heartRateSprite.drawString(String((int)radarData.heartRateEst), heartRateSprite.width()*0.05, heartRateSprite.height()*0.2,7);
    heartRateSprite.pushToSprite(&background, background.width() - 70, 5, TFT_BLACK); // Top-right corner

    // Update breathing rate number sprite
    String breathRateStr = radarData.breathingRateEst < 10
                           ? "0" + String((int)radarData.breathingRateEst)
                           : String((int)radarData.breathingRateEst);

    breathRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    breathRateSprite.setTextColor(TFT_GREEN, TFT_BLACK);
    breathRateSprite.fillSprite(TFT_BLACK);
    breathRateSprite.drawString(breathRateStr, breathRateSprite.width()*0.05, breathRateSprite.height()*0.2,7);
    breathRateSprite.pushToSprite(&background, background.width() - 70, background.height() - 85, TFT_BLACK); // Bottom-right corner

    // Push the background sprite to the display
    background.pushSprite(0, 0);
}



/*
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
*/