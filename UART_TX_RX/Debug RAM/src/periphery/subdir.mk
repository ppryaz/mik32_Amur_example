################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/periphery/uart_lib.c 

OBJS += \
./src/periphery/uart_lib.o 

C_DEPS += \
./src/periphery/uart_lib.d 


# Each subdirectory must supply rules for building sources it contributes
src/periphery/%.o: ../src/periphery/%.c src/periphery/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og  -g3 -DMIK32V2 -I"Z:\Examples_mik32Amur\UART_TX_RX\src\core" -I"Z:\Examples_mik32Amur\UART_TX_RX\src\periphery" -I"Z:\Examples_mik32Amur\UART_TX_RX\HAL\core\Include" -I"Z:\Examples_mik32Amur\UART_TX_RX\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


