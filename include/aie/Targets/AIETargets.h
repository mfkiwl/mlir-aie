//===- AIETargets.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef AIE_TARGETS_AIETARGETS_H
#define AIE_TARGETS_AIETARGETS_H

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/LogicalResult.h"

namespace xilinx {
namespace AIE {
class DeviceOp;

mlir::LogicalResult AIETranslateToXAIEV2(mlir::ModuleOp module,
                                         llvm::raw_ostream &output);
mlir::LogicalResult AIEFlowsToJSON(mlir::ModuleOp module,
                                   llvm::raw_ostream &output);
mlir::LogicalResult ADFGenerateCPPGraph(mlir::ModuleOp module,
                                        llvm::raw_ostream &output);
mlir::LogicalResult AIETranslateSCSimConfig(mlir::ModuleOp module,
                                            llvm::raw_ostream &output);
mlir::LogicalResult AIETranslateShimSolution(mlir::ModuleOp module,
                                             llvm::raw_ostream &);
mlir::LogicalResult AIETranslateGraphXPE(mlir::ModuleOp module,
                                         llvm::raw_ostream &);
mlir::LogicalResult AIETranslateToCDO(mlir::ModuleOp module,
                                      llvm::raw_ostream &output);
mlir::LogicalResult AIETranslateToIPU(mlir::ModuleOp module,
                                      llvm::raw_ostream &output);
std::vector<uint32_t> AIETranslateToIPU(mlir::ModuleOp);
mlir::LogicalResult AIETranslateToLdScript(mlir::ModuleOp module,
                                           llvm::raw_ostream &output,
                                           int tileCol, int tileRow);
mlir::LogicalResult AIETranslateToBCF(mlir::ModuleOp module,
                                      llvm::raw_ostream &output, int tileCol,
                                      int tileRow);
mlir::LogicalResult
AIELLVMLink(llvm::raw_ostream &output, std::vector<std::string> Files,
            bool DisableDITypeMap = false, bool NoVerify = false,
            bool Internalize = false, bool OnlyNeeded = false,
            bool PreserveAssemblyUseListOrder = false, bool Verbose = false);

#ifdef AIE_ENABLE_GENERATE_CDO_DIRECT
#include "cdo_driver.h"
mlir::LogicalResult AIETranslateToCDODirect(mlir::ModuleOp &m,
                                            const std::string &workDirPath,
                                            byte_ordering endianness,
                                            bool emitUnified);

#define XAIE_DEV_GEN_AIEML 2U
#define HW_GEN XAIE_DEV_GEN_AIEML
#define XAIE_NUM_ROWS 6
#define XAIE_NUM_COLS 5
#define XAIE_BASE_ADDR 0x40000000
#define XAIE_COL_SHIFT 25
#define XAIE_ROW_SHIFT 20
#define XAIE_SHIM_ROW 0
#define XAIE_MEM_TILE_ROW_START 1
#define XAIE_MEM_TILE_NUM_ROWS 1
#define XAIE_AIE_TILE_ROW_START 2
#define XAIE_AIE_TILE_NUM_ROWS 4
#define XAIE_PARTITION_BASE_ADDR 0x0
#define NPI_ADDR 0x0

struct XAie_Config_;
struct XAie_DevInst_;
struct XAieCDOControl {
  XAieCDOControl(uint8_t hwGen = XAIE_DEV_GEN_AIEML,
                 uint64_t xaieBaseAddr = XAIE_BASE_ADDR,
                 uint8_t xaieColShift = XAIE_COL_SHIFT,
                 uint8_t xaieRowShift = XAIE_ROW_SHIFT,
                 uint8_t xaieNumCols = XAIE_NUM_COLS,
                 uint8_t xaieNumRows = XAIE_NUM_ROWS,
                 uint8_t xaieShimRow = XAIE_SHIM_ROW,
                 uint8_t xaieMemTileRowStart = XAIE_MEM_TILE_ROW_START,
                 uint8_t xaieMemTileNumRows = XAIE_MEM_TILE_NUM_ROWS,
                 uint8_t xaieAieTileRowStart = XAIE_AIE_TILE_ROW_START,
                 uint8_t xaieAieTileNumRows = XAIE_AIE_TILE_NUM_ROWS,
                 uint64_t xaiePartitionBaseAddr = XAIE_PARTITION_BASE_ADDR,
                 uint64_t npiAddr = NPI_ADDR);

  void addErrorHandlingToCDO();
  void addAieElfToCDO(uint8_t col, uint8_t row, const std::string &elfPath);
  void addAieElfsToCDO(DeviceOp &targetOp, const std::string &workDirPath);
  void addInitConfigToCDO(DeviceOp &targetOp);
  void addCoreEnableToCDO(DeviceOp &targetOp);

private:
  std::unique_ptr<XAie_Config_> configPtr;
  std::unique_ptr<XAie_DevInst_> devInst;
};

void initializeCDOGenerator(byte_ordering endianness);
void generateCDOBinariesSeparately(XAieCDOControl &ctl,
                                   const std::string &workDirPath,
                                   DeviceOp &targetOp);
void generateCDOUnified(XAieCDOControl &ctl, const std::string &workDirPath,
                        DeviceOp &targetOp);
#endif
} // namespace AIE

namespace aievec {

/// Translates the AIE vector dialect MLIR to C++ code.
mlir::LogicalResult translateAIEVecToCpp(mlir::Operation *op, bool aieml,
                                         mlir::raw_ostream &os);

} // namespace aievec
} // namespace xilinx

#endif