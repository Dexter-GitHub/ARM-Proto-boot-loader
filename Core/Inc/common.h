/*
 * flash_if.h
 *
 *  Created on: 2023. 12. 18.
 *      Author: mysty
 */
#ifndef __COMMON_H__
#define __COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif

#define FTP_USER_NAME           "layer"
#define FTP_PASSWORD            "1234"
#define APPLICATION_FILE_NAME   "AutoRMeasure.bin"
#define HOST_IP_ADDR1           192
#define HOST_IP_ADDR2           168
#define HOST_IP_ADDR3           0
#define HOST_IP_ADDR4           75

typedef void (*pFunction)(void);

void JumpToApplication(void);
void FtpDownloadApplication(void);

#ifdef __cplusplus    
}
#endif
#endif  /* __COMMON_H__ */
