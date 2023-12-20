/******************************************************
 * @file 24AA02E48.c
 * @date 2019/10/17
 * @author ChangYoub Lim (mystyle1057@gmail.com)
 * @version 1.0
 * @brief 2K I2C Serial EEPROMs 
 ******************************************************/
#include <stdio.h>
#include <string.h>
#include "24AA02E48.h"

extern I2C_HandleTypeDef hi2c2;

/*
 * @brief  24AA02E48 Internal MAC Address Get
 * @param  MAC Buffer pointer
 * @return ok = 0, error : -1
 */
int32_t Get_MacAddress(uint8_t *mac)
{
    uint8_t wBuf = EUI48_NODE_ADDRES;
    uint8_t rBuf[MAC_ADDR_LEN] = {0,};
    int32_t ret = 0;
    
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c2, DEV_24AA02E48_ADDR, (uint8_t *)&wBuf, sizeof(wBuf), 1000)) {
        ret = -1;
    }
    else {
        if (HAL_OK != HAL_I2C_Master_Receive(&hi2c2, DEV_24AA02E48_ADDR | CTRL_BYTE_RW_BIT, rBuf, MAC_ADDR_LEN, 1000)) {
            ret = -1;
        }
        else {
            memcpy(mac, rBuf, MAC_ADDR_LEN);
        }
    }
    
    return ret;
}

/*
 * @brief  24AA02E48 EEPROM Write Data
 * @param  Start Address, Write Data Buffer, Write Data Length
 * @return ok = 0, error : -1
 */
int32_t Write_Eeprom(uint8_t startAddr, uint8_t *wData, uint8_t len)
{    
    EEPROM_PAGE_WRITE pageWrite;
    int8_t addr;
    uint16_t pages, tSize, rSize;
    int32_t ret = 0;

    if (len <= DEV_24AA02E48_MAX_2KBIT) {    
        tSize = (uint16_t)len;
        pages = tSize / 8;
        rSize = tSize % 8;
        addr = startAddr;  

        for (uint32_t i = 0; i < pages; i++) {
            pageWrite.wordAddr = addr;
            memcpy(pageWrite.data, wData, sizeof(pageWrite.data));
            if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c2, DEV_24AA02E48_ADDR, (uint8_t *)&pageWrite, sizeof(pageWrite), 1000))
            {
                ret = -1;
                break;        
            }

            HAL_Delay(6);
            wData += DEV_242AA02E48_PAGE_SIZE;
            addr  += DEV_242AA02E48_PAGE_SIZE; 
        }

        if ((ret == 0) && (rSize != 0)) {
            pageWrite.wordAddr = addr;
            memcpy(pageWrite.data, wData, rSize);
            if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c2, DEV_24AA02E48_ADDR, (uint8_t *)&pageWrite, 
                                                  rSize + DEV_242AA02E48_WORD_ADDR_SIZE, 1000)) {
                ret = -1;
            }
            HAL_Delay(6);
        }
    }
    else {
        printf("24AA02E48 EEPROM Write Overflow error.\r\n");
    }
    
    return ret;
}

/*
 * @brief  24AA02E48 EEPROM Read Data
 * @param  Start Address, Read Data Buffer, Read Data Length
 * @return ok = 0, error : -1
 */
int32_t Read_Eeprom(uint8_t startAddr, uint8_t *rData, uint8_t len)
{
    int32_t ret = 0;

    if (len <= DEV_24AA02E48_MAX_2KBIT) {     
        if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c2, DEV_24AA02E48_ADDR, (uint8_t *)&startAddr, sizeof(startAddr), 1000)) {
            ret = -1;
        }
        else {
            if (HAL_OK != HAL_I2C_Master_Receive(&hi2c2, DEV_24AA02E48_ADDR | CTRL_BYTE_RW_BIT, rData, len, 1000)) {
                ret = -1;

            }
        }
    }
    else {
        printf("24AA02E48 EEPROM Write Overflow error.\r\n");
    }
    
    return ret;
}

