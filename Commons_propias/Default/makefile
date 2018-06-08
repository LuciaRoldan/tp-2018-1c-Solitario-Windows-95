################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include Debug/Commons_propias/subdir.mk
-include Commons_propias/subdir.mk
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
all: Commons_propias

# Tool invocations
Commons_propias: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/usr/lib/jvm/java-8-openjdk-i386/jre/lib/i386/server /usr/lib/jvm/java-8-openjdk-i386/jre/lib/i386" -o "Commons_propias" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Commons_propias
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
