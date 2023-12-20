/******************************************************
 * @file W5500_init.c
 * @date 2019/11/01
 * @author ChangYoub Lim (mystyle1057@gmail.com)
 * @version 1.0
 * @brief Wizenet W5500 User CallBack Function
 ******************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "w5500_init.h"

static void W5500_SetIpAddr(char *);
static void W5500_SetMacAddr(int8_t *);
static void W5500_SetSubMask(char *);
static void W5500_SetGateway(char *);

extern SPI_HandleTypeDef hspi2;
static wiz_NetInfo wizeNetInfo;

/*
 * @brief  W5500 Initialization
 * @param  None
 * @return None
 */
void W5500_Init(void)
{
    uint8_t memSize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
        
    W5500_DeSelect();
    
    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_cs_cbfunc(W5500_Select,    W5500_DeSelect);

    if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memSize) == -1) {
        printf("WIZCHIP Initialized fail.\r\n");
    }  
}

/*
 * @brief  W5500 Net Configuration Setting
 * @param  User Mac Address Pointer
 * @return None
 */
void W5500_NetConf(int8_t *macAddr, char *ip, char *sn, char *gate)
{    
    char ipAddr[20] = {0, };
    char subNetMask[20] = {0, };
    char gateWay[20] = {0, };

    memcpy(ipAddr, ip, sizeof(ipAddr));
    memcpy(subNetMask, sn, sizeof(subNetMask));
    memcpy(gateWay, gate, sizeof(gateWay));

    W5500_SetIpAddr(ipAddr);    
    W5500_SetMacAddr(macAddr);   
    W5500_SetSubMask(subNetMask);
    W5500_SetGateway(gateWay);
    ctlnetwork(CN_SET_NETINFO, &wizeNetInfo);
}

/*
 * @brief  W5500 Net Configuration Display
 * @param  None
 * @return None
 */
void W5500_DisplayNetConf(void)
{    
    wiz_NetInfo netInfo;
    
    ctlnetwork(CN_GET_NETINFO, &netInfo);
    printf("[W5500 IP CONFIG]\r\n");
    printf("IP           : %d.%d.%d.%d\r\n",                   netInfo.ip[0],  netInfo.ip[1],  netInfo.ip[2],  netInfo.ip[3]);    
    printf("MAC          : %02X:%02X:%02X:%02X:%02X:%02X\r\n", netInfo.mac[0], netInfo.mac[1], netInfo.mac[2], netInfo.mac[3], netInfo.mac[4], netInfo.mac[5]);
    printf("Gate way     : %d.%d.%d.%d\r\n",                   netInfo.gw[0],  netInfo.gw[1],  netInfo.gw[2],  netInfo.gw[3]);
    printf("Sub net mask : %d.%d.%d.%d\r\n",                   netInfo.sn[0],  netInfo.sn[1],  netInfo.sn[2],  netInfo.sn[3]);
    printf("DNS          : %d.%d.%d.%d\r\n",                   netInfo.dns[0], netInfo.dns[1], netInfo.dns[2], netInfo.dns[3]);   
}

/*
 * @brief  W5500 Net Configuration Get
 * @param  None
 * @return None
 */
void W5500_GetNetConf(wiz_NetInfo *netInfo)
{
    ctlnetwork(CN_GET_NETINFO, netInfo); 
}

/*
 * @brief  W5500 IP Address Setting
 * @param  IP Address
 * @return None
 */
static void W5500_SetIpAddr(char *addr)
{
    char *addrTemp;
    uint8_t i = 0;  
    
    addrTemp = strtok(addr, ".");
    while (addrTemp != NULL) {
        wizeNetInfo.ip[i] = atoi(addrTemp); 
        addrTemp = strtok(NULL, ".");               
        i++;
    }     
}

/*
 * @brief  W5500 Mac Address Setting
 * @param  Mac Address
 * @return None
 */
static void W5500_SetMacAddr(int8_t *addr)
{    
    for (uint8_t i = 0; i < 6; i++) {
        wizeNetInfo.mac[i] = addr[i]; 
    }
}

/*
 * @brief  W5500 Sub net Mask Setting
 * @param  Sub net Mask
 * @return None
 */
static void W5500_SetSubMask(char *addr)
{
    char *addrTemp;
    uint8_t i = 0;  
    
    addrTemp = strtok(addr, ".");
    while (addrTemp != NULL) {
        wizeNetInfo.sn[i] = atoi(addrTemp); 
        addrTemp = strtok(NULL, ".");               
        i++;
    }  
}

/*
 * @brief  W5500 Gate Way Setting
 * @param  Gate Way
 * @return None
 */
static void W5500_SetGateway(char *addr)
{
    char *addrTemp;
    uint8_t i = 0;  
    
    addrTemp = strtok(addr, ".");
    while (addrTemp != NULL) {
        wizeNetInfo.gw[i] = atoi(addrTemp); 
        addrTemp = strtok(NULL, ".");               
        i++;
    } 
}

/*
 * @brief  W5500 SPI Data Write
 * @param  Data
 * @return None
 */
void W5500_WriteByte(uint8_t byte)
{
    HAL_SPI_Transmit(&hspi2, &byte, sizeof(byte), 100);
    while (HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_RESET);
}

/*
 * @brief  W5500 SPI Data Read
 * @param  None
 * @return Data
 */
uint8_t W5500_ReadByte(void)
{
    uint8_t recvData;
    HAL_SPI_Receive(&hspi2, &recvData, sizeof(recvData), 100);
    while (HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_RESET);
    return recvData;    
}

/*
 * @brief  W5500 SPI Chip Select
 * @param  None
 * @return None
 */
void W5500_Select(void)
{
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
}

/*
 * @brief  W5500 SPI Chip Deselect
 * @param  None
 * @return None
 */
void W5500_DeSelect(void)
{
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
}

