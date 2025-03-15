################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/maria_monitor/src/maria_info.c \
../src/maria_monitor/src/maria_parser.c \
../src/maria_monitor/src/maria_utils.c 

OBJS += \
./src/maria_monitor/src/maria_info.o \
./src/maria_monitor/src/maria_parser.o \
./src/maria_monitor/src/maria_utils.o 

C_DEPS += \
./src/maria_monitor/src/maria_info.d \
./src/maria_monitor/src/maria_parser.d \
./src/maria_monitor/src/maria_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/maria_monitor/src/%.o src/maria_monitor/src/%.su src/maria_monitor/src/%.cyclo: ../src/maria_monitor/src/%.c src/maria_monitor/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L412xx -DBOARD_PWM_GENERATOR_1_0 -DTARGET_ARCH_ARM -DNO_MARIA_INFO -DLINK_MARIA -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/houston/src/stm32/workspace_pwm_generator/pwm_generator/src/common_drivers" -I"C:/houston/src/stm32/workspace_pwm_generator/pwm_generator/src/maria_monitor/src" -I"C:/houston/src/stm32/workspace_pwm_generator/pwm_generator/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-maria_monitor-2f-src

clean-src-2f-maria_monitor-2f-src:
	-$(RM) ./src/maria_monitor/src/maria_info.cyclo ./src/maria_monitor/src/maria_info.d ./src/maria_monitor/src/maria_info.o ./src/maria_monitor/src/maria_info.su ./src/maria_monitor/src/maria_parser.cyclo ./src/maria_monitor/src/maria_parser.d ./src/maria_monitor/src/maria_parser.o ./src/maria_monitor/src/maria_parser.su ./src/maria_monitor/src/maria_utils.cyclo ./src/maria_monitor/src/maria_utils.d ./src/maria_monitor/src/maria_utils.o ./src/maria_monitor/src/maria_utils.su

.PHONY: clean-src-2f-maria_monitor-2f-src

