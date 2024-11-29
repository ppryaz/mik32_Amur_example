################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/i2c_driver.c \
../src/lcd2004a_i2c.c \
../src/main.c \
../src/uart_lib.c \
../src/xprintf.c 

OBJS += \
./src/i2c_driver.o \
./src/lcd2004a_i2c.o \
./src/main.o \
./src/uart_lib.o \
./src/xprintf.o 

C_DEPS += \
./src/i2c_driver.d \
./src/lcd2004a_i2c.d \
./src/main.d \
./src/uart_lib.d \
./src/xprintf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\lcd2004a_i2c_RusText\src\core" -I"C:\MikronIDE\workspace\lcd2004a_i2c_RusText\src\periphery" -I"C:\MikronIDE\workspace\lcd2004a_i2c_RusText\HAL\core\Include" -I"C:\MikronIDE\workspace\lcd2004a_i2c_RusText\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


