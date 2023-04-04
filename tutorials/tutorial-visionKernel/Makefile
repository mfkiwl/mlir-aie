include ../../utils/vitisVariables.config
include ../makefile-common

.PHONY: all clean

all: tutorial-visionKernel.exe
	@mkdir -p elf
	@mv *.elf* ./elf
	@cp ./elf/*.elf ./elf/*.elf.map .

external_kernel/threshold.o: external_kernel/threshold.cc
#	$(MAKE) -C external_kernel build
	$(MAKE) -C external_kernel mlir

vitisVisionThreshold.o: external_kernel/threshold.o
	cp external_kernel/threshold.o ./vitisVisionThreshold.o
#	cp external_kernel/kernel.o ./kernel.o

# Command line mlir-aie compile script "aiecc.py"
# Sysroot and host target used to cross compile  host executable
# Local runtime_lib needed for testbench functions
tutorial-visionKernel.exe: test.cpp aie.mlir vitisVisionThreshold.o
	aiecc.py -v -j4 $(AIECC_FLAGS) $(word 2,$^) $(AIECC_HOST_FLAGS) ./$< -o $@

clean_aie:
	rm -rf acdc_project sim aiesimulator_output *elf core* *log *vcd *exe pl_sample_counts .AIE_SIM_CMD_LINE_OPTIONS

clean:
	rm -rf acdc_project sim aiesimulator_output *elf core* *log *vcd *exe pl_sample_counts *.o .AIE_SIM_CMD_LINE_OPTIONS external_kernel/*.o
	$(MAKE) -C external_kernel clean
