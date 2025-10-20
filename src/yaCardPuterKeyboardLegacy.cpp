#include "yaCardPuterKeyboardLegacy.h"

/**
 * @brief キーボード状態更新のバックグラウンドタスク
 */
void updateTask(void* param) {
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
void YaCardputerKeyboardLegacy::scanKeys(keyState& key) {
  memset(key, 0, sizeof(keyState));
  // 8つの行を順番にスキャン
  for (int r = 0; r < 8; r++) {
    // 現在の行をアクティブにする
    selectRow(r);

    // 少し待って信号を安定させる
    delayMicroseconds(50);

    // 7つの列をチェック
    for (int c = 0; c < numCols; c++) {
      if (digitalRead(colPins[c]) == LOW) {
        key[r] |= 1 << c;
      }
    }
  }
}

void YaCardputerKeyboardLegacy::updateKeyState() {
  // 最新のキー状態を取得
  scanKeys(currentKeyState);

  // 前回と同じ状態ならばなにもしない
  if (memcmp(currentKeyState, previousKeyState, sizeof(keyState)) == 0) {
    return;
  }
  // FIFOバッファに追加
  keyStateFIFO.push(currentKeyState);
  memcpy(previousKeyState, currentKeyState, sizeof(keyState));
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
  xTaskCreatePinnedToCore(updateTask, "KeyUpdateTask", 2048, this,
                          1,  // 優先度１
                          nullptr,
                          1  // コア1で実行
  );
  delay(50);  // キー入力を安定させるための待機
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

  for (int r = 0; r < 8; r++) {
    if (key[r] != 0) {
      int8_t col = getLSB(key[r]);
      if (col > 0 && col <= 7) {
        return keymap[r][col - 1];
      }
    }
  }
  return '\0';  // キーが見つからなかった場合はヌル文字を返す
}
