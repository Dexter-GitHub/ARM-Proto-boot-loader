#include <stdio.h>
#include <string.h>
#include "./Module/NandFlash/nandFlash.h"
#include "fatfs.h"
#include "ff.h"

DSTATUS NandFlash_initialize(BYTE);
DSTATUS NandFlash_status(BYTE);
DRESULT NandFlash_read(BYTE, BYTE *, DWORD, UINT);
DRESULT NandFlash_write(BYTE, const BYTE *, DWORD, UINT);
DRESULT NandFlash_ioctl(BYTE, BYTE, void *);

Diskio_drvTypeDef NandFlash_Driver = 
{
  NandFlash_initialize,
  NandFlash_status,
  NandFlash_read, 
#if  _USE_WRITE
  NandFlash_write,
#endif  /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  NandFlash_ioctl,
#endif /* _USE_IOCTL == 1 */
};

static volatile DSTATUS status = STA_NOINIT;

void FSMC_NAND_ReadId(NAND_IDTypeDef *nandId)
{
    uint32_t data = 0;
   
    (*(volatile uint8_t *)(NAND_FLASH_START_ADDR | NAND_CMD_AREA)) = NAND_CMD_READID;
    (*(volatile uint8_t *)(NAND_FLASH_START_ADDR | NAND_ADDR_AREA)) = 0x00;   
    
    data = (*(volatile uint32_t *)(NAND_FLASH_START_ADDR|DATA_AREA));

    nandId->Maker_Id  = ADDR_1ST_CYCLE(data);
    nandId->Device_Id = ADDR_2ND_CYCLE(data);
    nandId->Third_Id  = ADDR_3RD_CYCLE(data);
    nandId->Fourth_Id = ADDR_4TH_CYCLE(data);   
}

uint8_t FSMC_NAND_ChkDevice(void)
{
    NAND_IDTypeDef nandId;
    uint8_t retVal = 0;

    HAL_NAND_Read_ID(&hnand1, &nandId);

    if ((nandId.Maker_Id == 0x01) && (nandId.Device_Id == 0xDC) &&
        (nandId.Third_Id == 0x90) && (nandId.Fourth_Id == 0x95)) {
        printf("NAND Flash Device: S34ML04G2\r\n");
        retVal = NAND_FLASH_OK;
    }
    else if ((nandId.Maker_Id == 0x01) && (nandId.Device_Id == 0xF1) &&
             (nandId.Third_Id == 0x80) && (nandId.Fourth_Id == 0x1D)) {
        printf("NAND Flash Device: S34ML01G2\r\n");
        retVal = NAND_FLASH_OK;
    }
    else if ((nandId.Maker_Id == 0xEC) && (nandId.Device_Id == 0xF1) &&
             (nandId.Third_Id == 0x80) && (nandId.Fourth_Id == 0x15)) {
        printf("Device: K9F1G08U0A\r\n");
    }
    else if ((nandId.Maker_Id == 0xEC) && (nandId.Device_Id == 0xF1) &&
             (nandId.Third_Id == 0x00) && (nandId.Fourth_Id == 0x95)) {
        printf("Device: K9F1G08U0D\r\n");
        retVal = NAND_FLASH_OK;
    }
    else if ((nandId.Maker_Id == 0xAD) && (nandId.Device_Id == 0xF1) &&
             (nandId.Third_Id == 0x80) && (nandId.Fourth_Id == 0x1D)) {
        printf("Device: HY27UF081G2A\r\n");
    }
    else {
        printf("Device: Unknow\r\n");
    }

    return retVal;
}

#define ROW_ADDRESS (Address.Page + (Address.Block + (Address.Plane * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)
uint32_t FSMC_NAND_AddressIncrement(NAND_AddressTypeDef* Address)
{
    uint32_t status = NAND_VALID_ADDRESS;

    Address->Page++;

    if (Address->Page == NAND_BLOCK_SIZE) {
        Address->Page = 0;
        Address->Block++;

        if (Address->Block == NAND_ZONE_SIZE) {
            Address->Block = 0;
            Address->Plane++;

            if (Address->Plane == NAND_MAX_ZONE)
              status = NAND_INVALID_ADDRESS;
        }
    } 

    return (status);
}

