################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include portAvr/subdir.mk
-include lib/subdir.mk
-include kernel/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
NhietDo_doam.lss \

FLASH_IMAGE += \
NhietDo_doam.hex \

EEPROM_IMAGE += \
NhietDo_doam.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: NhietDo_doam.elf secondary-outputs

# Tool invocations
NhietDo_doam.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,NhietDo_doam.map -uvprintf -mmcu=atmega328p -o "NhietDo_doam.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

NhietDo_doam.lss: NhietDo_doam.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S NhietDo_doam.elf  >"NhietDo_doam.lss"
	@echo 'Finished building: $@'
	@echo ' '

NhietDo_doam.hex: NhietDo_doam.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex NhietDo_doam.elf  "NhietDo_doam.hex"
	@echo 'Finished building: $@'
	@echo ' '

NhietDo_doam.eep: NhietDo_doam.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex NhietDo_doam.elf  "NhietDo_doam.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: NhietDo_doam.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p NhietDo_doam.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) NhietDo_doam.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
