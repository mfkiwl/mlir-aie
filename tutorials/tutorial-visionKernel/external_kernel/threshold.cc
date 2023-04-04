//===- kernel.cc -------------------------------------------------*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (C) 2022, Advanced Micro Devices, Inc.
//
//===----------------------------------------------------------------------===//

//#define __AIENGINE__ 1
#define __AIENGINE__ 2
#define NOCPP

//#define h1 32
//#define w1 32
//#define w2 32

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum _threshold_type {
    XF_THRESHOLD_TYPE_BINARY = 0,
    XF_THRESHOLD_TYPE_BINARY_INV = 1,
    XF_THRESHOLD_TYPE_TRUNC = 2,
    XF_THRESHOLD_TYPE_TOZERO = 3,
    XF_THRESHOLD_TYPE_TOZERO_INV = 4,
};

#define REL_WRITE 0
#define REL_READ 1
#define THRESH_TYPE XF_THRESHOLD_TYPE_BINARY

#include <aie_api/aie.hpp>
//#include <aie.hpp>
//#include <imgproc/xf_threshold_aie.hpp>

template <typename T, int N>
__attribute__((noinline)) void threshold(
    T* img_in, T* img_out, const T& img_width, const T& img_height, const T& thresh_val, const T& max_val) {
    ::aie::vector<T, N> constants;
    ::aie::vector<T, N> data_out;
    ::aie::mask<N> temp_val;
    constants[0] = 0;          // updating constant zero_val value
    constants[1] = thresh_val; // updating constant threshold value
    constants[2] = max_val;    // updating constant max_val value

    for (int j = 0; j < (img_height * img_width); j += N) // 16x samples per loop
        chess_prepare_for_pipelining chess_loop_range(14, ) { 
            ::aie::vector<T, N> data_buf1 = ::aie::load_v(img_in); // in:00++15|_________|_________|_________
            img_in += N;
            switch (THRESH_TYPE) {
                case XF_THRESHOLD_TYPE_TRUNC:
                    data_out = ::aie::min(constants[1], data_buf1);
                    break;
                case XF_THRESHOLD_TYPE_BINARY:
                    temp_val = ::aie::lt(constants[1], data_buf1);
                    data_out = ::aie::select(constants[0], constants[2], temp_val);
                    break;
                case XF_THRESHOLD_TYPE_BINARY_INV:
                    temp_val = ::aie::lt(constants[1], data_buf1);
                    data_out = ::aie::select(constants[2], constants[0], temp_val);
                    break;
                case XF_THRESHOLD_TYPE_TOZERO:
                    temp_val = ::aie::lt(constants[1], data_buf1);
                    data_out = ::aie::select(constants[0], data_buf1, temp_val);
                    break;
                case XF_THRESHOLD_TYPE_TOZERO_INV:
                    temp_val = ::aie::lt(constants[1], data_buf1);
                    data_out = ::aie::select(data_buf1, constants[0], temp_val);
                    break;

                default:
                    data_out = ::aie::min(constants[1], data_buf1);
            }
            ::aie::store_v(img_out, data_out);
            img_out += N;
        }
}

extern "C" {

//void extern_kernel(int16_t *restrict buf) { buf[3] = 14; }
//void my_threshold(int16_t *img_in, int16_t *img_out, 
//	const int16_t img_width, const int16_t img_height, 
//	const int16_t thresh_val, const int16_t max_val) 
void vitis_vision_threshold(int16_t *img_in, int16_t *img_out) 
{
    int32_t img_width  = 32;
    int32_t img_height = 32;
    int32_t thresh_val = 100;
    int32_t max_val = 255;

	threshold<int16_t, 32>(img_in, img_out, img_width, img_height, thresh_val, max_val);
}

} // extern "C"
