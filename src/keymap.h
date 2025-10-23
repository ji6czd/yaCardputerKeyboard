#ifndef _KEYMAP_PHONE_KEY_H_
#define _KEYMAP_PHONE_KEY_H_

// キーのマッピング
const char default_keymap[8][7] = {
    {'1', '3'},                               // Row 0
    {'4', '6'},                               // Row 1
    {'7', '9'},                               // Row 2
    {'*', '#'},                               // Row 3
    {'2', 'A', '\0', '\0', '\0', '\0', '+'},  // Row 4
    {'5', 'B', '\0', '\0', '\0', '\0', '-'},  // Row 5
    {'8', 'C'},                               // Row 6
    {'0', 'D'}                                // Row 7
};

const char en_us_keymap[8][7] = {};

#endif  // _KEYMAP_PHONE_KEY_H_
