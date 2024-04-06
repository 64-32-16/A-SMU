#ifndef DAC856XCLASS_h
#define DAC856XCLASS_h

#include <Arduino.h>
#include "SPI.h"


#define CMD_SETA_UPDATEA          0x18  // Update Channel A
#define CMD_SETB_UPDATEB          0x19  // Update Channel B
#define CMD_UPDATE_ALL_DACS       0x0F  // 更新两路寄存器命令，后16位只需时钟即可

#define CMD_GAIN                  0x02    // GAIN
#define DATA_GAIN_B2_A2           0x0000  // B路2倍，A路1倍
#define DATA_GAIN_B2_A1           0x0001  // B路1倍，A路2倍
#define DATA_GAIN_B1_A2           0x0002  // B路2倍，A路2倍
#define DATA_GAIN_B1_A1           0x0003  // GAIN Channel A=1 and Cannel B=1        

#define CMD_PWR_UP_A_B            0x20  // 命令：上电A、B路
#define DATA_PWR_UP_A_B           0x0003  // Power up DAC-A and DAC-B  data

#define CMD_RESET_ALL_REG         0x28  // 命令：所有寄存器复位、清空寄存器
#define DATA_RESET_ALL_REG        0x0001  // 数据：所有寄存器复位、清空寄存器

#define CMD_LDAC_DIS              0x30  // LDAC脚功能命令
#define DATA_LDAC_DIS             0x0003  // LDAC脚不起作用

#define CMD_INTERNAL_REF_DIS      0x38  // 命令：Disable internal reference and reset DACs to gain = 1
#define DATA_INTERNAL_REF_DIS     0x0000  // 数据：Disable internal reference and reset DACs to gain = 1
#define CMD_INTERNAL_REF_EN       0x38  // 命令：Enable Internal Reference & reset DACs to gain = 2
#define DATA_INTERNAL_REF_EN      0x0001  // 数据：Enable Internal Reference & reset DACs to gain = 2


class DAC856XClass
{
    public:
        
        DAC856XClass( SPIClass *spi);

        void Begin(int cs_pin);
        void WriteValue(uint8_t cmd_byte, uint16_t input);

    protected:
        int CSPin;
        SPIClass *SPI1;

        void Initialize(void);
        void DAC_WR_REG(uint8_t cmd_byte, uint16_t data_byte);

};


#endif