uint32_t FSMC_NAND_ReadStatus(void)
{
    uint32_t data = 0x00, status = NAND_BUSY;

    *(volatile uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_STATUS;
    data = *(volatile uint8_t *)(NAND_FLASH_START_ADDR);

    if ((data & NAND_ERROR) == NAND_ERROR)
        status = NAND_ERROR;
    else if ((data & NAND_READY) == NAND_READY)
        status = NAND_READY;
    else
        status = NAND_BUSY; 
  
    return (status);
}

uint32_t FSMC_NAND_GetStatus(void)
{
    uint32_t timeout = 0x1000000, status = NAND_READY;

    status = FSMC_NAND_ReadStatus(); 

    /* Wait for a NAND operation to complete or a TIMEOUT to occur */
    while ((status != NAND_READY) &&( timeout != 0x00)) {
        status = FSMC_NAND_ReadStatus();
        timeout --;      
    }

    if (timeout == 0x00) {          
    status =  NAND_TIMEOUT_ERROR;      
    } 

    /* Return the operation status */
    return (status);      
}

uint32_t FSMC_NAND_WriteSmallPage(NAND_AddressTypeDef Address, uint8_t *pBuffer, uint32_t NumPageToWrite)
{
    uint32_t numpagewritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
    uint32_t status = NAND_READY, size = 0x00;

    while ((NumPageToWrite != 0x00) && 
           (addressstatus == NAND_VALID_ADDRESS) && 
           (status == NAND_READY)) {
        /* Page write command and address */
        *(volatile uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;
        *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;  
        *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0X00;  
        *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);  
        *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);   

        /* Calculate the size */
        size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpagewritten);

        /* Write data */
        for (uint32_t index = 0; index < size; index++)
            *(volatile uint8_t *)(NAND_FLASH_START_ADDR | DATA_AREA) = pBuffer[index];

        *(volatile uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;

        /* Check status for successful operation */
        status = FSMC_NAND_GetStatus();

        if (status == NAND_READY) {
          numpagewritten++;

          NumPageToWrite--;

          /* Calculate Next small page Address */
          addressstatus = FSMC_NAND_AddressIncrement(&Address);    
        }    
    }

    return (status | addressstatus);
}

uint32_t FSMC_NAND_ReadSmallPage(NAND_AddressTypeDef Address, uint8_t *pBuffer, uint32_t NumPageToRead)
{
  uint32_t numpageread = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00;

  while ((NumPageToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS)) {	   
    /* Page Read command and page address */
    *(volatile uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_1;    
    *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00; 
    *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0X00; 
    *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);  
    *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);     
    *(volatile uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE;   
    
    /* Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpageread);
    
    /* Get Data into Buffer */    
    
    for (uint32_t index = 0; index < size; index++)
        pBuffer[index] = *(volatile uint8_t *)(NAND_FLASH_START_ADDR | DATA_AREA);

    numpageread++;
    
    NumPageToRead--;

    /* Calculate page address */           			 
    addressstatus = FSMC_NAND_AddressIncrement(&Address);
  }

  status = FSMC_NAND_GetStatus();

  return (status | addressstatus);
}

