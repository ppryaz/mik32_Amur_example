################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/enc28j60.c \
../src/main.c \
../src/uart_lib.c \
../src/xprintf.c 

OBJS += \
./src/enc28j60.o \
./src/main.o \
./src/uart_lib.o \
./src/xprintf.o 

C_DEPS += \
./src/enc28j60.d \
./src/main.d \
./src/uart_lib.d \
./src/xprintf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\src\core" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\src\periphery" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\HAL\core\Include" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


