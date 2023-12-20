#include "main.h"
#include "bsp.h"

const GPIO_PinState SWITCH_GetStatus(void)
{
    return HAL_GPIO_ReadPin(SW_IN1_GPIO_Port, SW_IN1_Pin);
}