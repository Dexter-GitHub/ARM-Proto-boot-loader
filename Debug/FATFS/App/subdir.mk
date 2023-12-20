################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FATFS/App/fatfs.c 

OBJS += \
./FATFS/App/fatfs.o 

C_DEPS += \
./FATFS/App/fatfs.d 


# Each subdirectory must supply rules for building sources it contributes
FATFS/App/%.o FATFS/App/%.su: ../FATFS/App/%.c FATFS/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xG -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/App" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/FATFS/App" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module/Eeprom" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module/Ethernet" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module/Ethernet/W5500" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module/Ethernet/PING" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module/Ethernet/FTPClient" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module/Ethernet/FTPServer" -I"D:/workspace/STM32/STM32CubeMX/2. T5 Auto Resister Measure/AutoRMeasure_bootloader/arm_bootloader/Core/Inc/Module/NandFlash" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-FATFS-2f-App

clean-FATFS-2f-App:
	-$(RM) ./FATFS/App/fatfs.d ./FATFS/App/fatfs.o ./FATFS/App/fatfs.su

.PHONY: clean-FATFS-2f-App

