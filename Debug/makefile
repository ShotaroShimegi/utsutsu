################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/Mouse/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
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
EXECUTABLES += \
utsutsu.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
utsutsu.list \

OBJCOPY_HEX += \
utsutsu.hex \

OBJCOPY_BIN += \
utsutsu.bin \


# All Target
all: utsutsu.elf secondary-outputs

# Tool invocations
utsutsu.elf: $(OBJS) $(USER_OBJS) C:\Users\sssho\STM32CubeIDE\workspace_1.0.2\utsutsu\STM32F405RGTX_FLASH.ld
	arm-none-eabi-gcc -o "utsutsu.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\sssho\STM32CubeIDE\workspace_1.0.2\utsutsu\STM32F405RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="utsutsu.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

utsutsu.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "utsutsu.list"
	@echo 'Finished building: $@'
	@echo ' '

utsutsu.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "utsutsu.hex"
	@echo 'Finished building: $@'
	@echo ' '

utsutsu.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "utsutsu.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
