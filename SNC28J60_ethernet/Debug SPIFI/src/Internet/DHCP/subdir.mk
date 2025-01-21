################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Internet/DHCP/dhcp.c 

OBJS += \
./src/Internet/DHCP/dhcp.o 

C_DEPS += \
./src/Internet/DHCP/dhcp.d 


# Each subdirectory must supply rules for building sources it contributes
src/Internet/DHCP/%.o: ../src/Internet/DHCP/%.c src/Internet/DHCP/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\SNC28J60_ethernet2\src\core" -I"C:\MikronIDE\workspace\SNC28J60_ethernet2\src\periphery" -I"C:\MikronIDE\workspace\SNC28J60_ethernet2\HAL\core\Include" -I"C:\MikronIDE\workspace\SNC28J60_ethernet2\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


