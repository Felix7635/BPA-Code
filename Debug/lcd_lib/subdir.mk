################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/Felix/Documents/CubeMX/HD44780-Stm32HAL/lcd_lib/lcd.c 

OBJS += \
./lcd_lib/lcd.o 

C_DEPS += \
./lcd_lib/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
lcd_lib/lcd.o: /Users/Felix/Documents/CubeMX/HD44780-Stm32HAL/lcd_lib/lcd.c lcd_lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I/Users/Felix/Documents/CubeMX/STM32LiquidCrystal/src -I/Users/Felix/Downloads/STM_MY_LCD16X2 -I/Users/Felix/Documents/CubeMX/HD44780-Stm32HAL/lcd_lib -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"lcd_lib/lcd.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

