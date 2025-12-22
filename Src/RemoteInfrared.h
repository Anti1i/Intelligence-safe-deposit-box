#ifndef __REMOTE_INFRARED_H
#define __REMOTE_INFRARED_H

#include <stdint.h>

/* 红外键值映射 (根据实际遥控器修改) */
#define IR_KEY_OPEN   0x08  // 键 '1'
#define IR_KEY_CLOSE  0x88  // 键 '2'
#define IR_KEY_MUTE   0x78  // 键 'Delete' (消警)

void Remote_Infrared_TimeTick(void);
uint8_t Remote_Infrared_GetKeyCode(void);

#endif
