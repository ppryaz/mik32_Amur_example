################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/mik32_hal_rcc.c \
../src/mik32_hal_spifi.c 

OBJS += \
./src/main.o \
./src/mik32_hal_rcc.o \
./src/mik32_hal_spifi.o 

C_DEPS += \
./src/main.d \
./src/mik32_hal_rcc.d \
./src/mik32_hal_spifi.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og  -g3 -I"C:\sc-dt\workspace\Bootloader_v2\src\core" -I"C:\sc-dt\workspace\Bootloader_v2\src\periphery" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


