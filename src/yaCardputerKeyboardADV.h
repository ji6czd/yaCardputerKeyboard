#ifndef _YACARDPUTERKEYBOARDADV_H_
#define _YACARDPUTERKEYBOARDADV_H_

#include "yaCardputerKeyboard.h"

class YaCardputerKeyboardADV : public YaCardputerKeyboard {
 public:
  YaCardputerKeyboardADV() = default;
  void begin() override;
  void updateKeyState() override;
};
#endif  // _YACARDPUTERKEYBOARDADV_H_