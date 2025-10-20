/**
 * @file yaCardPuterKeyboardLegacy.h
 * @brief 初代CardPuterをサポート
 * @author Seiken
 * @date 2025-10-20
 */

#ifndef _YACARDPUTERKEYBOARDLEGACY_H_
#define _YACARDPUTERKEYBOARDLEGACY_H_

#include <Arduino.h>

#include "yaCardputerKeyboard.h"

class YaCardputerKeyboardLegacy : public YaCardputerKeyboard {
 public:
  YaCardputerKeyboardLegacy() = default;
  void begin() override;
  void updateKeyState() override;
  char getKey() override;

 private:
  /**
   * 旧CardPuter (V1.1)
   */
  const int rowSelectPins[3] = {8, 9, 11};  // GPIO G8(A0), G9(A1), G11(A2)
  const int colPins[7] = {3, 4, 5, 6, 7, 13, 15};
  const int numCols = sizeof(colPins) / sizeof(colPins[0]);

  /**
   * @brief 1バイトのビットマスクから、最下位ビットの位置を示す整数を返す
   * @param b ビットマスク（0〜255の範囲）
   * @return Onになっている最下位ビットの位置（1〜8）。0の場合は0を返す。
   * @note 例えば、bが0b00000100の場合、3を返す。
   *       bが0b00000000の場合、0を返す。
   *       bが0b00001100の場合、3を返す（最下位ビット）。
   *       bが0b11111111の場合、1を返す（最下位ビット）。
   */
  int8_t getLSB(uint8_t b) {
    if (b == 0) return 0;

    int8_t i = 1;
    while ((b & 1) == 0) {
      b >>= 1;
      i++;
    }
    return i;
  }

  /**
   * @brief 1バイトのビットマスクから、最上位ビットの位置を示す整数を返す
   * @param b ビットマスク（0〜255の範囲）
   * @return Onになっている最上位ビットの位置（1〜8）。0の場合は0を返す。
   * @note 例えば、bが0b00000100の場合、3を返す。
   *       bが0b00000000の場合、0を返す。
   *       bが0b00001100の場合、4を返す（最上位ビット）。
   *       bが0b11111111の場合、8を返す（最上位ビット）。
   */
  int8_t letMSB(uint8_t b) {
    if (b == 0) return 0;

    int8_t i = 8;
    while ((b & 0x80) == 0) {
      b <<= 1;
      i--;
    }
    return i;
  }

  void selectRow(byte row) {
    // 3つの制御ピンにrowの2進数表現を書き込む
    digitalWrite(rowSelectPins[0], (row & 0b001) ? HIGH : LOW);  // A0
    digitalWrite(rowSelectPins[1], (row & 0b010) ? HIGH : LOW);  // A1
    digitalWrite(rowSelectPins[2], (row & 0b100) ? HIGH : LOW);  // A2
  }
  void scanKeys(keyState& key) override;
};

#endif  // _YACARDPUTERKEYBOARDLEGACY_H_