################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/ioeasy.c \
../lib/uart.c 

OBJS += \
./lib/ioeasy.o \
./lib/uart.o 

C_DEPS += \
./lib/ioeasy.d \
./lib/uart.d 


# Each subdirectory must supply rules for building sources it contributes
lib/%.o: ../lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"D:\PLam\workspace\NhietDo_doam" -I"D:\PLam\workspace\NhietDo_doam\kernel" -I"D:\PLam\workspace\NhietDo_doam\lib" -I"D:\PLam\workspace\NhietDo_doam\portAvr" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


