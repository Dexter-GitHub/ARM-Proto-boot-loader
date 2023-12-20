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

typedef void (*pFunction)(void);

void JumpToApplication(void);
void FtpDownloadApplication(void);

#ifdef __cplusplus    
}
#endif
#endif  /* __COMMON_H__ */
