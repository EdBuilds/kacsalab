################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bmmcp/bmmcp_common.c 

OBJS += \
./bmmcp/bmmcp_common.o 

C_DEPS += \
./bmmcp/bmmcp_common.d 


# Each subdirectory must supply rules for building sources it contributes
bmmcp/bmmcp_common.o: ../bmmcp/bmmcp_common.c bmmcp/subdir.mk
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"bmmcp/bmmcp_common.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