uint32_t FSMC_NAND_EraseBlock(NAND_AddressTypeDef Address)
{
  *(volatile uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_ERASE0;
  *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);  
  *(volatile uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS); 		
  *(volatile uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_ERASE1;   
  
  return (FSMC_NAND_GetStatus());
}

void FSMC_NAND_AllErase(void)
{
    NAND_AddressTypeDef WriteReadAddr;
    WriteReadAddr.Plane = 0x00;
    WriteReadAddr.Block = 0x00;
    WriteReadAddr.Page  = 0x00;    

    while (1) {   
        FSMC_NAND_EraseBlock(WriteReadAddr);
        WriteReadAddr.Block++;
        if (WriteReadAddr.Block == NAND_ZONE_SIZE) {
            printf("Format end\r\n");
            break;
        }        
    }
}

static uint8_t txBuffer [NAND_PAGE_SIZE];
static uint8_t rxBuffer [NAND_PAGE_SIZE];
#if 0
void FSMC_NAND_Test(void)
{
    NAND_AddressTypeDef WriteReadAddr;

    /* NAND memory address to write to */
    WriteReadAddr.Plane = 0x00;
    WriteReadAddr.Block = 0x00;
    WriteReadAddr.Page = 0x00;

    /* Erase the NAND first Block */
    FSMC_NAND_EraseBlock(WriteReadAddr);

    /* Write data to FSMC NOR memory */
    /* Fill the buffer to send */
    for(uint16_t index = 0; index < NAND_PAGE_SIZE; index++ )
        txBuffer[index] = index;
	
    FSMC_NAND_WriteSmallPage(WriteReadAddr, txBuffer, 1);
    
    printf("\r\nWritten to the number of: \r\n");
    for(uint16_t i = 0; i < 100; i++)
        printf("%x\r\n", txBuffer[i]);

    /* Read back the written data */
    FSMC_NAND_ReadSmallPage(WriteReadAddr, rxBuffer, 1);
    
    printf("\r\nRead several: \r\n");
    for(uint16_t i = 0; i < 100; i++)
        printf("%x\r\n", rxBuffer[i]);  
}

#else
void FSMC_NAND_Test(void)
{    
    NAND_AddressTypeDef writeReadAddr;
    FSMC_NAND_ChkDevice();
	
    writeReadAddr.Plane = 0x00;
    writeReadAddr.Block = 0x00;
    writeReadAddr.Page = 0x00;
	
    HAL_NAND_Erase_Block(&hnand1, &writeReadAddr);
	
    for (int index = 0; index < NAND_PAGE_SIZE; index++) {
        txBuffer[index] = index;
    }    
    HAL_NAND_Write_Page_8b(&hnand1, &writeReadAddr, txBuffer, 1);	
    HAL_NAND_Read_Page_8b(&hnand1, &writeReadAddr,  rxBuffer, 1);
	
    for (int index = 0; index < 10; index++) {
        printf("%d.", index);
        printf("0x%x\r\n", rxBuffer[index]);
    }    
}
#endif

DSTATUS NandFlash_initialize(BYTE lun)
{
    status = STA_NOINIT;
    
    if(FSMC_NAND_ChkDevice() == NAND_FLASH_OK)
        status &= ~STA_NOINIT;
 
    return status;
}

DSTATUS NandFlash_status(BYTE lun)
{
    return status;
}
#if 0
DRESULT NandFlash_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    NAND_AddressTypeDef writeReadAddr;
    
    writeReadAddr.Page = sector - ((sector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
    writeReadAddr.Plane = 0;    
    writeReadAddr.Block = sector/NAND_BLOCK_SIZE;  

    HAL_NAND_Read_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)buff, 1);

    return RES_OK;
}
#else
DRESULT NandFlash_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    NAND_AddressTypeDef writeReadAddr;

    if (sector < (NAND_ZONE_SIZE*NAND_BLOCK_SIZE)) {
        writeReadAddr.Page = sector - ((sector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
        writeReadAddr.Plane = 0;    
        writeReadAddr.Block = sector/NAND_BLOCK_SIZE;  
    }
    else {
        writeReadAddr.Page = sector - ((sector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
        writeReadAddr.Plane = 1;    
        writeReadAddr.Block = (sector/NAND_BLOCK_SIZE)/2;
    }
//	    printf("READ :: sector = %d, count = %d, page = %d, block = %d, Plane = %d, 0x%x(%c)\r\n", 
//	            sector, count, writeReadAddr.Page, writeReadAddr.Block, writeReadAddr.Plane, ((WORD *)buff)[0], ((WORD *)buff)[0]);

    HAL_NAND_Read_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)buff, 1);
   
    return RES_OK;
}
#endif

#if 1
static uint32_t formatSts = 0;
static uint8_t backupBuffer[NAND_PAGE_SIZE];
DRESULT NandFlash_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    NAND_AddressTypeDef writeReadAddr;   
    NAND_AddressTypeDef backupAddr;    
    uint16_t newPage = 0;
    uint16_t nFromSector = 0;
    uint16_t nToSector = 0;

    backupAddr.Page = 0;
    backupAddr.Plane = 0;    
    backupAddr.Block = 1023;
    
    nFromSector = sector;
    nToSector = sector + (count - 1);
    if (sector == 0)
        formatSts++;
    else if (formatSts == 963) //  67
        formatSts = 0;
           
    for (;nFromSector <= nToSector; nFromSector++) {
        newPage = nFromSector - ((nFromSector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
        writeReadAddr.Plane = 0;    
        writeReadAddr.Block = nFromSector/NAND_BLOCK_SIZE;

//	        printf("WRITE :: sector = %d, count = %d, page = %d, block = %d\r\n", 
//		            nFromSector, count, newPage, writeReadAddr.Block);

        if (formatSts > 0) {       
            printf("fomatSts = %ld\r\n", formatSts);
            formatSts++;
            writeReadAddr.Page = newPage;
	        HAL_NAND_Write_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)buff, 1);                    
        }
        else {
            FSMC_NAND_EraseBlock(backupAddr);
            for (int i = 0; i < NAND_BLOCK_SIZE; i++) {
                backupAddr.Page = i;
                writeReadAddr.Page = i;
                HAL_NAND_Read_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)backupBuffer, 1);
                if(newPage == i)
                    HAL_NAND_Write_Page_8b(&hnand1, &backupAddr, (uint8_t *)buff, 1);                    
                else
                    HAL_NAND_Write_Page_8b(&hnand1, &backupAddr, (uint8_t *)backupBuffer, 1);                 
            }              
         
            FSMC_NAND_EraseBlock(writeReadAddr); 
  
            for (int i = 0; i < NAND_BLOCK_SIZE; i++) {
                backupAddr.Page = i;                
                writeReadAddr.Page = i;
                HAL_NAND_Read_Page_8b(&hnand1,  &backupAddr, (uint8_t *)backupBuffer, 1);
                HAL_NAND_Write_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)backupBuffer, 1);                 
            }
  
        }      

    }
    
    return RES_OK;
}

