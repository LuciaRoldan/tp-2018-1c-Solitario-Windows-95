################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Plani

dependents:
	-cd /home/utnso/tp-2018-1c-Solitario-Windows-95/Commons/Debug && $(MAKE) all
	-cd /home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Debug && $(MAKE) all

# Tool invocations
Plani: $(OBJS) $(USER_OBJS) /home/utnso/tp-2018-1c-Solitario-Windows-95/Commons/Debug/libCommons.so /home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Debug/libCommons_propias.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Debug" -L"/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons/Debug" -pthread -o "Plani" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Plani
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons/Debug/libCommons.so:
/home/utnso/tp-2018-1c-Solitario-Windows-95/Commons_propias/Debug/libCommons_propias.so:

-include ../makefile.targets
