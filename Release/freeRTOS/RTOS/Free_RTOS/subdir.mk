################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freeRTOS/RTOS/Free_RTOS/croutine.c \
../freeRTOS/RTOS/Free_RTOS/heap_1.c \
../freeRTOS/RTOS/Free_RTOS/list.c \
../freeRTOS/RTOS/Free_RTOS/port.c \
../freeRTOS/RTOS/Free_RTOS/queue.c \
../freeRTOS/RTOS/Free_RTOS/tasks.c \
../freeRTOS/RTOS/Free_RTOS/timers.c 

OBJS += \
./freeRTOS/RTOS/Free_RTOS/croutine.o \
./freeRTOS/RTOS/Free_RTOS/heap_1.o \
./freeRTOS/RTOS/Free_RTOS/list.o \
./freeRTOS/RTOS/Free_RTOS/port.o \
./freeRTOS/RTOS/Free_RTOS/queue.o \
./freeRTOS/RTOS/Free_RTOS/tasks.o \
./freeRTOS/RTOS/Free_RTOS/timers.o 

C_DEPS += \
./freeRTOS/RTOS/Free_RTOS/croutine.d \
./freeRTOS/RTOS/Free_RTOS/heap_1.d \
./freeRTOS/RTOS/Free_RTOS/list.d \
./freeRTOS/RTOS/Free_RTOS/port.d \
./freeRTOS/RTOS/Free_RTOS/queue.d \
./freeRTOS/RTOS/Free_RTOS/tasks.d \
./freeRTOS/RTOS/Free_RTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
freeRTOS/RTOS/Free_RTOS/%.o: ../freeRTOS/RTOS/Free_RTOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


