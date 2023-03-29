//===- test.cc -------------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (C) 2022, Advanced Micro Devices, Inc.
//
//===----------------------------------------------------------------------===//

#include "threshold.h"
#include <stdio.h>

#define BUF_SIZE 256
//#define BUF_SIZE 2048
int main() {

  int16_t inbuf[BUF_SIZE];
  int16_t outbuf[BUF_SIZE];
  int32_t img_width  = 256;
  //int32_t img_height = 256;
  //int32_t img_height = 16;
  int32_t img_height = 2;
  int32_t thresh_val = 100;
  int32_t max_val = 255;

  //my_threshold(inbuf,outbuf,img_width,img_height,thresh_val,max_val);
  //my_threshold(inbuf,outbuf);
  vitis_vision_threshold(inbuf,outbuf);
  //printf("buf[3] = %d\n", buf[3]);
  return 0;
}
