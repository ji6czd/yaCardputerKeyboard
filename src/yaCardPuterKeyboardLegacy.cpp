#include "yaCardputerKeyboardLegacy.h"

/**
 * @brief キーボード状態更新のバックグラウンドタスク
 */
void updateLegacyTask(void* param) {
  YaCardputerKeyboardLegacy* keyboard =
      static_cast<YaCardputerKeyboardLegacy*>(param);
  while (true) {
    keyboard->updateKeyState();
    vTaskDelay(10 / portTICK_PERIOD_MS);  // 10msごとに更新
  }
}

/**
 * @brief キー状態の更新
 */
void YaCardputerKeyboardLegacy::scanKeys(KeyMatrix& key) {
  memset(&key, 0, sizeof(KeyMatrix));  // すべてのキーを未押下に初期化
  // 8つの行を順番にスキャン
  for (int r = 0; r < 8; r++) {
    // 現在の行をアクティブにする
    selectRow(r);

    // 少し待って信号を安定させる
    delayMicroseconds(50);

    // 7つの列をチェック
    for (int c = 0; c < numCols; c++) {
      if (digitalRead(colPins[c]) == LOW) {
        // See note about key layout in README.md
        uint8_t row = (7 - r);
        uint8_t col = (c > 4) ? c - 5 : c + 2;
        key[row] |= 1 << col;
      }
    }
  }
}

void YaCardputerKeyboardLegacy::updateKeyState() {
  // 最新のキー状態を取得
  scanKeys(currentKeyMatrix);

  // 前回と同じ状態ならばなにもしない
  if (memcmp(currentKeyMatrix, previousKeyMatrix, sizeof(KeyMatrix)) == 0) {
    return;
  }
  // 各キーの状態を比較して変化を検出
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t changed = currentKeyMatrix[r] ^ previousKeyMatrix[r];
    uint8_t pressed = changed & currentKeyMatrix[r];
    uint8_t released = changed & previousKeyMatrix[r];
    // 話されたキーをバッファに追加
    for (uint8_t c = 0; c < 7; c++) {
      if (released & (1 << c)) {
        keyState key = {r, c, false};
        keyStateFIFO.push(key);
      }
    }
    for (uint8_t c = 0; c < 7; c++) {
      // 押されたキーをバッファに追加
      if (pressed & (1 << c)) {
        keyState key = {r, c, true};
        keyStateFIFO.push(key);
      }
    }
  }
  memcpy(previousKeyMatrix, currentKeyMatrix, sizeof(KeyMatrix));
}

void YaCardputerKeyboardLegacy::begin() {
  // 行の選択ピンをOUTPUTに設定
  for (int i = 0; i < 3; i++) {
    pinMode(rowSelectPins[i], OUTPUT);
  }

  // 列のピンをINPUT_PULLUPに設定
  // これにより、キーが押されていないときはHIGHになり、
  // 押されたときにアクティブな行（LOW）と接触してLOWになります。
  for (int i = 0; i < numCols; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }
  // バックグラウンドでキー状態更新タスクを開始
  xTaskCreatePinnedToCore(updateLegacyTask, "KeyUpdateLegacyTask", 2048, this,
                          1,  // 優先度１
                          nullptr,
                          1  // コア1で実行
  );
  delay(50);  // 1回はscan*(が実行されなければならない。)
}

/**
 * @brief 押されたキーを取得
 * @return 押されたキーの文字。バッファが空の場合はヌル文字を返す。
 */
char YaCardputerKeyboardLegacy::getKey() {
  keyState key;
  if (!popKeyState(key)) {
    return '\0';  // バッファが空ならばヌル文字を返す
  }
  return (key.pressed) ? keymap[key.row][key.col] : '\0';
}
