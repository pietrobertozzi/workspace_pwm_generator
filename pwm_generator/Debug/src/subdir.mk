################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/brd_pwm_generator.c \
../src/symbols.c 

OBJS += \
./src/brd_pwm_generator.o \
./src/symbols.o 

C_DEPS += \
./src/brd_pwm_generator.d \
./src/symbols.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o src/%.su src/%.cyclo: ../src/%.c src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L412xx -DBOARD_PWM_GENERATOR_1_0 -DTARGET_ARCH_ARM -DNO_MARIA_INFO -DLINK_MARIA -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/houston/src/stm32/workspace_pwm_generator/pwm_generator/src/common_drivers" -I"C:/houston/src/stm32/workspace_pwm_generator/pwm_generator/src/maria_monitor/src" -I"C:/houston/src/stm32/workspace_pwm_generator/pwm_generator/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src

clean-src:
	-$(RM) ./src/brd_pwm_generator.cyclo ./src/brd_pwm_generator.d ./src/brd_pwm_generator.o ./src/brd_pwm_generator.su ./src/symbols.cyclo ./src/symbols.d ./src/symbols.o ./src/symbols.su

.PHONY: clean-src

