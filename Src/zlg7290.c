#include "zlg7290.h"

void I2C_ZLG7290_Read(I2C_HandleTypeDef *I2Cx, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size)
{
    // 使用 HAL 库标准的 Mem_Read
    // DevAddress 通常是 0x70(写) 或 0x71(读)，HAL库只需要传入 7位地址或自动处理读写位
    // 这里为了兼容你之前的代码习惯，直接透传
    HAL_I2C_Mem_Read(I2Cx, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, 100);
}

void I2C_ZLG7290_Write(I2C_HandleTypeDef *I2Cx, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size)
{
    HAL_I2C_Mem_Write(I2Cx, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, 100);
}
