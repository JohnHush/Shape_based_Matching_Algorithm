################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
ModelBuilding.obj: ../ModelBuilding.cpp $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400 -g --define=_TMS320C6X --include_path="D:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="E:/workspace/emcv/cxcore" --include_path="E:/workspace/emcv/cv" --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="ModelBuilding.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.cpp $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400 -g --define=_TMS320C6X --include_path="D:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="E:/workspace/emcv/cxcore" --include_path="E:/workspace/emcv/cv" --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

su_alloc.obj: ../su_alloc.cpp $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400 -g --define=_TMS320C6X --include_path="D:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="E:/workspace/emcv/cxcore" --include_path="E:/workspace/emcv/cv" --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="su_alloc.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


