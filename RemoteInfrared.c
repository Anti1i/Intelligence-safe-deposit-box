#include "RemoteInfrared.h"
#include "Hardware_Config.h"

// 计时变量 (由TIM3中断每100us减1)
volatile uint32_t IR_Time_Counter = 0;
volatile uint8_t  IR_Data_Ready = 0;
uint32_t IR_Raw_Code = 0;

void Remote_Infrared_TimeTick(void)
{
    if (IR_Time_Counter > 0) IR_Time_Counter--;
}

// 外部中断回调 (下降沿触发)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint8_t bit_cnt = 0;
    static uint32_t key_code = 0;
    
    if (GPIO_Pin != GPIO_PIN_13) return;

    // NEC协议解码逻辑
    // 1. 引导码监测 (9ms低 + 4.5ms高)
    // 2. 数据监测 (逻辑0: 1.12ms, 逻辑1: 2.25ms)
    // 简化实现: 通过两次下降沿之间的时间间隔判断
    
    uint32_t interval = 1000 - IR_Time_Counter; // 距离上次中断的时间(x100us)
    IR_Time_Counter = 1000; // 重置计时器 (100ms超时)
    
    if (interval > 130 && interval < 140) { // 13.5ms (9+4.5) 引导码
        bit_cnt = 0;
        key_code = 0;
    }
    else if (interval > 10 && interval < 13) { // 1.12ms -> 逻辑0
        key_code <<= 1;
        bit_cnt++;
    }
    else if (interval > 21 && interval < 24) { // 2.25ms -> 逻辑1
        key_code <<= 1;
        key_code |= 1;
        bit_cnt++;
    }
    
    if (bit_cnt == 32) { // 32位数据接收完毕
        IR_Raw_Code = key_code;
        IR_Data_Ready = 1;
        bit_cnt = 0;
    }
}

uint8_t Remote_Infrared_GetKeyCode(void)
{
    if (IR_Data_Ready) {
        IR_Data_Ready = 0;
        // 解析格式: ID(8) + ~ID(8) + Key(8) + ~Key(8)
        uint8_t key = (IR_Raw_Code >> 8) & 0xFF;
        uint8_t key_inv = IR_Raw_Code & 0xFF;
        
        if (key == (uint8_t)(~key_inv)) {
            return key;
        }
    }
    return 0;
}