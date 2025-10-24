#ifndef _KEYMAP_H_
#define _KEYMAP_H_

constexpr char SHIFT = 0x01;
constexpr char KEY_ALT = 0x02;
constexpr char KEY_OPT = 0x03;
constexpr char KEY_FN = 0x04;

// キーのマッピング
const char phone_keymap[8][7] = {
    {'1', '3'},                               // Row 0
    {'4', '6'},                               // Row 1
    {'7', '9'},                               // Row 2
    {'*', '#'},                               // Row 3
    {'2', 'A', '\0', '\0', '\0', '\0', '+'},  // Row 4
    {'5', 'B', '\0', '\0', '\0', '\0', '-'},  // Row 5
    {'8', 'C'},                               // Row 6
    {'0', 'D'}                                // Row 7
};

const char cardputer_keymap[8][7] = {
    {'`', '2', '4', '6', '8', '0', '='},    // Row 0
    {'\t', 'w', 'r', 'y', 'i', 'p', ']'},   // Row 1
    {'\0', 'a', 'd', 'g', 'j', 'l', '\''},  // Row 2
    {'\0', '\0', 'x', 'v', 'n', ',', '/'},  // Row 3
    {'1', '3', '5', '7', '9', '-', '\b'},   // Row 4
    {'q', 'e', 't', 'u', 'o', '[', '\\'},   // Row 5
    {'\0', 's', 'f', 'h', 'k', ';', '\r'},  // Row 6
    {'\0', 'z', 'c', 'b', 'm', '.', ' '}    // Row 7
};

const char cardputer_shift_keymap[8][7] = {
    {'~', '@', '$', '^', '*', ')', '+'},    // Row 0
    {'\t', 'W', 'R', 'Y', 'I', 'P', '}'},   // Row 1
    {'\0', 'A', 'D', 'G', 'J', 'L', '\"'},  // Row 2
    {'\0', '\0', 'X', 'V', 'N', '<', '?'},  // Row 3
    {'!', '#', '%', '&', '(', '_', '\b'},   // Row 4
    {'Q', 'E', 'T', 'U', 'O', '{', '|'},    // Row 5
    {'\0', 'S', 'F', 'H', 'K', ':', '\r'},  // Row 6
    {'\0', 'Z', 'C', 'B', 'M', '>', ' '}    // Row 7
};
#endif  // _KEYMAP_PHONE_KEY_H_
