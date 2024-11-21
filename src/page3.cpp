#include <Arduino.h>
#include <TFT_eSPI.h>
#include "page3.h"
#include "RadarData.h"

void showPage3(TFT_eSPI &tft, TFT_eSprite &background,RadarData &radarData) {
    static TFT_eSprite txtSprite = TFT_eSprite(&tft);
    static int x = 0;

    background.fillSprite(TFT_BROWN);
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