#else
static uint32_t formatSts = 0;
static uint8_t backupBuffer[NAND_PAGE_SIZE];
DRESULT NandFlash_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    NAND_AddressTypeDef writeReadAddr;   
    NAND_AddressTypeDef backupAddr;    
    uint16_t newPage = 0;
    uint32_t nFromSector = 0;
    uint32_t nToSector = 0;

    backupAddr.Page = 0;
    backupAddr.Plane = 1;        
    
    nFromSector = sector;
    nToSector = sector + (count - 1);
	
    if(sector == 0)
        formatSts++;
    else if(formatSts == 1986)
        formatSts = 0;
           
    for(;nFromSector <= nToSector; nFromSector++)
    {               
        if(sector < (NAND_ZONE_SIZE*NAND_BLOCK_SIZE))
        {
            backupAddr.Block = 2046;
            newPage = nFromSector - ((nFromSector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
            writeReadAddr.Plane = 0;    
            writeReadAddr.Block = sector/NAND_BLOCK_SIZE;
        }
        else
        {
            backupAddr.Block = 2047;
            newPage = nFromSector - ((nFromSector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
            writeReadAddr.Plane = 1;    
            writeReadAddr.Block = (sector/NAND_BLOCK_SIZE)/2;
        }

        printf("WRITE :: sector = %d, count = %d, page = %d, block = %d, Plane = %d, 0x%x(%c)\r\n", 
		            nFromSector, count, newPage, writeReadAddr.Block, writeReadAddr.Plane, ((char *)buff)[0], ((WORD *)buff)[0]);

        if(formatSts > 0)
        {       
//	            printf("fomatSts = %d\r\n", formatSts);
            formatSts++;
            writeReadAddr.Page = newPage;
	        HAL_NAND_Write_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)buff, 1);                    
        }
        else
        {
            HAL_NAND_Erase_Block(&hnand1, &backupAddr);
            for(int i = 0; i < NAND_BLOCK_SIZE; i++)
            {
                backupAddr.Page = i;
                writeReadAddr.Page = i;
                HAL_NAND_Read_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)backupBuffer, 1);
                if(newPage == i)
                {
                    HAL_NAND_Write_Page_8b(&hnand1, &backupAddr, (uint8_t *)buff, 1);  
//	                    printf("0x%x (p:%d, b:%d, P:%d)\r\n",  ((char *)buff)[0], backupAddr.Page, backupAddr.Block, backupAddr.Plane);
                }
                else
                    HAL_NAND_Write_Page_8b(&hnand1, &backupAddr, (uint8_t *)backupBuffer, 1);                 
            }                       

            HAL_NAND_Erase_Block(&hnand1, &writeReadAddr);
            for(int i = 0; i < NAND_BLOCK_SIZE; i++)
            {
                backupAddr.Page = i;                
                writeReadAddr.Page = i;
                HAL_NAND_Read_Page_8b(&hnand1,  &backupAddr, (uint8_t *)backupBuffer, 1);
                HAL_NAND_Write_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)backupBuffer, 1);                 
            }  
        }     
    }
    
    return RES_OK;
}
#endif

