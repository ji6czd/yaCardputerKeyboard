#ifndef _CARDPUTER_KEYBOARD_H_
#define _CARDPUTER_KEYBOARD_H_

#if __has_include(<M5Cardputer.h>)
#include <M5Cardputer.h>
#define USE_M5_CARDPUTER
#else
#include <Arduino.h>
#endif
#include "keymap.h"
/**
 * @struct keyState
 * @brief 物理キーの状態を表す構造体
 * @var keyState::row
 * 行番号（0〜7）
 * @var keyState::col
 * 列番号（0〜6）
 * @var keyState::pressed
 * 押下状態（true: 押下, false: 離上）
 * @note 操作されたキーの情報がFIFOバッファに格納さる。
 */
struct keyState {
  uint8_t row : 4;
  uint8_t col : 3;
  bool pressed : 1;
} __attribute__((packed));

class KeyStateFIFO {
 public:
  KeyStateFIFO() : head(0), tail(0), count(0) {}

  // バッファにkeyStateを追加（FIFO）
  bool push(const keyState newKeyState) {
    if (count >= BUFFER_SIZE) {
      // バッファが満杯の場合、最も古いデータを上書き
      tail = (tail + 1) % BUFFER_SIZE;
    } else {
      count++;
    }

    // データをコピー
    buffer[head] = newKeyState;

    head = (head + 1) % BUFFER_SIZE;
    return true;
  }

  // バッファからkeyStateを取り出し（FIFO）
  bool pop(keyState& outKeyState) {
    if (count == 0) {
      return false;  // バッファが空
    }

    // データをコピー
    outKeyState = buffer[tail];

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
    outKeyState = buffer[tail];

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
    outKeyState = buffer[newestIndex];

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
    outKeyState = buffer[actualIndex];

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
  virtual bool peekKeyState(keyState& outKeyState) const {
    return keyStateFIFO.peek(outKeyState);
  }
  virtual bool isBufferEmpty() const { return keyStateFIFO.isEmpty(); }
  virtual size_t getBufferSize() const { return keyStateFIFO.size(); }
  virtual char getKey() {
    keyState key;
    if (!popKeyState(key)) {
      return '\0';  // バッファが空
    }
    return keymap[key.row][key.col];
  }
  virtual bool isKeypressed(char c) {
    keyState key;
    for (int i = 0; i < keyStateFIFO.size(); i++) {
      if (keyStateFIFO.getAt(i, key)) {
        if (key.pressed && keymap[key.row][key.col] == c) {
          return true;
        }
      }
    }
    return false;
  }

  virtual bool setKeymap(const char keymap[8][7]) final {
    memcpy(this->keymap, keymap, sizeof(this->keymap));
    return true;
  }

 protected:
  KeyStateFIFO keyStateFIFO;
  char keymap[8][7];
};

#endif  // _CARDPUTER_KEYBOARD_H_