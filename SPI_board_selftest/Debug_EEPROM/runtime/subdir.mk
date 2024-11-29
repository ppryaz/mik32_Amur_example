################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../runtime/crt0.S 

OBJS += \
./runtime/crt0.o 

S_UPPER_DEPS += \
./runtime/crt0.d 


# Each subdirectory must supply rules for building sources it contributes
runtime/%.o: ../runtime/%.S runtime/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	riscv64-unknown-elf-gcc -march=rv32imc_zicsr_zifencei -mabi=ilp32 -msmall-data-limit=8 -mstrict-align -mno-save-restore -Ofast  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


