#include "RemoteInfrared.h"
#include <stdio.h>

extern __IO uint32_t GlobalTimingDelay100us;

__IO uint32_t FlagGotKey = 0;
__IO Remote_Infrared_data_union RemoteInfrareddata;

const uint32_t TIME_DELAY_6MS = 60;
const uint32_t TIME_DELAY_10MS = 100;

/**
 * @brief  红外中断服务程序 - NEC协议解码
 */
void Remote_Infrared_KEY_ISR(void)
{
    static __IO uint8_t  bBitCounter = 0;
    static __IO uint32_t bKeyCode = 0;
    
    bBitCounter++;

    if(bBitCounter == 1)
    {
        if(Remote_Infrared_DAT_INPUT)
        {
            bBitCounter = 0;
        }
        else
        {
            GlobalTimingDelay100us = TIME_DELAY_10MS;
        }
    }
    else if(bBitCounter == 2)
    {
        if(Remote_Infrared_DAT_INPUT)
        {
            if((GlobalTimingDelay100us > 2) && (GlobalTimingDelay100us < 18))
            {
                GlobalTimingDelay100us = TIME_DELAY_6MS;
            }
            else
            { 
                bBitCounter = 0; 
            }
        }
        else
        {
            bBitCounter = 0;            
        }
    }
    else if(bBitCounter == 3)
    {
        if(Remote_Infrared_DAT_INPUT)
        {
            bBitCounter = 0; 
        }
        else
        {
            if((GlobalTimingDelay100us > 5) && (GlobalTimingDelay100us < 20))
            {
                GlobalTimingDelay100us = TIME_DELAY_6MS;
            }
            else if((GlobalTimingDelay100us > 32) && (GlobalTimingDelay100us < 46))
            {
                // 重复码
                bBitCounter = 0;
                RemoteInfrareddata.uiRemoteInfraredData = bKeyCode;
                FlagGotKey = 1;
            }            
            else
            {
                bBitCounter = 0; 
            }          
        }
    }    
    else if(bBitCounter > 3 && bBitCounter < 68)
    {  
        if(Remote_Infrared_DAT_INPUT)
        {
            if((GlobalTimingDelay100us > 50) && (GlobalTimingDelay100us < 58))
            {
                GlobalTimingDelay100us = TIME_DELAY_6MS;
            }
            else
            {
                bBitCounter = 0; 
            }           
        }
        else
        {
            if((GlobalTimingDelay100us > 50) && (GlobalTimingDelay100us < 58))
            {
                GlobalTimingDelay100us = TIME_DELAY_6MS;
                bKeyCode <<= 1;
                bKeyCode += 0x00;
            }
            else if((GlobalTimingDelay100us > 40) && (GlobalTimingDelay100us < 48))
            {
                GlobalTimingDelay100us = TIME_DELAY_6MS;            
                bKeyCode <<= 1;
                bKeyCode += 0x01;
            }              
            else
            {
                bBitCounter = 0; 
            }  
        }

        if(bBitCounter == 67)
        {
            RemoteInfrareddata.uiRemoteInfraredData = bKeyCode;
            bBitCounter = 0;
            FlagGotKey = 1;
        }
    }
    else
    {
        bBitCounter = 0;
    }
}

/**
 * @brief  获取红外遥控器Raw Data
 * @return 32位Raw Data，如果没有按键则返回0
 */
uint32_t Remote_Infrared_GetRawData(void)
{
    uint32_t result = 0;
    
    if (FlagGotKey == 1)
    {
        FlagGotKey = 0;
        
        uint32_t raw = RemoteInfrareddata.uiRemoteInfraredData;
        
        // 验证数据完整性
        if((RemoteInfrareddata.RemoteInfraredDataStruct.bID == 
            (uint8_t)~RemoteInfrareddata.RemoteInfraredDataStruct.bIDNot) &&
           (RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCode == 
            (uint8_t)~RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCodeNot))
        {
            // 数据有效
            printf("\r\n=== IR Key Received ===\r\n");
            printf("Raw Data: 0x%08X\r\n", raw);
            printf("  ID:      0x%02X\r\n", RemoteInfrareddata.RemoteInfraredDataStruct.bID);
            printf("  ID_NOT:  0x%02X\r\n", RemoteInfrareddata.RemoteInfraredDataStruct.bIDNot);
            printf("  KeyCode: 0x%02X\r\n", RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCode);
            printf("  Key_NOT: 0x%02X\r\n", RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCodeNot);
            printf("  Status:  VALID\r\n");
            printf("======================\r\n\r\n");
            
            result = raw;
        }
        else
        {
            printf("IR Data INVALID: 0x%08X\r\n", raw);
        }
    }
    
    return result;
}
