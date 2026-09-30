################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT.c \
../Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_printf.c \
../Middlewares/Third_Party/SEGGER/segger/SEGGER_SYSVIEW.c 

S_UPPER_SRCS += \
../Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_ASM_ARMv7M.S 

OBJS += \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT.o \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_ASM_ARMv7M.o \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_printf.o \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_SYSVIEW.o 

S_UPPER_DEPS += \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_ASM_ARMv7M.d 

C_DEPS += \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT.d \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_printf.d \
./Middlewares/Third_Party/SEGGER/segger/SEGGER_SYSVIEW.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/SEGGER/segger/%.o Middlewares/Third_Party/SEGGER/segger/%.su Middlewares/Third_Party/SEGGER/segger/%.cyclo: ../Middlewares/Third_Party/SEGGER/segger/%.c Middlewares/Third_Party/SEGGER/segger/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Workspace/RTOS_Workspace/003LED_Block_Tasks/Middlewares/Third_Party/SEGGER/Config" -I"D:/Workspace/RTOS_Workspace/003LED_Block_Tasks/Middlewares/Third_Party/SEGGER/OS" -I"D:/Workspace/RTOS_Workspace/003LED_Block_Tasks/Middlewares/Third_Party/SEGGER/segger" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/SEGGER/segger/%.o: ../Middlewares/Third_Party/SEGGER/segger/%.S Middlewares/Third_Party/SEGGER/segger/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Workspace/RTOS_Workspace/003LED_Block_Tasks/Middlewares/Third_Party/SEGGER/Config" -I"D:/Workspace/RTOS_Workspace/003LED_Block_Tasks/Middlewares/Third_Party/SEGGER/OS" -I"D:/Workspace/RTOS_Workspace/003LED_Block_Tasks/Middlewares/Third_Party/SEGGER/segger" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Middlewares-2f-Third_Party-2f-SEGGER-2f-segger

clean-Middlewares-2f-Third_Party-2f-SEGGER-2f-segger:
	-$(RM) ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT.cyclo ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT.d ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT.o ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT.su ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_ASM_ARMv7M.d ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_ASM_ARMv7M.o ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_printf.cyclo ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_printf.d ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_printf.o ./Middlewares/Third_Party/SEGGER/segger/SEGGER_RTT_printf.su ./Middlewares/Third_Party/SEGGER/segger/SEGGER_SYSVIEW.cyclo ./Middlewares/Third_Party/SEGGER/segger/SEGGER_SYSVIEW.d ./Middlewares/Third_Party/SEGGER/segger/SEGGER_SYSVIEW.o ./Middlewares/Third_Party/SEGGER/segger/SEGGER_SYSVIEW.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-SEGGER-2f-segger

