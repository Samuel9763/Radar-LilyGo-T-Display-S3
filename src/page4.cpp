#include <Arduino.h>
#include <TFT_eSPI.h>   // 包含 TFT_eSPI 庫，用於操作顯示屏
#include "page4.h"      // 包含自定義的頁面4的頭文件
#include "RadarData.h"  // 包含雷達數據的頭文件
#include <math.h>        // 用於進行正弦波的計算

// 用於產生心率 PQRST 波形的函數
float generatePQRSTWave(float phase) {
    if (phase < 0.1 * PI) {
        // P 波
        return -0.3 * sin(2 * (phase - 1.1 * PI));
    } else if (phase < 0.2 * PI) {
        // Q 波
        return 0.3 * sin(2 * (phase - 1.1 * PI));
    } else if (phase < 0.35 * PI) {
        // R 波
        return -0.75 * sin(2 * (phase - 1.1 * PI));
    } else if (phase < 0.45 * PI) {
        // S 波
        return 0.5 * sin(2 * (phase - 1.1 * PI));
    } else if (phase < 0.6 * PI) {
        // T 波
        return -0.5 * sin(2 * (phase - 1.1 * PI));
    } else if (phase < 1.2 * PI) { // 增加基線的相位範圍
        // 基線部分，延長基線的相位，使得波形之間的間隔更大
        return 0;
    } else {
        // 確保相位循環
        return 0;
    }
}

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
        waveformsSprite.createSprite(background.width() * 3 / 4, background.height());
        waveformsSprite.fillSprite(TFT_BLACK); // 填充背景為黑色
        heartRateSprite.createSprite(70, 75); // 設定大小以適應數字
        heartRateSprite.fillSprite(TFT_BLACK); // 填充背景為黑色
        breathRateSprite.createSprite(70, 75); // 設定大小以適應數字
        breathRateSprite.fillSprite(TFT_BLACK); // 填充背景為黑色
        isInitialized = true; // 設置初始化標誌為 true，防止重複初始化
    }

    // 用於佈局的常數
    const int waveformWidth = waveformsSprite.width();            // 合併波形的寬度
    const int waveformHeight = waveformsSprite.height();          // 合併波形的高度
    const int waveBaseY = waveformHeight / 2;                     // 波形的中心線位置
    const float amplitude = 50.0;                                 // 波形的振幅
    const float timeStep = 1.0 / 10;                           // 增加時間步長的頻率以增加波形點密度

    // 用於追踪波形更新的靜態變量
    static float heartPhase = 0.0;        // 心率波形的相位
    static float breathPhase = 0.0;       // 呼吸波形的相位
    static int x = 0;                     // 波形的 X 座標，用於兩個波形的共享
    static float lastHeartY = waveBaseY;  // 上一次心率波形的 Y 座標
    static float lastBreathY = waveBaseY; // 上一次呼吸波形的 Y 座標

    // 根據 radarData 更新相位
    float heartFrequency = radarData.heartRateEst * 0.9;        // 獲取心率估計值
    float breathFrequency = radarData.breathingRateEst * 0.3;   // 獲取呼吸率估計值

    // 減少心電圖的頻率，使波形之間的間隔變大
    heartPhase += (2 * PI * heartFrequency * timeStep) / 180; 
    if (heartPhase > 2 * PI) heartPhase -= 2 * PI;

    breathPhase += (2 * PI * breathFrequency * timeStep) / 60; 
    if (breathPhase > 2 * PI) breathPhase -= 2 * PI;

    // 計算當前的 Y 座標，使用 PQRST 波形來顯示心率
    float heartY = waveBaseY + amplitude * generatePQRSTWave(heartPhase); 
    float breathY = waveBaseY + amplitude * sin(breathPhase);

    // 清除前一段的波形以實現滾動效果
    
    int clearWidth = 25;  
    waveformsSprite.fillRect(x, 0, clearWidth, waveformHeight, TFT_BLACK);

    // 在共享的精靈上更新波形，使用更頻繁的繪製來增加波形的連續性
    waveformsSprite.drawLine(x - 1, lastHeartY, x, heartY, TFT_RED);
    waveformsSprite.drawLine(x - 1, lastBreathY, x, breathY, TFT_GREEN);

    // 更新上一次的 Y 座標
    lastHeartY = heartY;
    lastBreathY = breathY;
    x++;
    if (x >= waveformWidth) x = 0;

    // 繪製邊框
    waveformsSprite.drawRect(0, 0, waveformWidth, waveformHeight, TFT_WHITE);

    // 將波形精靈推送到背景
    waveformsSprite.pushToSprite(&background, 0, 0, TFT_BLACK);

    // 更新心率數字精靈
    heartRateSprite.fillSprite(TFT_BLACK);
    heartRateSprite.setTextColor(TFT_RED, TFT_BLACK);
    heartRateSprite.drawString(String((int)radarData.heartRateEst), heartRateSprite.width() * 0.05, heartRateSprite.height() * 0.2, 7);
    heartRateSprite.pushToSprite(&background, background.width() - 70, 5, TFT_BLACK);

    // 更新呼吸率數字精靈
    breathRateSprite.fillSprite(TFT_BLACK);
    breathRateSprite.setTextColor(TFT_GREEN, TFT_BLACK);
    breathRateSprite.drawString(String((int)radarData.breathingRateEst), breathRateSprite.width() * 0.05, breathRateSprite.height() * 0.2, 7);
    breathRateSprite.pushToSprite(&background, background.width() - 70, background.height() - 85, TFT_BLACK);

    // 將背景精靈推送到顯示屏
    background.pushSprite(0, 0);
}