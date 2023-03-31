include ../../../utils/vitisVariables.config
include ../../makefile-common

.PHONY: build gui sim

mlir: threshold.o

%.o: %.cc
	xchesscc -f ${CHESSCC_FLAGS} -c $< -o $@

build:
	xchessmk ${CHESS_FLAGS} test.prx

gui:
	xchessde ${CHESS_FLAGS} test.prx &

sim:
	xca_udm_dbg ${CHESS_FLAGS} -t sim.tcl

clean:
	rm -rf work kernel.o test.prf
