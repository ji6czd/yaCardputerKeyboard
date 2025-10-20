#ifndef _CARDPUTER_KEYBOARD_H_
#define _CARDPUTER_KEYBOARD_H_

#include <Arduino.h>

#include "keymap.h"
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
  bool peek(keyState& outKeyState) const {
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
  bool peekNewest(keyState& outKeyState) const {
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
  bool getAt(size_t index, keyState& outKeyState) const {
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

class YaCardputerKeyboard {
 public:
  YaCardputerKeyboard() { setKeymap(default_keymap); }
  virtual void begin() = 0;
  virtual void updateKeyState() = 0;
  virtual bool popKeyState(keyState& outKeyState) {
    return keyStateFIFO.pop(outKeyState);
  }
  virtual bool getLatestKeyState(keyState& outKeyState) const {
    return keyStateFIFO.peekNewest(outKeyState);
  }
  virtual bool isBufferEmpty() const { return keyStateFIFO.isEmpty(); }
  virtual size_t getBufferSize() const { return keyStateFIFO.size(); }
  virtual char getKey() = 0;
  virtual bool setKeymap(const char keymap[8][7]) final {
    memcpy(this->keymap, keymap, sizeof(this->keymap));
    return true;
  }

 protected:
  KeyStateFIFO keyStateFIFO;
  keyState currentKeyState;
  keyState previousKeyState;
  char keymap[8][7];
  virtual void scanKeys(keyState& key) = 0;
};

#endif  // _CARDPUTER_KEYBOARD_H_