################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/uIP/clock-arch.c \
../src/uIP/psock.c \
../src/uIP/timer.c \
../src/uIP/uip-fw.c \
../src/uIP/uip-neighbor.c \
../src/uIP/uip-split.c \
../src/uIP/uip.c \
../src/uIP/uip_arp.c \
../src/uIP/uiplib.c 

OBJS += \
./src/uIP/clock-arch.o \
./src/uIP/psock.o \
./src/uIP/timer.o \
./src/uIP/uip-fw.o \
./src/uIP/uip-neighbor.o \
./src/uIP/uip-split.o \
./src/uIP/uip.o \
./src/uIP/uip_arp.o \
./src/uIP/uiplib.o 

C_DEPS += \
./src/uIP/clock-arch.d \
./src/uIP/psock.d \
./src/uIP/timer.d \
./src/uIP/uip-fw.d \
./src/uIP/uip-neighbor.d \
./src/uIP/uip-split.d \
./src/uIP/uip.d \
./src/uIP/uip_arp.d \
./src/uIP/uiplib.d 


# Each subdirectory must supply rules for building sources it contributes
src/uIP/%.o: ../src/uIP/%.c src/uIP/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\src\core" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\src\periphery" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\HAL\core\Include" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


