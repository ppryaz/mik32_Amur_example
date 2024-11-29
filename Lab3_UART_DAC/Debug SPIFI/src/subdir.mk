################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/syscalls.c 

OBJS += \
./src/main.o \
./src/syscalls.o 

C_DEPS += \
./src/main.d \
./src/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\Lab3_UART_DAC\src\core" -I"C:\MikronIDE\workspace\Lab3_UART_DAC\src\periphery" -I"C:\MikronIDE\workspace\Lab3_UART_DAC\HAL\core\Include" -I"C:\MikronIDE\workspace\Lab3_UART_DAC\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


