################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../portAvr/atomport-asm.s 

C_SRCS += \
../portAvr/atomport-private.c \
../portAvr/atomport.c 

OBJS += \
./portAvr/atomport-asm.o \
./portAvr/atomport-private.o \
./portAvr/atomport.o 

S_DEPS += \
./portAvr/atomport-asm.d 

C_DEPS += \
./portAvr/atomport-private.d \
./portAvr/atomport.d 


# Each subdirectory must supply rules for building sources it contributes
portAvr/%.o: ../portAvr/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -g2 -gstabs -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

portAvr/%.o: ../portAvr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"D:\PLam\workspace\ap_suat_nuoc_dieu_khien_bien_tan\kernel" -I"D:\PLam\workspace\ap_suat_nuoc_dieu_khien_bien_tan" -I"D:\PLam\workspace\ap_suat_nuoc_dieu_khien_bien_tan\lib" -I"D:\PLam\workspace\ap_suat_nuoc_dieu_khien_bien_tan\portAvr" -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


