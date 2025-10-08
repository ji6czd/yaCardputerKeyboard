#ifndef _CARDPUTER_KEYBOARD_H_
#define _CARDPUTER_KEYBOARD_H_

#include <Arduino.h>

// FIFOバッファの実装

using keyState = uint8_t[8];
class KeyStateFIFO {
 public:
  KeyStateFIFO() : head(0), tail(0), count(0) {}

  // バッファにkeyStateを追加（FIFO）
  bool push(const keyState& newKeyState) {
    if (count >= BUFFER_SIZE) {
      // バッファが満杯の場合、最も古いデータを上書き
      tail = (tail + 1) % BUFFER_SIZE;
    } else {
      count++;
    }

    // データをコピー
    for (int i = 0; i < 8; i++) {
      buffer[head][i] = newKeyState[i];
    }

    head = (head + 1) % BUFFER_SIZE;
    return true;
  }

  // バッファからkeyStateを取り出し（FIFO）
  bool pop(keyState& outKeyState) {
    if (count == 0) {
      return false;  // バッファが空
    }

    // データをコピー
    for (int i = 0; i < 8; i++) {
      outKeyState[i] = buffer[tail][i];
    }

    tail = (tail + 1) % BUFFER_SIZE;
    count--;
    return true;
  }

  // バッファの先頭データを取得（取り出さない）
  bool peek(keyState& outKeyState) {
    if (count == 0) {
      return false;  // バッファが空
    }

    // データをコピー
    for (int i = 0; i < 8; i++) {
      outKeyState[i] = buffer[tail][i];
    }

    return true;
  }

  // バッファの最新データを取得（取り出さない）
  bool peekNewest(keyState& outKeyState) {
    if (count == 0) {
      return false;  // バッファが空
    }

    // 最新のデータは head の1つ前の位置にある
    size_t newestIndex = (head + BUFFER_SIZE - 1) % BUFFER_SIZE;

    // データをコピー
    for (int i = 0; i < 8; i++) {
      outKeyState[i] = buffer[newestIndex][i];
    }

    return true;
  }

  // バッファが空かどうか
  bool isEmpty() const { return count == 0; }

  // バッファが満杯かどうか
  bool isFull() const { return count >= BUFFER_SIZE; }

  // 現在の要素数を取得
  size_t size() const { return count; }

  // バッファをクリア
  void clear() {
    head = 0;
    tail = 0;
    count = 0;
  }

  // 指定位置のデータを取得（0が最も古い、size()-1が最も新しい）
  bool getAt(size_t index, keyState& outKeyState) {
    if (index >= count) {
      return false;
    }

    size_t actualIndex = (tail + index) % BUFFER_SIZE;
    for (int i = 0; i < 8; i++) {
      outKeyState[i] = buffer[actualIndex][i];
    }

    return true;
  }

 private:
  static constexpr size_t BUFFER_SIZE = 16;
  keyState buffer[BUFFER_SIZE];  // 16個のkeyStateを格納
  size_t head;                   // 次に書き込む位置
  size_t tail;                   // 次に読み出す位置
  size_t count;                  // 現在格納されている要素数
};

class CardputerKeyboard {
 public:
  CardputerKeyboard() = default;
  void begin();
  void updateKeyState();
  bool popKeyState(keyState& outKeyState) {
    return keyStateFIFO.pop(outKeyState);
  }
  bool getLatestKeyState(keyState& outKeyState) {
    return keyStateFIFO.peekNewest(outKeyState);
  }
  bool isBufferEmpty() const { return keyStateFIFO.isEmpty(); }
  size_t getBufferSize() const { return keyStateFIFO.size(); }
  char getKey();

 private:
  KeyStateFIFO keyStateFIFO;
  keyState currentKeyState;
  keyState previousKeyState;

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
  void scanKeys(keyState& key);
};

extern CardputerKeyboard cardKey;

#endif  // _CARDPUTER_KEYBOARD_H_