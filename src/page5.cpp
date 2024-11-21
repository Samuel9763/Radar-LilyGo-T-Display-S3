#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page5.h"
#include "RadarData.h"

void showPage5(TFT_eSPI &tft, TFT_eSprite &background,RadarData &radarData) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("No UART data ...", tft.width() / 2, tft.height() / 2);
}
