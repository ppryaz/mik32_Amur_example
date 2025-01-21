################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Network/arp.c \
../src/Network/enc28j60.c \
../src/Network/ethernet.c \
../src/Network/icmp.c \
../src/Network/ip.c \
../src/Network/network.c \
../src/Network/tcp.c \
../src/Network/udp.c \
../src/Network/util.c 

OBJS += \
./src/Network/arp.o \
./src/Network/enc28j60.o \
./src/Network/ethernet.o \
./src/Network/icmp.o \
./src/Network/ip.o \
./src/Network/network.o \
./src/Network/tcp.o \
./src/Network/udp.o \
./src/Network/util.o 

C_DEPS += \
./src/Network/arp.d \
./src/Network/enc28j60.d \
./src/Network/ethernet.d \
./src/Network/icmp.d \
./src/Network/ip.d \
./src/Network/network.d \
./src/Network/tcp.d \
./src/Network/udp.d \
./src/Network/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/Network/%.o: ../src/Network/%.c src/Network/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\src\core" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\src\periphery" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\HAL\core\Include" -I"C:\MikronIDE\workspace\SNC28J60_ethernetUIP\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


