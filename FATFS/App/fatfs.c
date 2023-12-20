/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */
#include <stdio.h>
#include <string.h>
#include "nandFlash.h"

// PARTITION VolToPart[];

uint32_t FATFS_GetVolume(void);

const char *FresultString[] = {
    "FR_OK",                  /* (0) Succeeded */
    "FR_DISK_ERR",            /* (1) A hard error occurred in the low level disk I/O layer */
    "FR_INT_ERR",             /* (2) Assertion failed */
    "FR_NOT_READY",           /* (3) The physical drive cannot work */
    "FR_NO_FILE",             /* (4) Could not find the file */
    "FR_NO_PATH",             /* (5) Could not find the path */
    "FR_INVALID_NAME",        /* (6) The path name format is invalid */
    "FR_DENIED",              /* (7) Access denied due to prohibited access or directory full */
    "FR_EXIST",               /* (8) Access denied due to prohibited access */
    "FR_INVALID_OBJECT",      /* (9) The file/directory object is invalid */
    "FR_WRITE_PROTECTED",     /* (10) The physical drive is write protected */
    "FR_INVALID_DRIVE",       /* (11) The logical drive number is invalid */
    "FR_NOT_ENABLED",         /* (12) The volume has no work area */
    "FR_NO_FILESYSTEM",       /* (13) There is no valid FAT volume */
    "FR_MKFS_ABORTED",        /* (14) The f_mkfs() aborted due to any parameter error */
    "FR_TIMEOUT",             /* (15) Could not get a grant to access the volume within defined period */
    "FR_LOCKED",              /* (16) The operation is rejected according to the file sharing policy */
    "FR_NOT_ENOUGH_CORE",     /* (17) LFN working buffer could not be allocated */
    "FR_TOO_MANY_OPEN_FILES", /* (18) Number of open files > _FS_SHARE */
    "FR_INVALID_PARAMETER"    /* (19) Given parameter is invalid */
};

FATFS NANDFLASHDISKFatFS;
char NANDFLASHDISKPath[4];
static DIR dir;

void NAND_FlashDiskInit(void);
//	retUSER = FATFS_LinkDriver(&NandFlash_Driver, NANDFLASHDISKPath);
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&NandFlash_Driver, NANDFLASHDISKPath);

  /* USER CODE BEGIN Init */
    /* additional user code for init */
    NAND_FlashDiskInit();
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
    return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
void NAND_FlashDiskInit(void)
{
    FRESULT fres;    
    
    fres = f_mount(&NANDFLASHDISKFatFS, NANDFLASHDISKPath, 0);

    if (fres != FR_OK)
        printf("- NandFlash mount error.\r\n");
    else {        
        fres = f_opendir(&dir, (TCHAR const*)NANDFLASHDISKPath);        
        if (fres == FR_OK) {
            printf("- Nand Flash mount ok. volum is \"%s\".\r\n", NANDFLASHDISKPath);
        }
        else if (fres == FR_NO_FILESYSTEM) {
            FSMC_NAND_AllErase();
            fres = f_mkfs(NANDFLASHDISKPath, 0, 0);
            printf("f_mkfs = %d\r\n", fres);            
        }
        else {
            printf("- f_opendir error = %d.\r\n", fres);
        }
        f_closedir(&dir);
    }
}

uint32_t FATFS_GetVolume(void)
{
    FATFS *pFs;
    DIR dir;
    int8_t *pPath;
#if 0    
    uint64_t totalVol;
    uint64_t freeVol;
#endif

    pFs = (FATFS *)&NANDFLASHDISKFatFS;
    pPath = (int8_t *)NANDFLASHDISKPath;

    f_opendir(&dir, (TCHAR const*)pPath);
    f_closedir(&dir);

#if 0
    printf("- total clusters : %d\r\n", pFs->n_fatent-2);
    printf("- free clusters  : %d\r\n", pFs->free_clust);
    printf("- cluster size   : %d\r\n", pFs->csize);
#if _MAX_SS != _MIN_SS    
    printf("- sector size    : %d\r\n", pFs->ssize);
    totalVol = (uint64_t)pFs->csize * (uint64_t)pFs->ssize * (uint64_t)(pFs->n_fatent-2);
    freeVol = (uint64_t)pFs->csize * (uint64_t)pFs->ssize * (uint64_t)(pFs->free_clust);
#else
    printf("- sector size    : %d\r\n", _MAX_SS);
    totalVol = (uint64_t)pFs->csize * _MAX_SS * (uint64_t)(pFs->n_fatent-2);
	freeVol  = (uint64_t)pFs->csize * (uint64_t)_MAX_SS * (uint64_t)(pFs->free_clust);
#endif 
    printf("- total volume   : %i KBytes\r\n", (uint32_t)(totalVol/1024));
	printf("- free volume    : %i KBytes\r\n", (uint32_t)(freeVol/1024));
#endif

    return pFs->free_clust;
}

