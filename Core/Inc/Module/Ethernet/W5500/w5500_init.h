#ifndef __W5500_INIT_H
#define __W5500_INIT_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "wizchip_conf.h"

#define W5500_IP_ADDRESS    "192.168.0.100"
#define W5500_SUB_NET_MASK  "255.255.255.0"
#define W5500_GATE_WAY      "192.168.0.1"

void W5500_Init(void);
void W5500_Select(void);
void W5500_DeSelect(void);
uint8_t W5500_ReadByte(void);
void W5500_WriteByte(uint8_t);
void W5500_DisplayNetConf(void);
void W5500_GetNetConf(wiz_NetInfo *);
void W5500_NetConf(int8_t *, char *, char *, char *);

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* __W5500_INIT_H */

