################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTFormat.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTPacket.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.c \
../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.c 

OBJS += \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTFormat.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTPacket.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.o \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.o 

C_DEPS += \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTFormat.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTPacket.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.d \
./src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/%.o: ../src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/%.c src/ioLibrary_Driver/Internet/MQTT/MQTTPacket/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Og -ffunction-sections  -g3 -DMIK32V2 -I"C:\MikronIDE\workspace\W5500_ethernet\src\core" -I"C:\MikronIDE\workspace\W5500_ethernet\src\periphery" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\core\Include" -I"C:\MikronIDE\workspace\W5500_ethernet\HAL\peripherals\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


