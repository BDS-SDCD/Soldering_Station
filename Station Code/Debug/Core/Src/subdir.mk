################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Button.c \
../Core/Src/Filter.c \
../Core/Src/Flash_Custom.c \
../Core/Src/OLED.c \
../Core/Src/PAC.c \
../Core/Src/PID.c \
../Core/Src/Soldering_Station.c \
../Core/Src/UI_Base_Func.c \
../Core/Src/UI_Face.c \
../Core/Src/UI_Menu.c \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/Button.o \
./Core/Src/Filter.o \
./Core/Src/Flash_Custom.o \
./Core/Src/OLED.o \
./Core/Src/PAC.o \
./Core/Src/PID.o \
./Core/Src/Soldering_Station.o \
./Core/Src/UI_Base_Func.o \
./Core/Src/UI_Face.o \
./Core/Src/UI_Menu.o \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/Button.d \
./Core/Src/Filter.d \
./Core/Src/Flash_Custom.d \
./Core/Src/OLED.d \
./Core/Src/PAC.d \
./Core/Src/PID.d \
./Core/Src/Soldering_Station.d \
./Core/Src/UI_Base_Func.d \
./Core/Src/UI_Face.d \
./Core/Src/UI_Menu.d \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Button.cyclo ./Core/Src/Button.d ./Core/Src/Button.o ./Core/Src/Button.su ./Core/Src/Filter.cyclo ./Core/Src/Filter.d ./Core/Src/Filter.o ./Core/Src/Filter.su ./Core/Src/Flash_Custom.cyclo ./Core/Src/Flash_Custom.d ./Core/Src/Flash_Custom.o ./Core/Src/Flash_Custom.su ./Core/Src/OLED.cyclo ./Core/Src/OLED.d ./Core/Src/OLED.o ./Core/Src/OLED.su ./Core/Src/PAC.cyclo ./Core/Src/PAC.d ./Core/Src/PAC.o ./Core/Src/PAC.su ./Core/Src/PID.cyclo ./Core/Src/PID.d ./Core/Src/PID.o ./Core/Src/PID.su ./Core/Src/Soldering_Station.cyclo ./Core/Src/Soldering_Station.d ./Core/Src/Soldering_Station.o ./Core/Src/Soldering_Station.su ./Core/Src/UI_Base_Func.cyclo ./Core/Src/UI_Base_Func.d ./Core/Src/UI_Base_Func.o ./Core/Src/UI_Base_Func.su ./Core/Src/UI_Face.cyclo ./Core/Src/UI_Face.d ./Core/Src/UI_Face.o ./Core/Src/UI_Face.su ./Core/Src/UI_Menu.cyclo ./Core/Src/UI_Menu.d ./Core/Src/UI_Menu.o ./Core/Src/UI_Menu.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

