#include "yaCardputerKeyboardADV.h"

#if __has_include("utility/Adafruit_TCA8418/Adafruit_TCA8418.h")
#include "utility/Adafruit_TCA8418/Adafruit_TCA8418.h"
#else
#include <Adafruit_TCA8418.h>
#endif
Adafruit_TCA8418 keypad;

void updateADVTask(void* pvParameters) {
  YaCardputerKeyboardADV* keyboard =
      static_cast<YaCardputerKeyboardADV*>(pvParameters);
  while (true) {
    keyboard->updateKeyState();
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

void YaCardputerKeyboardADV::begin() {
  Wire.setPins(8, 9);  // SDA = GPIO21, SCL = GPIO22
  if (keypad.begin() == false) {
    Serial.println("TCA8418 not found!");
    while (1);
  }
  // 7行8列のマトリックスキーボードとして設定
  if (keypad.matrix(7, 8) == false) {
    Serial.println("Failed to set matrix size!");
    while (1);
  }
  xTaskCreatePinnedToCore(updateADVTask, "KeyUpdateTask", 2048, this, 1, NULL,
                          1);
}

void YaCardputerKeyboardADV::updateKeyState() {
  while (keypad.available()) {
    union keyEvent {
      uint8_t byte;
      struct {
        uint8_t data : 7;
        bool pressed : 1;
      };
    };

    keyEvent ev;
    ev.byte = keypad.getEvent();
    keyState key;
    key.col = (ev.data / 10);
    key.row = (ev.data % 10) - 1;
    key.pressed = ev.pressed;
    keyStateFIFO.push(key);
  }
}
