################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fatfs/diskio.c \
../src/fatfs/ff.c \
../src/fatfs/ffsystem.c \
../src/fatfs/ffunicode.c 

OBJS += \
./src/fatfs/diskio.o \
./src/fatfs/ff.o \
./src/fatfs/ffsystem.o \
./src/fatfs/ffunicode.o 

C_DEPS += \
./src/fatfs/diskio.d \
./src/fatfs/ff.d \
./src/fatfs/ffsystem.d \
./src/fatfs/ffunicode.d 


# Each subdirectory must supply rules for building sources it contributes
src/fatfs/%.o: ../src/fatfs/%.c src/fatfs/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\SD_spi_FatFS\src\core" -I"C:\MikronIDE\workspace\SD_spi_FatFS\src\periphery" -I"C:\MikronIDE\workspace\SD_spi_FatFS\HAL\core\Include" -I"C:\MikronIDE\workspace\SD_spi_FatFS\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


