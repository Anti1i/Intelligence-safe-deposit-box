#ifndef __REMOTE_INFRARED_H
#define __REMOTE_INFRARED_H

#include "stm32f4xx_hal.h"

/* 红外接收引脚定义 - 你的板子使用PF15 */
#define Remote_Infrared_DAT_INPUT  HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15)

/*
 * 红外按键定义 - 使用完整的32位Raw Data
 * 
 * 你的遥控器发送的数据示例：
 * - 按键1: 0x00FF08F7
 * - 按键2: 0x00FF8877
 * 
 * 请按遥控器的每个键，查看串口输出的Raw Data值，
 * 然后在下面配置对应的键值。
 */

/* ====== 请根据你的遥控器修改这里 ====== */

#define IR_RAW_KEY_UNLOCK   0x00FF08F7  // 开锁键（按遥控器看串口输出修改）
#define IR_RAW_KEY_LOCK     0x00FF8877  // 关锁键（按遥控器看串口输出修改）
#define IR_RAW_KEY_STOP     0x00FFFFFF  // 紧急停止键（按遥控器看串口输出修改）

/* ======================================= */

/* 红外数据联合体 */
typedef union
{
    uint32_t uiRemoteInfraredData;
    struct
    {
        uint8_t bID;
        uint8_t bIDNot;
        uint8_t bKeyCode;
        uint8_t bKeyCodeNot;
    } RemoteInfraredDataStruct;
} Remote_Infrared_data_union;

/* 外部变量声明 */
extern __IO uint32_t FlagGotKey;
extern __IO Remote_Infrared_data_union RemoteInfrareddata;

/* 函数声明 */
void Remote_Infrared_KEY_ISR(void);
uint32_t Remote_Infrared_GetRawData(void);

#endif
