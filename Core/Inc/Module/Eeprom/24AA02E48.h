#ifndef __24AA02E48_H__
#define __24AA02E48_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define DEV_24AA02E48_ADDR              0xA0    /* Control Byte 1,0,1,0,A2*,A1*,A0*,R/W */
#define EUI48_NODE_ADDRES               0xFA    

#define DEV_24AA02E48_MAX_2KBIT         250     /* 2Kbit */

#define CTRL_BYTE_RW_BIT                0x01U
#define A0_CHIP_SEL_BIT                 0x02U
#define A1_CHIP_SEL_BIT                 0x04U
#define A2_CHIP_SEL_BIT                 0x08U

#define DEV_242AA02E48_WORD_ADDR_SIZE   1
#define DEV_242AA02E48_PAGE_SIZE        8       /* one page size = 8-byte, max page = 31page */

#define MAC_ADDR_LEN                    6

#pragma pack(1)
typedef struct _EEPROM_PAGE_WRITE {
    unsigned char wordAddr;
    unsigned char data[DEV_242AA02E48_PAGE_SIZE];
}EEPROM_PAGE_WRITE;
#pragma pack(4)

int32_t Get_MacAddress(uint8_t *mac);
int32_t Write_Eeprom(uint8_t startAddr, uint8_t *wData, uint8_t len);
int32_t Read_Eeprom(uint8_t startAddr, uint8_t *rData, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif /* __24AA02E48_H_ */


