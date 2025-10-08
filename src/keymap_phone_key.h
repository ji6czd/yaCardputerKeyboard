#ifndef _KEYMAP_PHONE_KEY_H_
#define _KEYMAP_PHONE_KEY_H_

#include <Arduino.h>

// キーのマッピング
const char keymap[8][7] = {
    {'a', 'b', 'c', 'd', 'e', '0', 'f'}, {'g', 'h', 'i', 'j', 'k', '8', 'l'},
    {'m', 'n', 'o', 'p', 'q', '5', 'r'}, {'s', 't', 'u', 'v', 'w', '2', 'x'},
    {'y', 'z', 'A', 'B', 'C', '*', '#'}, {'D', 'E', 'F', 'G', 'H', '7', '9'},
    {'I', 'J', 'K', 'L', 'M', '4', '6'}, {'N', 'O', 'P', 'Q', 'R', '1', '3'}};

#endif  // _KEYMAP_PHONE_KEY_H_