void FATFS_ScanFile(void)
{    
    FILINFO fno;
    FRESULT fres = FR_OK;
    uint32_t i = 0;   

    fres = f_opendir(&dir, NANDFLASHDISKPath);
    if (fres == FR_OK) {
        while (1) {            
            fres = f_readdir(&dir, &fno);                   /* Read a directory item */            
            HAL_Delay(100);
			if (fres != FR_OK) {
				printf("f_readdir() error= %s\r\n", FresultString[fres]);
    			break;
			}
			else if (fno.fname[0] == 0 || fno.fname[0] == 0xFF) {
				printf("- [%li] : End of All Files\r\n", i);
				break;
			}
			else if (fno.fname[0] == '.') {
				printf("- [%li] : Skipping \"%s\"\r\n", i, fno.fname);
			}
			else if (fno.fattrib & AM_DIR) {
				printf("- [%li] : Directory Entry: %s\r\n", i, fno.fname);
			}
			else {
				printf("- [%li] : %s(%ld bytes) ", i, fno.fname, fno.fsize);
                printf("date : %02d.%02d.%02d ", (((fno.fdate >> 9) & 0x7f) + 1980), (fno.fdate >> 5) & 0x0f, (fno.fdate & 0x1f));                
                printf("time : %02d.%0d.%02d\r\n", (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F, fno.ftime & 0x1f);
			}
            i++;
        }
    }
    else
		printf("f_opendir() = %s\r\n", FresultString[fres]);

    f_closedir(&dir);    
}

FRESULT FATFS_FileList(const char *path, uint8_t opt)
{
    DIR dir;
    FILINFO fno;
    FRESULT fres = FR_OK;
    uint32_t i = 0; 
    uint32_t totalSize = 0;     
    uint32_t fileCnt = 0;
    uint32_t dirCnt = 0;
    
    fres = f_opendir(&dir, path); 
    if (fres == FR_OK) {
        while(1) {
            fres = f_readdir(&dir, &fno);                   /* Read a directory item */
            HAL_Delay(100);
			if (fres != FR_OK)
    			break;
            else if (fno.fname[0] == 0 || fno.fname[0] == 0xFF) {
                if (opt == 1)
                    printf("Total : %ld files, %ld directorys (%ld bytes)\r\n", fileCnt, dirCnt, totalSize);
                break;
            }
            else if (fno.fattrib & AM_DIR) {
                if (!(fno.fattrib & AM_HID)) {
                    if (opt == 0)
                        printf("%s/ ", fno.fname);
                    else {
                        printf("%c", fno.fattrib & AM_DIR ? 'd' : '-');
                        printf("%c", fno.fattrib & AM_RDO ? 'r' : '-');    
                        printf("%c", fno.fattrib & AM_HID ? 'h' : '-');    
                        printf("%c", fno.fattrib & AM_SYS ? 's' : '-');
                        printf("%c", fno.fattrib & AM_ARC ? 'a' : '-');
                        printf(" %ld ", fno.fsize);
                        printf("Jan %02d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                        printf("%s/\r\n", fno.fname);
                    }
                    dirCnt++;
                }
            }
            else {
                if (opt == 0)
                    printf("%s ", fno.fname);
                else {
                    printf("%c", fno.fattrib & AM_DIR ? 'd' : '-');
                    printf("%c", fno.fattrib & AM_RDO ? 'r' : '-');    
                    printf("%c", fno.fattrib & AM_HID ? 'h' : '-');    
                    printf("%c", fno.fattrib & AM_SYS ? 's' : '-');
                    printf("%c", fno.fattrib & AM_ARC ? 'a' : '-');
                    printf(" %ld ", fno.fsize);
                    totalSize += fno.fsize;
                    switch ((fno.fdate >> 5) & 0x0f) {
                        case 1:
                            printf("Jan %02d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 2:
                            printf("Feb %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 3:
                            printf("Mar %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 4:
                            printf("Apr %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 5:
                            printf("May %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 6:
                            printf("Jun %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 7:
                            printf("Jul %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 8:
                            printf("Aug %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 9:
                            printf("Sept %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 10:
                            printf("Oct %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 11:
                            printf("Nov %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        case 12:
                            printf("Dec %d %02d:%02d ", (fno.fdate & 0x1f), (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F);
                            break;

                        default: break;
                    }
                 
//	                    printf("date : %02d.%02d.%02d ", (((fno.fdate >> 9) & 0x7f) + 1980), (fno.fdate >> 5) & 0x0f, (fno.fdate & 0x1f));                
//	                    printf("time : %02d.%0d.%02d\r\n", (fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x3F, fno.ftime & 0x1f);
                    printf("%s\r\n", fno.fname);
                    fileCnt++;
                }                    
            }
            i++;
        }
    }
    f_closedir(&dir);
    
    return fres;
}

void FATFS_DiskCheck(void)
{
    FATFS *pFs = (FATFS *)&NANDFLASHDISKFatFS;
   
    printf("Total disk space : %ldbyte", (pFs->n_fatent-2) * (_MAX_SS * pFs->csize));
    printf("(%ldGbit)\r\n", (((pFs->n_fatent-2) * (_MAX_SS * pFs->csize))*8)/1000000000);
    printf("Allocation unit  : %dbyte\r\n", (_MAX_SS * pFs->csize));
    printf("Total allocation units on disk : %ld\r\n", pFs->n_fatent-2);
}

FRESULT FATFS_FileOut(const char *fileName, char *buf, uint32_t size, uint32_t *len)
{
    FRESULT fres;
    uint32_t br;
    
    fres = f_open(&USERFile, fileName, (FA_READ | FA_OPEN_EXISTING));
    if (fres == FR_OK) {
        fres = f_read(&USERFile, buf, size, (UINT *)&br);
        *len = br-1;
        f_close(&USERFile); 
    }
    else {
        printf("error : %s\r\n", FresultString[fres]);
        f_close(&USERFile); 
    }
    
    return fres;
}

#if 1
FRESULT scan_files(char* path, char *buf, int * buf_len)
{
	FRESULT res;
	FILINFO fno = {0,};
	DIR dir;
	int len = 0, buf_ptr = 0;
	// int i = 0;
	char *fn; 	/* This function is assuming no_Unicode cfg.*/
	char date_str[15];
	int date_str_ptr = 0;
#ifdef _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif

	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		// i = strlen(path);

		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0)
               break;
            
			if (fno.fname[0] == '.') 
               continue;
#ifdef _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;
#else
			fn = fno.fname;
#endif           
			switch ((fno.fdate >> 5) & 0x0f) {
                case 1:
                    len = sprintf(date_str, "JAN ");
                    break;

                case 2:
                    len = sprintf(date_str, "FEB ");
                    break;

                case 3:
                    len = sprintf(date_str, "MAR ");
                    break;

                case 4:
                    len = sprintf(date_str, "APR ");
                    break;

                case 5:
                    len = sprintf(date_str, "MAY ");
                    break;

                case 6:
                    len = sprintf(date_str, "JUN ");
                    break;

                case 7:
                    len = sprintf(date_str, "JUL ");
                    break;

                case 8:
                    len = sprintf(date_str, "AUG ");
                    break;

                case 9:
                    len = sprintf(date_str, "SEP ");
                    break;
                    
                case 10:
                    len = sprintf(date_str, "OCT ");
                    break;

                case 11:
                    len = sprintf(date_str, "NOV ");
                    break;

                case 12:
                    len = sprintf(date_str, "DEC ");
                    break;

                default: break;
			}

			date_str_ptr += len;
			len = sprintf(date_str + date_str_ptr, "%d ", (fno.fdate & 0x1f));
			date_str_ptr += len;
			len = sprintf(date_str + date_str_ptr, "%d", (((fno.fdate >> 9) & 0x7f) + 1980));
			date_str_ptr = 0;

			if (fno.fattrib & AM_DIR)			
				sprintf(buf + buf_ptr, "d");
            else
				sprintf(buf + buf_ptr, "-");
			buf_ptr++;
			// drwxr-xr-x 1 ftp ftp              0 Apr 07  2014 $RECYCLE.BIN\r\n
			//len = sprintf(buf + buf_ptr, "rwxr-xr-x 1 ftp ftp              %d %s %s\r\n", fno.fsize, date_str, fn);
//				len = sprintf(buf + buf_ptr, "rwxr-xr-x 1 ftp ftp %d %s %s\r\n", fno.fsize, date_str, fn);
   			len = sprintf(buf + buf_ptr, "rwxr-xr-x 1 ftp ftp %ld %s\r\n", fno.fsize, fn);
            
			buf_ptr += len;
            HAL_Delay(100);
		}
		*buf_len = strlen(buf);
//			printf("%s", buf);
//			printf("\r\nbuf_len : %d, sizeof(buf): %d\r\n", *buf_len, sizeof(buf));
		f_closedir(&dir);
	}
	return res;
}

int get_filesize(char* path, char *filename)
{
	FRESULT res;
	FILINFO fno = {0,};
	DIR dir;
	char *fn; 	/* This function is assuming no_Unicode cfg.*/
#ifdef _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif

    printf("path = %s\r\n", path);

	if (*path == 0x00)
		res = f_opendir(&dir, "/");
	else
		res = f_opendir(&dir, path);
	if (res == FR_OK){
        
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) 
               break;
			if (fno.fname[0] == '.') 
               continue;
#ifdef _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;
#else
			fn = fno.fname;
#endif
            printf("fn = %s, filename = %s, %d\r\n", fn, filename, !strcmp(fn, filename));
			if (!strcmp(fn, filename)) {                
				if (fno.fattrib & AM_DIR) {
					printf("\r\n%s/%s is a directory\r\n", path, filename);
					return 0;
				}
				return fno.fsize;
			}            
		}
		//printf("\r\n%s/%s was not found\r\n", path, filename);
		//f_closedir(&dir);
	}
	return -1;
}
#endif
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
