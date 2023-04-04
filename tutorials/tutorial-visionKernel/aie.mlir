//===- aie.mlir ------------------------------------------------*- MLIR -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (C) 2022, Advanced Micro Devices, Inc.
//
//===----------------------------------------------------------------------===//

// aiecc.py -j4 --sysroot=%VITIS_SYSROOT% --host-target=aarch64-linux-gnu %s -I%aie_runtime_lib%/  %extraAieCcFlags% %aie_runtime_lib%/test_library.cpp %S/test.cpp -o tutorial-9.exe

// REQUIRES: valid_xchess_license
// RUN: make -C %S
// RUN: %run_on_board ./tutorial-9.exe
// RUN: make -C %S clean

// Declare this MLIR module. A block encapsulates all 
// AIE tiles, buffers, and communication in an AI Engine design
module @tutorial_visonKernel {

// AIE.device(xcve2802) {
//    AIE.target("AIE2")


%tile34 = AIE.tile(3, 4)
    %tile70 = AIE.tile(7, 0)

    %ext_buf70_in  = AIE.external_buffer {sym_name = "buffer_in"}: memref<32x32xi16> 
    %ext_buf70_out = AIE.external_buffer {sym_name = "buffer_out"}: memref<32x32xi16> 

    %objFifo_in = AIE.objectFifo.createObjectFifo(%tile70, {%tile34}, 1) : !AIE.objectFifo<memref<32x32xi16>>
    %objFifo_out = AIE.objectFifo.createObjectFifo(%tile34, {%tile70}, 1) : !AIE.objectFifo<memref<32x32xi16>>

    AIE.objectFifo.registerExternalBuffers(%tile70, %objFifo_in : !AIE.objectFifo<memref<32x32xi16>>, {%ext_buf70_in}) : (memref<32x32xi16>)
    AIE.objectFifo.registerExternalBuffers(%tile70, %objFifo_out : !AIE.objectFifo<memref<32x32xi16>>, {%ext_buf70_out}) : (memref<32x32xi16>)

    func.func private @vitis_vision_threshold(%in: memref<32x32xi16>, %out: memref<32x32xi16>) -> ()
 
    %core34 = AIE.core(%tile34) {
        %c0 = arith.constant 0 : index
        %c1 = arith.constant 1 : index
        %cNumberOfSubImages = arith.constant 1 : index
        
        scf.for %iter = %c0 to %cNumberOfSubImages step %c1 { 
            %inputSubview = AIE.objectFifo.acquire<Consume>(%objFifo_in : !AIE.objectFifo<memref<32x32xi16>>, 1) : !AIE.objectFifoSubview<memref<32x32xi16>>
            %outputSubview = AIE.objectFifo.acquire<Produce>(%objFifo_out : !AIE.objectFifo<memref<32x32xi16>>, 1) : !AIE.objectFifoSubview<memref<32x32xi16>>
            
            %input = AIE.objectFifo.subview.access %inputSubview[0] : !AIE.objectFifoSubview<memref<32x32xi16>> -> memref<32x32xi16>
            %output = AIE.objectFifo.subview.access %outputSubview[0] : !AIE.objectFifoSubview<memref<32x32xi16>> -> memref<32x32xi16>

            func.call @vitis_vision_threshold(%input, %output) : (memref<32x32xi16>, memref<32x32xi16>) -> ()
            
            AIE.objectFifo.release<Consume>(%objFifo_in : !AIE.objectFifo<memref<32x32xi16>>, 1)
            AIE.objectFifo.release<Produce>(%objFifo_out : !AIE.objectFifo<memref<32x32xi16>>, 1)
        }
            
        AIE.end
    } { link_with="vitisVisionThreshold.o" } // indicate kernel object name used by this core

}