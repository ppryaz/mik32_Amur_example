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
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og  -g3 -DMIK32V2 -I"C:\MikronWORK\MikronIDE\workspace\Lab2\src\core" -I"C:\MikronWORK\MikronIDE\workspace\Lab2\src\periphery" -I"C:\MikronWORK\MikronIDE\workspace\Lab2\HAL\core\Include" -I"C:\MikronWORK\MikronIDE\workspace\Lab2\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