DRESULT NandFlash_ioctl(BYTE lun, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;
    NAND_AddressTypeDef writeReadAddr; 
//   NAND_AddressTypeDef backupAddr; 
    uint16_t nFromSector = 0; 
    uint16_t nToSector = 0;
//    uint16_t erasePage = 0;

    switch (cmd) {
        case CTRL_SYNC:
            res = RES_OK;
            break;
	
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = NAND_BLOCK_SIZE * NAND_ZONE_SIZE;
            printf("GET_SECTOR_COUNT\r\n");
            res = RES_OK;
            break;
	
        case GET_SECTOR_SIZE:
            *(DWORD*)buff = NAND_PAGE_SIZE;
            printf("GET_SECTOR_SIZE\r\n");
            res = RES_OK;
            break;
	
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1023;//NAND_BLOCK_SIZE;
            printf("GET_BLOCK_SIZE\r\n");
            res = RES_OK;
            break;
#if 1	
        case CTRL_TRIM:
//	            printf("CTRL_TRIM\r\n");
            nFromSector = ((uint16_t *)buff)[0];
            nToSector = ((uint16_t *)buff)[1];
            for(int i = 0; i < nToSector; i++)
            {
                nFromSector += i;
                writeReadAddr.Page = (nFromSector - (nFromSector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
                writeReadAddr.Plane = 0;    
                writeReadAddr.Block = nFromSector/NAND_BLOCK_SIZE;
                FSMC_NAND_EraseBlock(writeReadAddr); 
            }
//	            printf("%d~%d\r\n", ((DWORD*)buff)[0], ((DWORD*)buff)[1]);            
            res = RES_OK;
            break;
#else
        case CTRL_TRIM:
            printf("CTRL_TRIM\r\n");
            backupAddr.Page = 0;
            backupAddr.Plane = 0;           
            nFromSector = ((uint16_t *)buff)[0];
            nToSector = ((uint32_t *)buff)[1];
            printf("nFromSector = %d, nToSector = %d\r\n", nFromSector, nToSector);
            for(int i = 0; nFromSector <= nToSector; i++)
            {                
                nFromSector += i;
                backupAddr.Block = 1023;
                erasePage = (nFromSector - (nFromSector / NAND_BLOCK_SIZE) * NAND_BLOCK_SIZE);
                writeReadAddr.Plane = 0;    
                writeReadAddr.Block = nFromSector/NAND_BLOCK_SIZE;
//	                printf("Page = %d, Block = %d, Plane = %d\r\n", erasePage, writeReadAddr.Block, writeReadAddr.Plane);    
                
                HAL_NAND_Erase_Block(&hnand1, &backupAddr);
                for(int i = 0; i < NAND_BLOCK_SIZE; i++)
                { 
                    writeReadAddr.Page = i;
                    backupAddr.Page = i;
                    
                    HAL_NAND_Read_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)backupBuffer, 1);
                    if(writeReadAddr.Page != erasePage)
                        HAL_NAND_Write_Page_8b(&hnand1, &backupAddr, (uint8_t *)backupBuffer, 1); 
                }
                
                HAL_NAND_Erase_Block(&hnand1, &writeReadAddr);
                for(int i = 0; i < NAND_BLOCK_SIZE; i++)
                {
                    backupAddr.Page = i;                
                    writeReadAddr.Page = i;
                    HAL_NAND_Read_Page_8b(&hnand1,  &backupAddr, (uint8_t *)backupBuffer, 1);
                    HAL_NAND_Write_Page_8b(&hnand1, &writeReadAddr, (uint8_t *)backupBuffer, 1);                 
                }  
            }
            printf("%d~%d\r\n", ((DWORD*)buff)[0], ((DWORD*)buff)[1]);            
            res = RES_OK;
            break;            
#endif            
        default :
            printf("DEFAULT %d\r\n", cmd);
            res = RES_PARERR;
            break;
    }
    
    return res;
} 

