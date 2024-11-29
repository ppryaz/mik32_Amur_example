################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/GUI_Paint.c \
../src/ImageData.c \
../src/OLED_1in3.c \
../src/OLED_1in3_test.c \
../src/font12.c \
../src/font16.c \
../src/font8.c \
../src/i2c_driver.c \
../src/lcd2004a_i2c.c \
../src/main.c \
../src/mikron_logo.c \
../src/uart_lib.c \
../src/xprintf.c 

OBJS += \
./src/GUI_Paint.o \
./src/ImageData.o \
./src/OLED_1in3.o \
./src/OLED_1in3_test.o \
./src/font12.o \
./src/font16.o \
./src/font8.o \
./src/i2c_driver.o \
./src/lcd2004a_i2c.o \
./src/main.o \
./src/mikron_logo.o \
./src/uart_lib.o \
./src/xprintf.o 

C_DEPS += \
./src/GUI_Paint.d \
./src/ImageData.d \
./src/OLED_1in3.d \
./src/OLED_1in3_test.d \
./src/font12.d \
./src/font16.d \
./src/font8.d \
./src/i2c_driver.d \
./src/lcd2004a_i2c.d \
./src/main.d \
./src/mikron_logo.d \
./src/uart_lib.d \
./src/xprintf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\lcd2004a_i2c\src\core" -I"C:\MikronIDE\workspace\lcd2004a_i2c\src\periphery" -I"C:\MikronIDE\workspace\lcd2004a_i2c\HAL\core\Include" -I"C:\MikronIDE\workspace\lcd2004a_i2c\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


