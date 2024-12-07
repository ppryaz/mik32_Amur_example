################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/i2c_driver.c \
../src/main.c \
../src/sd.c \
../src/uart_lib.c \
../src/xprintf.c 

OBJS += \
./src/i2c_driver.o \
./src/main.o \
./src/sd.o \
./src/uart_lib.o \
./src/xprintf.o 

C_DEPS += \
./src/i2c_driver.d \
./src/main.d \
./src/sd.d \
./src/uart_lib.d \
./src/xprintf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\SD_spi_FatFS\src\core" -I"C:\MikronIDE\workspace\SD_spi_FatFS\src\periphery" -I"C:\MikronIDE\workspace\SD_spi_FatFS\HAL\core\Include" -I"C:\MikronIDE\workspace\SD_spi_FatFS\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


