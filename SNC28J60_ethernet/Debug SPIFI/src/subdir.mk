################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/uart_lib.c \
../src/xprintf.c 

OBJS += \
./src/main.o \
./src/uart_lib.o \
./src/xprintf.o 

C_DEPS += \
./src/main.d \
./src/uart_lib.d \
./src/xprintf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\SNC28J60_ethernet\src\core" -I"C:\MikronIDE\workspace\SNC28J60_ethernet\src\periphery" -I"C:\MikronIDE\workspace\SNC28J60_ethernet\HAL\core\Include" -I"C:\MikronIDE\workspace\SNC28J60_ethernet\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


