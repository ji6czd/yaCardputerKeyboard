/**
 * @file yaCardputerKeyboardLegacy.h
 * @brief 初代Cardputerをサポート
 * @author Seiken
 * @date 2025-10-20
 */

#ifndef _YACARDPUTERKEYBOARDLEGACY_H_
#define _YACARDPUTERKEYBOARDLEGACY_H_

#include "yaCardputerKeyboard.h"

class YaCardputerKeyboardLegacy : public YaCardputerKeyboard {
 public:
  YaCardputerKeyboardLegacy() = default;
  void begin() override;
  void updateKeyState() override;

 private:
  // キーボードの状態を保持するためのマトリックス
  using KeyMatrix = uint8_t[8];  // 8行のビットマスク配列
  KeyMatrix previousKeyMatrix = {0};
  KeyMatrix currentKeyMatrix = {0};

  const int rowSelectPins[3] = {8, 9, 11};  // GPIO G8(A0), G9(A1), G11(A2)
  const int colPins[7] = {3, 4, 5, 6, 7, 13, 15};
  const int numCols = sizeof(colPins) / sizeof(colPins[0]);

  void selectRow(byte row) {
    // 3つの制御ピンにrowの2進数表現を書き込む
    digitalWrite(rowSelectPins[0], (row & 0b001) ? HIGH : LOW);  // A0
    digitalWrite(rowSelectPins[1], (row & 0b010) ? HIGH : LOW);  // A1
    digitalWrite(rowSelectPins[2], (row & 0b100) ? HIGH : LOW);  // A2
  }
  void scanKeys(KeyMatrix& keyMatrix);
};

#endif  // _YACARDPUTERKEYBOARDLEGACY_H_