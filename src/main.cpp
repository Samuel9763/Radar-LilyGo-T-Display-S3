#include <Arduino.h>

// 使用更具描述性的名稱
const int BUTTON_PIN_1 = 17; // 接按鈕的一端
const int BUTTON_PIN_2 = 18; // 接按鈕的另一端或 GND

void setup() {
  pinMode(BUTTON_PIN_1, INPUT_PULLUP); // 設置 BUTTON_PIN_1 為輸入，啟用內建上拉電阻
  pinMode(BUTTON_PIN_2, INPUT_PULLUP); // 設置 BUTTON_PIN_2 為輸入，啟用內建上拉電阻
}

void loop() {
  // 偵測按鈕按下狀態（檢查 BUTTON_PIN_1 和 BUTTON_PIN_2 是否為低電平）
  if (digitalRead(BUTTON_PIN_1) == LOW || digitalRead(BUTTON_PIN_2) == LOW) {
    delay(50); // 去抖動
    if (digitalRead(BUTTON_PIN_1) == LOW || digitalRead(BUTTON_PIN_2) == LOW) {
      long pressTime = millis();
      while (digitalRead(BUTTON_PIN_1) == LOW || digitalRead(BUTTON_PIN_2) == LOW) {
        if (millis() - pressTime > 3000) { // 長按 3 秒觸發關機
          enterDeepSleep();
        }
      }
    }
  }
}

void enterDeepSleep() {
  // 使用 ESP-IDF 或 Arduino 的深度睡眠函數
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN_1, 0); // 使用 BUTTON_PIN_1 喚醒
  esp_deep_sleep_start(); // 進入深度睡眠模式
}
