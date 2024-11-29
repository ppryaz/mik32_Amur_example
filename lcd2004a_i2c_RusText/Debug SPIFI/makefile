################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include runtime/subdir.mk
-include HAL/peripherals/Source/subdir.mk
-include HAL/core/Source/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := lcd2004a_i2c_RusText
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
lcd2004a_i2c_RusText.hex \

SECONDARY_SIZE += \
lcd2004a_i2c_RusText.siz \


# All Target
all: main-build

# Main-build Target
main-build: lcd2004a_i2c_RusText.elf secondary-outputs

# Tool invocations
lcd2004a_i2c_RusText.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -T "C:\MikronIDE\workspace\lcd2004a_i2c_RusText\scripts\spifi.ld" -nostartfiles -nostdlib -Xlinker --gc-sections -Wl,-Map,"lcd2004a_i2c_RusText.map" -o "lcd2004a_i2c_RusText.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lcd2004a_i2c_RusText.hex: lcd2004a_i2c_RusText.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	riscv64-unknown-elf-objcopy -O ihex "lcd2004a_i2c_RusText.elf"  "lcd2004a_i2c_RusText.hex"
	@echo 'Finished building: $@'
	@echo ' '

lcd2004a_i2c_RusText.siz: lcd2004a_i2c_RusText.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv64-unknown-elf-size --format=berkeley "lcd2004a_i2c_RusText.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) lcd2004a_i2c_RusText.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
