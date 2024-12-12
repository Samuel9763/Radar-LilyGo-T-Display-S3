#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page3.h"
#include "RadarData.h"

#include <math.h> // For sine calculations
/*      
void showPage3(TFT_eSPI &tft, TFT_eSprite &background,RadarData &radarData) {
    static TFT_eSprite txtSprite = TFT_eSprite(&tft);
    static int x = 0;

    background.fillSprite(TFT_RED);
    txtSprite.createSprite(160,80);
    txtSprite.setTextColor(TFT_WHITE,TFT_BLACK);
    txtSprite.fillSprite(TFT_BLACK);
    txtSprite.drawString(String(x),0,0,8);
    txtSprite.pushToSprite(&background,100,50,TFT_BLACK);
    background.pushSprite(0,0);
    x++;
    if (x > 99) x = 0;
    delay(100);

}
*/


void showPage3(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    // Clear the background completely when switching to this page
    background.fillSprite(TFT_BLACK);

    // Create sprites for waveforms
    static TFT_eSprite heartWaveform = TFT_eSprite(&tft);
    static TFT_eSprite breathWaveform = TFT_eSprite(&tft);
    static TFT_eSprite heartRateSprite = TFT_eSprite(&tft);
    static TFT_eSprite breathRateSprite = TFT_eSprite(&tft);
    static bool isInitialized = false;

    if (!isInitialized) {
        // Create heart waveform sprite
        heartWaveform.createSprite(background.width() * 7 / 10, background.height() / 2);
        heartWaveform.fillSprite(TFT_BLACK);

        // Create breathing waveform sprite
        breathWaveform.createSprite(background.width() * 7 / 10, background.height() / 2);
        breathWaveform.fillSprite(TFT_BLACK);

        // Create heart rate number sprite
        heartRateSprite.createSprite(90, 75); // Adjust size to fit the number
        heartRateSprite.fillSprite(TFT_YELLOW);

        // Create breathing rate number sprite
        breathRateSprite.createSprite(90, 75); // Adjust size to fit the number
        breathRateSprite.fillSprite(TFT_BLACK);

        isInitialized = true;
    }

    // Constants for layout
    const int waveformWidth = heartWaveform.width();            // Width of both waveforms
    const int waveformHeight = heartWaveform.height();          // Height of both waveforms
    const int heartWaveYBase = waveformHeight / 2;              // Center for the heart waveform
    const int breathWaveYBase = waveformHeight / 2;             // Center for the breathing waveform
    const float amplitude = 20.0;                               // Amplitude for both waveforms
    const float timeStep = 1.0 / 60.0;                          // Time increment per frame (assume ~60 FPS)

    // Static variables to track waveform updates
    static float heartPhase = 0.0;        // Phase for heart waveform
    static float breathPhase = 0.0;      // Phase for breathing waveform
    static int heartX = 0;               // X-coordinate for heart waveform
    static int breathX = 0;              // X-coordinate for breathing waveform
    static float lastHeartY = heartWaveYBase; // Last Y-coordinate for heart waveform
    static float lastBreathY = breathWaveYBase; // Last Y-coordinate for breathing waveform

    // Update phases based on radarData
    float heartFrequency = radarData.heartRateEst;
    float breathFrequency = radarData.breathingRateEst;

    heartPhase += (2 * PI * heartFrequency * timeStep)/30;
    if (heartPhase > 2 * PI) heartPhase -= 2 * PI;

    breathPhase += (2 * PI * breathFrequency * timeStep)/30;
    if (breathPhase > 2 * PI) breathPhase -= 2 * PI;

    // Calculate current Y-coordinates
    float heartY = heartWaveYBase + amplitude * sin(heartPhase);
    float breathY = breathWaveYBase + amplitude * sin(breathPhase);

    // Update heart waveform
    heartWaveform.drawLine(heartX, 0, heartX + 1, waveformHeight, TFT_BLACK); // Overwrite old
    heartWaveform.drawLine(heartX, lastHeartY, heartX + 1, heartY, TFT_RED);       // Draw new
    lastHeartY = heartY;
    heartX++;
    if (heartX >= waveformWidth) heartX = 0;

    // Update breathing waveform
    breathWaveform.drawLine(breathX, 0, breathX + 1, waveformHeight, TFT_BLACK); // Overwrite old
    breathWaveform.drawLine(breathX, lastBreathY, breathX + 1, breathY, TFT_GREEN);     // Draw new
    lastBreathY = breathY;
    breathX++;
    if (breathX >= waveformWidth) breathX = 0;

    // Draw boundary boxes
    heartWaveform.drawRect(0, 0, waveformWidth, waveformHeight, TFT_WHITE);
    breathWaveform.drawRect(0, 0, waveformWidth, waveformHeight, TFT_WHITE);

    // Push waveforms to the background
    heartWaveform.pushToSprite(&background, 0, 0, TFT_BLACK); // Upper-left corner for heart waveform
    breathWaveform.pushToSprite(&background, 0, background.height() / 2, TFT_BLACK); // Lower-left for breathing waveform

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
    heartRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    heartRateSprite.setTextColor(TFT_RED, TFT_BLACK);
    //heartRateSprite.setTextDatum(MC_DATUM);
    //heartRateSprite.setTextSize(4);
    heartRateSprite.fillSprite(TFT_BLACK);
    heartRateSprite.drawString(heartRateStr, heartRateSprite.width()*0.05, heartRateSprite.height()*0.2,6);
    heartRateSprite.pushToSprite(&background, background.width() - 90, 5, TFT_BLACK); // Top-right corner

    // Update breathing rate number sprite

    // Format the breathing rate number to always display 3 digits
    // Update breathing rate number sprite
    String breathRateStr;
    if (radarData.breathingRateEst < 10) {
        breathRateStr = "00" + String((int)radarData.breathingRateEst);
    } else if (radarData.breathingRateEst < 100) {
        breathRateStr = "0" + String((int)radarData.breathingRateEst);
    } else {
        breathRateStr = String((int)radarData.breathingRateEst);
    }
    breathRateSprite.fillSprite(TFT_BLACK); // Clear previous number
    breathRateSprite.setTextColor(TFT_GREEN, TFT_BLACK);
    //breathRateSprite.setTextDatum(MC_DATUM);
    //breathRateSprite.setTextSize(4);
    breathRateSprite.fillSprite(TFT_BLACK);
    breathRateSprite.drawString(breathRateStr, breathRateSprite.width()*0.05, breathRateSprite.height()*0.2,6);
    breathRateSprite.pushToSprite(&background, background.width() - 90, background.height() - 85, TFT_BLACK); // Bottom-right corner

    // Push the background sprite to the display
    background.pushSprite(0, 0);
}

