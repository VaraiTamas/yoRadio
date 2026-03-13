#ifndef controls_h
#define controls_h
#include "common.h"

#if IR_PIN!=255
enum : uint8_t { IR_POWER=0, IR_PLAY_STOP=1, IR_BACK=2, IR_PREV=3, IR_LIST=4, IR_NEXT=5, IR_VOLUME_DOWN=6, IR_MODE=7, IR_VOLUME_UP=8, IR_1=9, IR_2=10, IR_3=11, IR_4=12, IR_5=13, IR_6=14, IR_7=15, IR_8=16, IR_9=17, IR_0=18  };
#endif

boolean checklpdelay(int m, unsigned long &tstamp);

void initControls();
void loopControls();
#if (ENC_BTNL!=255 && ENC_BTNR!=255) || (ENC2_BTNL!=255 && ENC2_BTNR!=255)
class yoEncoder;
void encodersLoop(yoEncoder *enc, bool first=true);
#endif
void encoder1Loop();
void encoder2Loop();
void irLoop();
void irNumber(uint8_t num);
void irBlink();
void controlsEvent(bool toRight, int8_t volDelta = 0);

void onBtnClick(int id);
void onBtnDoubleClick(int id);
void onBtnDuringLongPress(int id);
void onBtnLongPressStart(int id);
void onBtnLongPressStop(int id);

void setIRTolerance(uint8_t tl);
void setEncAcceleration(uint16_t acc);
void flipTS();

extern __attribute__((weak)) void ctrls_on_loop();

#endif
