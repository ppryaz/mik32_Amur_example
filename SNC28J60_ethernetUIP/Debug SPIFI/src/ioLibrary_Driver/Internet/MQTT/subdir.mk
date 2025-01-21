################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ioLibrary_Driver/Internet/MQTT/MQTTClient.c \
../src/ioLibrary_Driver/Internet/MQTT/mqtt_interface.c 

OBJS += \
./src/ioLibrary_Driver/Internet/MQTT/MQTTClient.o \
./src/ioLibrary_Driver/Internet/MQTT/mqtt_interface.o 

C_DEPS += \
./src/ioLibrary_Driver/Internet/MQTT/MQTTClient.d \
./src/ioLibrary_Driver/Internet/MQTT/mqtt_interface.d 


# Each subdirectory must supply rules for building sources it contributes
src/ioLibrary_Driver/Internet/MQTT/%.o: ../src/ioLibrary_Driver/Internet/MQTT/%.c src/ioLibrary_Driver/Internet/MQTT/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\W5500_ethernet\src\core" -I"C:\MikronIDE\workspace\W5500_ethernet\src\periphery" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\core\Include" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


