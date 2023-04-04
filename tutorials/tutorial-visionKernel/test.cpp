//===- test.cpp -------------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (C) 2022, Advanced Micro Devices, Inc.
//
//===----------------------------------------------------------------------===//

#include "test_library.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>
#include <xaiengine.h>

#include "aie_inc.cpp"

constexpr int WIDTH = 32;
constexpr int HEIGHT = 32;

int main(int argc, char *argv[]) {
    
    printf("Test start.\n");

    int threshold = 100;

    aie_libxaie_ctx_t *_xaie = mlir_aie_init_libxaie();
    mlir_aie_init_device(_xaie);
    mlir_aie_configure_cores(_xaie);
    mlir_aie_configure_switchboxes(_xaie);
    mlir_aie_configure_dmas(_xaie);
    mlir_aie_initialize_locks(_xaie);

    mlir_aie_init_mems(_xaie, 2);

    int16_t *mem_ptr_in = (int16_t *)mlir_aie_mem_alloc(_xaie, 0, WIDTH*HEIGHT/2);
    int16_t *mem_ptr_out = (int16_t *)mlir_aie_mem_alloc(_xaie, 1, WIDTH*HEIGHT/2);

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            mem_ptr_out[i*WIDTH+j] = -1;


    // Set virtual pointer used to configure
#if defined(__AIESIM__)
  mlir_aie_external_set_addr_buffer_in(
      (u64)((_xaie->buffers[0])->physicalAddr));
  mlir_aie_external_set_addr_buffer_out(
      (u64)((_xaie->buffers[1])->physicalAddr));
#else
    mlir_aie_external_set_addr_buffer_in((u64)mem_ptr_in);
    mlir_aie_external_set_addr_buffer_out((u64)mem_ptr_out);
#endif

    mlir_aie_configure_shimdma_70(_xaie);    
    int errors = 0;

    mlir_aie_clear_tile_memory(_xaie, 3, 4);

    if (mlir_aie_acquire_of_0_lock_0(_xaie, 0, 10000) == XAIE_OK)
        printf("Acquired objFifo 0 lock 0 for write\n");
    else
        printf("ERROR: timed out on objFifo 0 lock 0 for write\n");
    
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            mem_ptr_in[i*WIDTH+j] = i*(256/HEIGHT);

    mlir_aie_sync_mem_dev(_xaie, 0);

    mlir_aie_release_of_0_lock_0(_xaie, 1, 0);
    printf("Released objFifo 0 lock 0 for read\n");

    // Helper function to enable all AIE cores
    printf("Start cores\n");
    mlir_aie_start_cores(_xaie);

    // acquire output shim
    if (mlir_aie_acquire_of_3_lock_0(_xaie, 1, 10000) == XAIE_OK)
        printf("Acquired objFifo 3 lock 0 for read\n");
    else
        printf("ERROR: timed out on objFifo 3 lock 0 for read\n");

    // check output DDR
    mlir_aie_sync_mem_cpu(_xaie, 1);

    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            mlir_aie_check("After start cores:", mem_ptr_out[i*WIDTH], i < threshold ? 0 : 255, errors);

    // release output shim
    if (mlir_aie_release_of_3_lock_0(_xaie, 0, 10000) == XAIE_OK)
      printf("Released objFifo 3 lock 0 for write\n");
    else
      printf("ERROR: timed out on objFifo 3 lock 0 for write\n");

    int res = 0;
    if (!errors) {
        printf("PASS!\n");
        res = 0;
    } else {
        printf("Fail!\n");
        res = -1;
    }
 
    mlir_aie_deinit_libxaie(_xaie);

    return res;
}