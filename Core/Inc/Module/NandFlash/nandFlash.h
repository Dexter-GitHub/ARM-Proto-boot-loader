#ifndef __NANDFLASH_H
#define __NANDFLASH_H

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "./User_diskio.h"

#define NAND_FLASH_START_ADDR       (uint32_t)0x70000000
#define DATA_AREA                   (uint32_t)0x00000000

#define NAND_CMD_AREA               (1U << 16U) 
#define NAND_ADDR_AREA              (1U << 17U) 

/* FSMC NAND memory command */
#define	NAND_CMD_READ_1             ((uint8_t)0x00)
#define	NAND_CMD_READ_TRUE          ((uint8_t)0x30)

#define	NAND_CMD_RDCOPYBACK         ((uint8_t)0x00)
#define	NAND_CMD_RDCOPYBACK_TRUE    ((uint8_t)0x35)

#define NAND_CMD_PAGEPROGRAM        ((uint8_t)0x80)
#define NAND_CMD_PAGEPROGRAM_TRUE   ((uint8_t)0x10)

#define NAND_CMD_COPYBACKPGM        ((uint8_t)0x85)
#define NAND_CMD_COPYBACKPGM_TRUE   ((uint8_t)0x10)
	
#define NAND_CMD_ERASE0             ((uint8_t)0x60)
#define NAND_CMD_ERASE1             ((uint8_t)0xD0)  

#define NAND_CMD_READID             ((uint8_t)0x90)	
#define NAND_CMD_STATUS             ((uint8_t)0x70)
#define NAND_CMD_RESET              ((uint8_t)0xFF)

#define NAND_CMD_CACHEPGM           ((uint8_t)0x80)
#define NAND_CMD_CACHEPGM_TRUE      ((uint8_t)0x15)

#define NAND_CMD_RANDOMIN           ((uint8_t)0x85)
#define NAND_CMD_RANDOMOUT          ((uint8_t)0x05)
#define NAND_CMD_RANDOMOUT_TRUE     ((uint8_t)0xE0)

#define NAND_CMD_CACHERD_START      ((uint8_t)0x00)
#define NAND_CMD_CACHERD_START2     ((uint8_t)0x31)
#define NAND_CMD_CACHERD_EXIT       ((uint8_t)0x34)

#define NAND_PAGE_SIZE              ((uint16_t)0x0800)//((uint16_t)0x0800)
#define NAND_BLOCK_SIZE             ((uint16_t)0x0040)//((uint16_t)0x0400)
#define NAND_ZONE_SIZE              ((uint16_t)0x0400)
#define NAND_MAX_ZONE               ((uint16_t)0x0001) /* 1 zones of 1024 block */

#define ADDR_1st_CYCLE(ADDR)        (uint8_t)((ADDR)&0xFF)
#define ADDR_2nd_CYCLE(ADDR)        (uint8_t)(((ADDR)&0xFF00) >> 8)
#define ADDR_3rd_CYCLE(ADDR)        (uint8_t)(((ADDR)&0xFF0000) >> 16)
#define ADDR_4th_CYCLE(ADDR)        (uint8_t)(((ADDR)&0xFF000000) >> 24)

#define NAND_FLASH_OK               1

#define MANUFACTURER_CODE           0x01U
#define DEVICE_IDENTIFIER           0xF1U
#define INTERNAL_CHIP_NUM           0x80U
#define PAGE_BLOCK_SPARE_SIZE       0x1DU

typedef struct
{
    uint16_t zone;
    uint16_t block;
    uint16_t page;
}NAND_ADDRESS;

void FSMC_NAND_ReadId(NAND_IDTypeDef *nandId);
uint8_t FSMC_NAND_ChkDevice(void);
void FSMC_NAND_Test(void);
void FSMC_NAND_AllErase(void);

extern Diskio_drvTypeDef NandFlash_Driver;

#ifdef __cplusplus
    }
#endif /* extern "C" */
#endif /* __NANDFLASH_H */


