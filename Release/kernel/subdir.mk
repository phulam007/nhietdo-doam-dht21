################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/atomkernel.c \
../kernel/atommutex.c \
../kernel/atomqueue.c \
../kernel/atomsem.c \
../kernel/atomtimer.c 

OBJS += \
./kernel/atomkernel.o \
./kernel/atommutex.o \
./kernel/atomqueue.o \
./kernel/atomsem.o \
./kernel/atomtimer.o 

C_DEPS += \
./kernel/atomkernel.d \
./kernel/atommutex.d \
./kernel/atomqueue.d \
./kernel/atomsem.d \
./kernel/atomtimer.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/%.o: ../kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"D:\PLam\workspace\NhietDo_doam" -I"D:\PLam\workspace\NhietDo_doam\kernel" -I"D:\PLam\workspace\NhietDo_doam\lib" -I"D:\PLam\workspace\NhietDo_doam\portAvr" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


