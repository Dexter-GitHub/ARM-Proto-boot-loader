#include <stdio.h>
#include "flash_if.h"

void FLASH_If_Init(void)
{
    /* Unlock the Program memory */
    HAL_FLASH_Unlock();

    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

    /* Unlock the Program memory */
    HAL_FLASH_Lock();
}

uint32_t FLASH_If_Erase(uint32_t start)
{    
    uint32_t nBrOfPages = 0;
    uint32_t pageError = 0;
    FLASH_EraseInitTypeDef pEraseInit;
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t ret = FLASH_IF_OK;

    HAL_FLASH_Unlock();    
    nBrOfPages = (FLASH_USER_END_ADDR - start) / FLASH_PAGE_SIZE;   /* FLASH_PAGE_SIZE = 0x800 */

    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    pEraseInit.PageAddress = start;
    pEraseInit.Banks = FLASH_BANK_1;
    pEraseInit.NbPages = nBrOfPages;        
    status = HAL_FLASHEx_Erase(&pEraseInit, &pageError);
    
    HAL_FLASH_Lock();

    if (status != HAL_OK) {
        ret = FLASH_IF_ERASEKO;
    }

    return ret;
}

uint32_t FLASH_If_Write(uint32_t destination, uint32_t *pSource, uint32_t length)
{
    uint32_t ret = FLASH_IF_OK;

    HAL_FLASH_Unlock();

    for (uint32_t i = 0; (i < length) && (destination <= (FLASH_USER_END_ADDR-4)); i++) {
        if (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, destination, *(uint32_t *)(pSource + i))) {
            if (*(uint32_t *)destination != *(uint32_t *)(pSource + i)) {
                ret = FLASH_IF_WRITINGCTRL_ERROR;
            }
            /* Increment FLASH destination address */
            destination += 4;
        }
        else {
            ret = FLASH_IF_WRITING_ERROR;
        }

    }

    HAL_FLASH_Lock();

    return ret;
}
