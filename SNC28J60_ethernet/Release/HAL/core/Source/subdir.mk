################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/core/Source/mik32_hal_scr1_timer.c 

OBJS += \
./HAL/core/Source/mik32_hal_scr1_timer.o 

C_DEPS += \
./HAL/core/Source/mik32_hal_scr1_timer.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/core/Source/%.o: ../HAL/core/Source/%.c HAL/core/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -O2 -ffunction-sections -DMIK32V2 -I"C:\MikronIDE\workspace\W5500_ethernet\src\core" -I"C:\MikronIDE\workspace\W5500_ethernet\src\periphery" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\core\Include" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


