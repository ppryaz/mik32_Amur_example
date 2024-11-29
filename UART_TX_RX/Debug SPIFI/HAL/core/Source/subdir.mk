################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/core/Source/mik32_hal_scr1_timer.c 

OBJS += \
./HAL/core/Source/mik32_hal_scr1_timer.o 

C_DEPS += \
./HAL/core/Source/mik32_hal_scr1_timer.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/core/Source/%.o: ../HAL/core/Source/%.c HAL/core/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\UART_TX_RX\src\core" -I"C:\MikronIDE\workspace\UART_TX_RX\src\periphery" -I"C:\MikronIDE\workspace\UART_TX_RX\HAL\core\Include" -I"C:\MikronIDE\workspace\UART_TX_RX\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


