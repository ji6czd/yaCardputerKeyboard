#include "CardputerKeyboard.h"

#include <M5Cardputer.h>

#include "keymap_phone_key.h"

CardputerKeyboard cardKey;

void CardputerKeyboard::begin() {
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
}

void CardputerKeyboard::scanKeys(keyState& key) {
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

void CardputerKeyboard::updateKeyState() {
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

char CardputerKeyboard::getKey() {
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
  Serial.println();
  return '\0';  // キーが見つからなかった場合はヌル文字を返す
}
