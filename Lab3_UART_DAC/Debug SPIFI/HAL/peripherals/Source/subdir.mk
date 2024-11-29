################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/peripherals/Source/mik32_hal.c \
../HAL/peripherals/Source/mik32_hal_adc.c \
../HAL/peripherals/Source/mik32_hal_crc32.c \
../HAL/peripherals/Source/mik32_hal_crypto.c \
../HAL/peripherals/Source/mik32_hal_dac.c \
../HAL/peripherals/Source/mik32_hal_dma.c \
../HAL/peripherals/Source/mik32_hal_gpio.c \
../HAL/peripherals/Source/mik32_hal_i2c.c \
../HAL/peripherals/Source/mik32_hal_irq.c \
../HAL/peripherals/Source/mik32_hal_otp.c \
../HAL/peripherals/Source/mik32_hal_pcc.c \
../HAL/peripherals/Source/mik32_hal_rtc.c \
../HAL/peripherals/Source/mik32_hal_spi.c \
../HAL/peripherals/Source/mik32_hal_spifi.c \
../HAL/peripherals/Source/mik32_hal_timer16.c \
../HAL/peripherals/Source/mik32_hal_timer32.c 

OBJS += \
./HAL/peripherals/Source/mik32_hal.o \
./HAL/peripherals/Source/mik32_hal_adc.o \
./HAL/peripherals/Source/mik32_hal_crc32.o \
./HAL/peripherals/Source/mik32_hal_crypto.o \
./HAL/peripherals/Source/mik32_hal_dac.o \
./HAL/peripherals/Source/mik32_hal_dma.o \
./HAL/peripherals/Source/mik32_hal_gpio.o \
./HAL/peripherals/Source/mik32_hal_i2c.o \
./HAL/peripherals/Source/mik32_hal_irq.o \
./HAL/peripherals/Source/mik32_hal_otp.o \
./HAL/peripherals/Source/mik32_hal_pcc.o \
./HAL/peripherals/Source/mik32_hal_rtc.o \
./HAL/peripherals/Source/mik32_hal_spi.o \
./HAL/peripherals/Source/mik32_hal_spifi.o \
./HAL/peripherals/Source/mik32_hal_timer16.o \
./HAL/peripherals/Source/mik32_hal_timer32.o 

C_DEPS += \
./HAL/peripherals/Source/mik32_hal.d \
./HAL/peripherals/Source/mik32_hal_adc.d \
./HAL/peripherals/Source/mik32_hal_crc32.d \
./HAL/peripherals/Source/mik32_hal_crypto.d \
./HAL/peripherals/Source/mik32_hal_dac.d \
./HAL/peripherals/Source/mik32_hal_dma.d \
./HAL/peripherals/Source/mik32_hal_gpio.d \
./HAL/peripherals/Source/mik32_hal_i2c.d \
./HAL/peripherals/Source/mik32_hal_irq.d \
./HAL/peripherals/Source/mik32_hal_otp.d \
./HAL/peripherals/Source/mik32_hal_pcc.d \
./HAL/peripherals/Source/mik32_hal_rtc.d \
./HAL/peripherals/Source/mik32_hal_spi.d \
./HAL/peripherals/Source/mik32_hal_spifi.d \
./HAL/peripherals/Source/mik32_hal_timer16.d \
./HAL/peripherals/Source/mik32_hal_timer32.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/peripherals/Source/%.o: ../HAL/peripherals/Source/%.c HAL/peripherals/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\Lab3_UART_DAC\src\core" -I"C:\MikronIDE\workspace\Lab3_UART_DAC\src\periphery" -I"C:\MikronIDE\workspace\Lab3_UART_DAC\HAL\core\Include" -I"C:\MikronIDE\workspace\Lab3_UART_DAC\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


