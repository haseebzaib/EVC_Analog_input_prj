################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Ext_ADC_Lib/ads8344.c 

OBJS += \
./Ext_ADC_Lib/ads8344.o 

C_DEPS += \
./Ext_ADC_Lib/ads8344.d 


# Each subdirectory must supply rules for building sources it contributes
Ext_ADC_Lib/%.o Ext_ADC_Lib/%.su Ext_ADC_Lib/%.cyclo: ../Ext_ADC_Lib/%.c Ext_ADC_Lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DSTM32C031xx -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I"F:/haseeb/EVC_Analog_input/EVC_Analog_input_prj/Ext_ADC_Lib" -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Ext_ADC_Lib

clean-Ext_ADC_Lib:
	-$(RM) ./Ext_ADC_Lib/ads8344.cyclo ./Ext_ADC_Lib/ads8344.d ./Ext_ADC_Lib/ads8344.o ./Ext_ADC_Lib/ads8344.su

.PHONY: clean-Ext_ADC_Lib

