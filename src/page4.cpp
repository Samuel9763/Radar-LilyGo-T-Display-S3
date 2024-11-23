#include <Arduino.h>
#include <TFT_eSPI.h>   // 包含 TFT_eSPI 庫，用於操作顯示屏
#include "page4.h"      // 包含自定義的頁面4的頭文件
#include "RadarData.h"  // 包含雷達數據的頭文件
#include <math.h>        // 用於進行正弦波的計算

void showPage4(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    // 清除背景，當切換到這個頁面時完全重置背景
    background.fillSprite(TFT_BLACK);
    int lineWidth = 3; // 設定波形的線條厚度

    // 創建顯示合併波形和心率、呼吸率數字的精靈（Sprite）
    static TFT_eSprite waveformsSprite = TFT_eSprite(&tft);
    static TFT_eSprite heartRateSprite = TFT_eSprite(&tft);
    static TFT_eSprite breathRateSprite = TFT_eSprite(&tft);
    static bool isInitialized = false;

    if (!isInitialized) {
        // 創建合併波形的精靈
        waveformsSprite.createSprite(background.width() * 3 / 4, background.height());
        waveformsSprite.fillSprite(TFT_BLACK); // 填充背景為黑色

        // 創建顯示心率的數字精靈
        heartRateSprite.createSprite(70, 75); // 設定大小以適應數字
        heartRateSprite.fillSprite(TFT_BLACK); // 填充背景為黑色

        // 創建顯示呼吸率的數字精靈
        breathRateSprite.createSprite(70, 75); // 設定大小以適應數字
        breathRateSprite.fillSprite(TFT_BLACK); // 填充背景為黑色

        isInitialized = true; // 設置初始化標誌為 true，防止重複初始化
    }

    // 用於佈局的常數
    const int waveformWidth = waveformsSprite.width();            // 合併波形的寬度
    const int waveformHeight = waveformsSprite.height();          // 合併波形的高度
    const int waveBaseY = waveformHeight / 2;                     // 波形的中心線位置
    const float amplitude = 50.0;                                 // 波形的振幅
    const float timeStep = 1.0 / 60.0;                            // 每幀的時間增量（假設約 60 FPS）

    // 用於追踪波形更新的靜態變量
    static float heartPhase = 0.0;        // 心率波形的相位
    static float breathPhase = 0.0;       // 呼吸波形的相位
    static int x = 0;                     // 波形的 X 座標，用於兩個波形的共享
    static float lastHeartY = waveBaseY;  // 上一次心率波形的 Y 座標
    static float lastBreathY = waveBaseY; // 上一次呼吸波形的 Y 座標

    // 根據 radarData 更新相位
    float heartFrequency = radarData.heartRateEst;        // 獲取心率估計值
    float breathFrequency = radarData.breathingRateEst;   // 獲取呼吸率估計值

    heartPhase += (2 * PI * heartFrequency * timeStep) / 60; // 心率波形更新（慢速視覺化）
    if (heartPhase > 2 * PI) heartPhase -= 2 * PI;           // 如果超過 2 PI，重置相位

    breathPhase += (2 * PI * breathFrequency * timeStep) / 60; // 呼吸波形更新（慢速視覺化）
    if (breathPhase > 2 * PI) breathPhase -= 2 * PI;           // 如果超過 2 PI，重置相位

    // 計算當前的 Y 座標
    float heartY = waveBaseY + amplitude * sin(heartPhase);    // 心率波形的 Y 座標
    float breathY = waveBaseY + amplitude * sin(breathPhase);  // 呼吸波形的 Y 座標

    // 在共享的精靈上更新波形
    if (x == 0) waveformsSprite.fillRect(x, 0, lineWidth, waveformHeight, TFT_BLACK); // 覆蓋舊的心率波形
    waveformsSprite.fillRect(x + lineWidth, 0, 1, waveformHeight, TFT_BLACK);         // 清除心率波形的舊位置
    waveformsSprite.fillRect(x, lastHeartY - lineWidth / 2, 2, lineWidth, TFT_RED);   // 畫出新的心率波形

    waveformsSprite.fillRect(x, lastBreathY - lineWidth / 2, 2, lineWidth, TFT_GREEN); // 畫出新的呼吸波形

    // 更新上一次的 Y 座標
    lastHeartY = heartY;
    lastBreathY = breathY;
    x++;
    if (x >= waveformWidth) x = 0; // 如果 X 座標超出波形寬度，重置 X

    // 繪製邊框
    waveformsSprite.drawRect(0, 0, waveformWidth, waveformHeight, TFT_WHITE); // 繪製波形框架

    // 將波形精靈推送到背景
    waveformsSprite.pushToSprite(&background, 0, 0, TFT_BLACK); // 對齊左上角

    // 更新心率數字精靈
    heartRateSprite.fillSprite(TFT_BLACK); // 清除之前的數字
    heartRateSprite.setTextColor(TFT_RED, TFT_BLACK); // 設置心率文字顏色
    heartRateSprite.drawString(String((int)radarData.heartRateEst), heartRateSprite.width() * 0.05, heartRateSprite.height() * 0.2, 7);
    heartRateSprite.pushToSprite(&background, background.width() - 70, 5, TFT_BLACK); // 推送到背景的右上角

    // 更新呼吸率數字精靈
    breathRateSprite.fillSprite(TFT_BLACK); // 清除之前的數字
    breathRateSprite.setTextColor(TFT_GREEN, TFT_BLACK); // 設置呼吸率文字顏色
    breathRateSprite.drawString(String((int)radarData.breathingRateEst), breathRateSprite.width() * 0.05, breathRateSprite.height() * 0.2, 7);
    breathRateSprite.pushToSprite(&background, background.width() - 70, background.height() - 85, TFT_BLACK); // 推送到背景的右下角

    // 將背景精靈推送到顯示屏
    background.pushSprite(0, 0);
}

/*
    這段註解掉的程式碼是另一種頁面展示的方式。
    它創建了一個旋轉的針，用於顯示雷達數據。
    每次呼叫時，它會增加角度，讓針旋轉。
*/

/*
void showPage4(TFT_eSPI &tft, TFT_eSprite &background, RadarData &radarData) {
    static int angle = 0;  // 追踪旋轉角度

    // 清除背景並設置一個銀色的圓形
    background.fillSprite(TFT_BLACK);  // 清屏
    int centerX = tft.width() / 2;
    int centerY = tft.height() / 2;
    background.fillCircle(centerX, centerY, 80, TFT_SILVER);  // 畫出銀色圓形

    // 創建針的精靈，並用紅色填充
    TFT_eSprite needle = TFT_eSprite(&tft);
    needle.createSprite(20, 80);  // 針的大小：20x80
    needle.fillSprite(TFT_BLACK);  // 透明背景
    needle.drawWedgeLine(10, 0, 10, 40, 1, 10, TFT_RED);
    needle.drawWedgeLine(10, 40, 10, 80, 10, 1, TFT_BLUE);
    needle.fillCircle(10, 40, 10, TFT_WHITE);

    // 旋轉針並將其推送到背景
    needle.pushRotated(&background, angle, TFT_BLACK);
    needle.deleteSprite();  // 推送後刪除針精靈
    background.pushSprite(0, 0);

    // 增加角度以進行下一幀的旋轉
    angle += 5;  // 調整旋轉速度
    if (angle >= 360) angle = 0;  // 旋轉一圈後重置角度
}
*/