#if 0
/* FSMC initialization function */
static void MX_FSMC_Init(void)
{
  FSMC_NAND_PCC_TimingTypeDef ComSpaceTiming;
  FSMC_NAND_PCC_TimingTypeDef AttSpaceTiming;

  /** Perform the NAND1 memory initialization sequence
  */
  hnand1.Instance = FSMC_NAND_DEVICE;
  /* hnand1.Init */
  hnand1.Init.NandBank = FSMC_NAND_BANK2;
  hnand1.Init.Waitfeature = FSMC_NAND_PCC_WAIT_FEATURE_ENABLE;
  hnand1.Init.MemoryDataWidth = FSMC_NAND_PCC_MEM_BUS_WIDTH_8;
  hnand1.Init.EccComputation = FSMC_NAND_ECC_ENABLE;
  hnand1.Init.ECCPageSize = FSMC_NAND_ECC_PAGE_SIZE_512BYTE;
  hnand1.Init.TCLRSetupTime = 0;
  hnand1.Init.TARSetupTime = 0;
  /* hnand1.Config */
  hnand1.Config.PageSize = 0;
  hnand1.Config.SpareAreaSize = 0;
  hnand1.Config.BlockSize = 0;
  hnand1.Config.BlockNbr = 0;
  hnand1.Config.PlaneNbr = 0;
  hnand1.Config.PlaneSize = 0;
  hnand1.Config.ExtraCommandEnable = DISABLE;
  /* ComSpaceTiming */
  ComSpaceTiming.SetupTime = 1;
  ComSpaceTiming.WaitSetupTime = 3;
  ComSpaceTiming.HoldSetupTime = 2;
  ComSpaceTiming.HiZSetupTime = 1;
  /* AttSpaceTiming */
  AttSpaceTiming.SetupTime = 1;
  AttSpaceTiming.WaitSetupTime = 3;
  AttSpaceTiming.HoldSetupTime = 2;
  AttSpaceTiming.HiZSetupTime = 1;

  if (HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /** Disconnect NADV
  */

  __HAL_AFIO_FSMCNADV_DISCONNECTED();

}
#endif
