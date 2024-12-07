################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Ethernet/W5100S/w5100s.c 

OBJS += \
./src/Ethernet/W5100S/w5100s.o 

C_DEPS += \
./src/Ethernet/W5100S/w5100s.d 


# Each subdirectory must supply rules for building sources it contributes
src/Ethernet/W5100S/%.o: ../src/Ethernet/W5100S/%.c src/Ethernet/W5100S/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\W5500_ethernet\src\core" -I"C:\MikronIDE\workspace\W5500_ethernet\src\periphery" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\core\Include" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


