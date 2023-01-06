################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
BoardSupport/driverlib/%.obj: ../BoardSupport/driverlib/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/FINAL2" --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/Lab4/BoardSupport" --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/Lab4_SOL/BoardSupport/inc" --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/Lab4_SOL/G8RTOS_Lab4" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="BoardSupport/driverlib/$(basename $(<F)).d_raw" --obj_directory="BoardSupport/driverlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

BoardSupport/driverlib/%.obj: ../BoardSupport/driverlib/%.s $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/FINAL2" --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/Lab4/BoardSupport" --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/Lab4_SOL/BoardSupport/inc" --include_path="C:/Users/Beck/OneDrive - University of Florida/Documents/EEL4754C/CCS/Lab4_SOL/G8RTOS_Lab4" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="BoardSupport/driverlib/$(basename $(<F)).d_raw" --obj_directory="BoardSupport/driverlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


