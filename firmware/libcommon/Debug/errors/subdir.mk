################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../errors/errors.c 

OBJS += \
./errors/errors.o 

C_DEPS += \
./errors/errors.d 


# Each subdirectory must supply rules for building sources it contributes
errors/errors.o: /home/tamas/general/kacsalab/firmware/libcommon/errors/errors.c errors/subdir.mk
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"errors/errors.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

