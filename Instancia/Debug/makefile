################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: instancia

dependents:
	-cd /home/utnso/workspace/commons/Debug && $(MAKE) clean all
	-cd /home/utnso/workspace/commons_propias/Debug && $(MAKE) clean all

# Tool invocations
instancia: $(OBJS) $(USER_OBJS) /home/utnso/workspace/commons/Debug/libcommons.so /home/utnso/workspace/commons_propias/Debug/libcommons_propias.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/commons_propias/Debug" -L"/home/utnso/workspace/commons/Debug" -o "instancia" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) instancia
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/commons/Debug/libcommons.so:
/home/utnso/workspace/commons_propias/Debug/libcommons_propias.so:

-include ../makefile.targets
