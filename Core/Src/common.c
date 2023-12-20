#include <stdio.h>
#include "common.h"
#include "fatfs.h"
#include "flash_if.h"
#include "ftpc.h"

static void BinaryFlashWrite(void);

static uint32_t gJumpAddress;
static pFunction jumpToApplication;
static uint8_t gFtpBuf[_MAX_SS];
static uint8_t gStep = 0;
static FIL gFile;

void JumpToApplication(void)
{
    /* Test if user code is programmed starting from address */ 
    if (((*(__IO uint32_t *)APPLICATION_ADDR) & 0x2FFE0000) == 0x20000000) {
        printf("Jump To Application.\r\n");
        /* Jump to user application */
        gJumpAddress = *(__IO uint32_t *)(APPLICATION_ADDR + 4);
        jumpToApplication = (pFunction)gJumpAddress;
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *)APPLICATION_ADDR);
        jumpToApplication();
    }
}

void FtpDownloadApplication(void)
{
    uint8_t state;
    uint8_t hostIpAddr[4] = {192, 168, 0, 75};

    state = ftpc_proc(gFtpBuf, (uint8_t *)FTP_USER_NAME, (uint8_t *)FTP_PASSWORD, hostIpAddr);
	if (state == SOCK_OK) {
		if (ftpc_status()) {		
			switch (gStep) {
				case 0:				
					ftpc_service(CMD_DIR);
					gStep = 1;
					break;

				case 1:
					ftpc_setFileName((uint8_t *)APPLICATION_FILE_NAME, (uint8_t *)APPLICATION_FILE_NAME);
					ftpc_service(CMD_GET);
					gStep = 2;
					break;

				case 2:
					BinaryFlashWrite();
					gStep = 3;
					break;

				case 3:
					JumpToApplication();
					break;

				default: break;
			}
		}
	}
}

static void BinaryFlashWrite(void)
{
    uint8_t fileName[20] = {0,};
	uint8_t readBuf[1024] = {0,};
    uint32_t fileSize = 0;
	uint32_t bw, totalBw = 0, ramsource;
	uint32_t flashdestination = APPLICATION_ADDR;
    FRESULT fres;
    
    FLASH_If_Init();
    FLASH_If_Erase(APPLICATION_ADDR);

    sprintf((char *)fileName, "0:/%s", APPLICATION_FILE_NAME);
    fres = f_open(&gFile, (char *)fileName, (FA_WRITE | FA_READ | FA_OPEN_ALWAYS));
    if (fres == FR_OK) {
        fileSize = f_size(&gFile);        
		while(1)  {
			fres = f_read(&gFile, readBuf, sizeof(readBuf), (UINT *)&bw);
			if (fres == FR_OK) {
				totalBw += bw;
				ramsource = (uint32_t)readBuf;

				if (FLASH_If_Write(flashdestination, (uint32_t *)ramsource, bw/4) == FLASH_IF_OK) {
					flashdestination += bw;
				}
				else {
					break;
				}

				if (totalBw >= fileSize) {
					break;
				}
			}
			else {
				break;
			}
		}		
		f_unlink((TCHAR *)fileName);
		f_close(&gFile);
    }
}
