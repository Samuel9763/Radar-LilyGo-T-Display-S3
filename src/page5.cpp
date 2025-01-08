#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page5.h"
#include "RadarData.h"
#include "DrawIcons.h"
#include <math.h> // For sine wave calculations

void showPage5(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
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
    float breathY = waveBaseY + amplitude * sin(breathPhase);

    // Update waveforms on the shared sprite
    if(x==0) waveformsSprite.fillRect(x, 0, lineWidth, waveformHeight, TFT_BLACK);
    waveformsSprite.fillRect(x+lineWidth, 0, 1, waveformHeight, TFT_BLACK);
    waveformsSprite.fillRect(x, lastHeartY - lineWidth / 2, 2, lineWidth, TFT_RED);
    waveformsSprite.fillRect(x, lastBreathY - lineWidth / 2, 2, lineWidth, TFT_GREEN);

    lastHeartY = heartY;
    lastBreathY = breathY;
    x++;
    if (x >= waveformWidth) x = 0;

    // Draw boundary box
    waveformsSprite.drawRect(0, 0, waveformWidth, waveformHeight, TFT_WHITE);

    // Push waveforms sprite to the background
    waveformsSprite.pushToSprite(&background, 0, 25, TFT_BLACK);

    // Update heart rate number sprite
    String heartRateStr;
    if (radarData.heartRateEst < 10) {
        heartRateStr = "00" + String((int)radarData.heartRateEst);
    } else if (radarData.heartRateEst < 100) {
        heartRateStr = "0" + String((int)radarData.heartRateEst);
    } else {
        heartRateStr = String((int)radarData.heartRateEst);
    }

    heartRateSprite.setTextColor(TFT_RED, TFT_BLACK);
    heartRateSprite.fillSprite(TFT_BLACK);
    heartRateSprite.drawString(heartRateStr, heartRateSprite.width()*0.05, heartRateSprite.height()*0.2,7);
    heartRateSprite.pushToSprite(&background, background.width() - 100, 5, TFT_BLACK);

    heartIconSprite.fillSprite(TFT_BLACK);
    drawSingleFlashingHeart(heartIconSprite, (int)radarData.heartRateEst);
    heartIconSprite.pushToSprite(&background, background.width() - 128, 5, TFT_BLACK);

    // Update breathing rate number sprite
    String breathRateStr;
    if (radarData.breathingRateEst < 10) {
        breathRateStr = "00" + String((int)radarData.breathingRateEst);
    } else if (radarData.breathingRateEst < 100) {
        breathRateStr = "0" + String((int)radarData.breathingRateEst);
    } else {
        breathRateStr = String((int)radarData.breathingRateEst);
    }

    breathRateSprite.setTextColor(TFT_GREEN, TFT_BLACK);
    breathRateSprite.fillSprite(TFT_BLACK);
    breathRateSprite.drawString(breathRateStr, breathRateSprite.width()*0.05, breathRateSprite.height()*0.2,7);
    breathRateSprite.pushToSprite(&background, background.width() - 100, background.height() - 85, TFT_BLACK);

    breathIconSprite.fillSprite(TFT_BLACK);
    drawFourLeafClover(breathIconSprite, (int)radarData.breathingRateEst);
    breathIconSprite.pushToSprite(&background, background.width() - 128, background.height() - 85, TFT_BLACK);

    // Create LED bar sprite
    int barWidth = ledBarSprite.width();
    int barHeight = 10;
    int totalLEDs = 20;
    int ledWidth = barWidth / totalLEDs;

    int activeLEDs;
    if (radarData.range < 0.3) {
        activeLEDs = radarData.range / 0.3 * (totalLEDs * 0.2);
    } else if (radarData.range < 0.5) {
        activeLEDs = (0.25 * totalLEDs) + (radarData.range - 0.3) / 0.2 * (totalLEDs * 0.25);
    } else if (radarData.range < 0.7) {
        activeLEDs = (0.5 * totalLEDs) + (radarData.range - 0.5) / 0.2 * (totalLEDs * 0.25);
    } else {
        activeLEDs = (0.75 * totalLEDs) + (radarData.range - 0.7) / 0.3 * (totalLEDs * 0.25);
    }

    if (activeLEDs > totalLEDs) activeLEDs = totalLEDs;

    for (int i = 0; i < totalLEDs; ++i) {
        int ledColor;
        if (i < totalLEDs * 0.25) {
            ledColor = TFT_LIGHTGREY;
        } else if (i < totalLEDs * 0.5) {
            ledColor = TFT_GREEN;
        } else if (i < totalLEDs * 0.75) {
            ledColor = TFT_YELLOW;
        } else {
            ledColor = TFT_RED;
        }

        if (i < activeLEDs) {
            ledBarSprite.fillRect(i * ledWidth, 0, ledWidth - 1, barHeight, ledColor);
        } else {
            ledBarSprite.fillRect(i * ledWidth, 0, ledWidth - 1, barHeight, TFT_BLACK);
        }
    }
    
    ledBarSprite.pushToSprite(&background, 0, 0, TFT_BLACK);
    background.pushSprite(0, 0);
}
