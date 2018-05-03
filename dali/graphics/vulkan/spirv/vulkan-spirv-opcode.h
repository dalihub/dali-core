/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef DALI_GRAPHICS_VULKAN_SPIRV_SPIRV_OPCODES_H
#define DALI_GRAPHICS_VULKAN_SPIRV_SPIRV_OPCODES_H

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
namespace SpirV
{
struct SPIRVOpCode
{
  std::string name{"OpNull"};
  uint32_t    code{static_cast<uint32_t>( -1u )};
  bool        hasResult{false};
  bool        hasResultType{false};
  bool        isType{false};
  bool        isTrivial{false}; // trivial types do not reference to any other result

  /** Defines local data */
  struct LocalData
  {
    uint32_t* start{nullptr};
    uint32_t  count{0u};
    uint32_t  resultId{0u};
    int32_t   resultType{-1};
  };

  LocalData localData{};

  bool operator!=( const SPIRVOpCode& opcode ) const
  {
    return opcode.code != code;
  }

  bool operator==( const SPIRVOpCode& opcode ) const
  {
    return opcode.code == code;
  }

  bool operator==( SpvOp op ) const
  {
    return ( code == static_cast<uint32_t>(op) );
  }

  template<class T>
  T GetParameter( uint32_t index ) const
  {
    return static_cast<T>( *( localData.start + index + 1 ) );
  }

  uint32_t GetParameterU32( uint32_t index ) const
  {
    return GetParameter<uint32_t>( index );
  }

  std::string GetParameterAsString( uint32_t index ) const
  {
    return reinterpret_cast<const char*>( localData.start + index + 1 );
  }

  template<class T>
  T operator[]( uint32_t index ) const
  {
    return GetParameter<T>( index );
  }

  SPIRVOpCode()  = default;
  ~SPIRVOpCode() = default;
};

namespace
{

/**
 * List of all the SPIR-V opcodes
 * OpCodes describing types: 19-39
 */
static const SPIRVOpCode OP_CODE_ARRAY[] = {{"OpNop", 0, false, false, false},
                                            {"OpUndef", 1, true, true, false},
                                            {"OpSourceContinued", 2, false, false, false},
                                            {"OpSource", 3, false, false, false},
                                            {"OpSourceExtension", 4, false, false, false},
                                            {"OpName", 5, false, false, false},
                                            {"OpMemberName", 6, false, false, false},
                                            {"OpString", 7, true, false, false},
                                            {"OpLine", 8, false, false, false},
                                            {"OpExtension", 10, false, false, false},
                                            {"OpExtInstImport", 11, true, false, false},
                                            {"OpExtInst", 12, true, true, false},
                                            {"OpMemoryModel", 14, false, false, false},
                                            {"OpEntryPoint", 15, false, false, false},
                                            {"OpExecutionMode", 16, false, false, false},
                                            {"OpCapability", 17, false, false, false},
                                            {"OpTypeVoid", 19, true, false, true, true},
                                            {"OpTypeBool", 20, true, false, true, true},
                                            {"OpTypeInt", 21, true, false, true, true},
                                            {"OpTypeFloat", 22, true, false, true, true},
                                            {"OpTypeVector", 23, true, false, true},
                                            {"OpTypeMatrix", 24, true, false, true},
                                            {"OpTypeImage", 25, true, false, true},
                                            {"OpTypeSampler", 26, true, false, true},
                                            {"OpTypeSampledImage", 27, true, false, true},
                                            {"OpTypeArray", 28, true, false, true},
                                            {"OpTypeRuntimeArray", 29, true, false, true},
                                            {"OpTypeStruct", 30, true, false, true},
                                            {"OpTypeOpaque", 31, true, false, true},
                                            {"OpTypePointer", 32, true, false, true},
                                            {"OpTypeFunction", 33, true, false, true},
                                            {"OpTypeEvent", 34, true, false, true},
                                            {"OpTypeDeviceEvent", 35, true, false, true},
                                            {"OpTypeReserveId", 36, true, false, true},
                                            {"OpTypeQueue", 37, true, false, true},
                                            {"OpTypePipe", 38, true, false, true},
                                            {"OpTypeForwardPointer", 39, false, false, true},
                                            {"OpConstantTrue", 41, true, true, false},
                                            {"OpConstantFalse", 42, true, true, false},
                                            {"OpConstant", 43, true, true, false},
                                            {"OpConstantComposite", 44, true, true, false},
                                            {"OpConstantSampler", 45, true, true, false},
                                            {"OpConstantNull", 46, true, true, false},
                                            {"OpSpecConstantTrue", 48, true, true, false},
                                            {"OpSpecConstantFalse", 49, true, true, false},
                                            {"OpSpecConstant", 50, true, true, false},
                                            {"OpSpecConstantComposite", 51, true, true, false},
                                            {"OpSpecConstantOp", 52, true, true, false},
                                            {"OpFunction", 54, true, true, false},
                                            {"OpFunctionParameter", 55, true, true, false},
                                            {"OpFunctionEnd", 56, false, false, false},
                                            {"OpFunctionCall", 57, true, true, false},
                                            {"OpVariable", 59, true, true, false},
                                            {"OpImageTexelPointer", 60, true, true, false},
                                            {"OpLoad", 61, true, true, false},
                                            {"OpStore", 62, false, false, false},
                                            {"OpCopyMemory", 63, false, false, false},
                                            {"OpCopyMemorySized", 64, false, false, false},
                                            {"OpAccessChain", 65, true, true, false},
                                            {"OpInBoundsAccessChain", 66, true, true, false},
                                            {"OpPtrAccessChain", 67, true, true, false},
                                            {"OpArrayLength", 68, true, true, false},
                                            {"OpGenericPtrMemSemantics", 69, true, true, false},
                                            {"OpInBoundsPtrAccessChain", 70, true, true, false},
                                            {"OpDecorate", 71, false, false, false},
                                            {"OpMemberDecorate", 72, false, false, false},
                                            {"OpDecorationGroup", 73, true, false, false},
                                            {"OpGroupDecorate", 74, false, false, false},
                                            {"OpGroupMemberDecorate", 75, false, false, false},
                                            {"OpVectorExtractDynamic", 77, true, true, false},
                                            {"OpVectorInsertDynamic", 78, true, true, false},
                                            {"OpVectorShuffle", 79, true, true, false},
                                            {"OpCompositeConstruct", 80, true, true, false},
                                            {"OpCompositeExtract", 81, true, true, false},
                                            {"OpCompositeInsert", 82, true, true, false},
                                            {"OpCopyObject", 83, true, true, false},
                                            {"OpTranspose", 84, true, true, false},
                                            {"OpSampledImage", 86, true, true, false},
                                            {"OpImageSampleImplicitLod", 87, true, true, false},
                                            {"OpImageSampleExplicitLod", 88, true, true, false},
                                            {"OpImageSampleDrefImplicitLod", 89, true, true, false},
                                            {"OpImageSampleDrefExplicitLod", 90, true, true, false},
                                            {"OpImageSampleProjImplicitLod", 91, true, true, false},
                                            {"OpImageSampleProjExplicitLod", 92, true, true, false},
                                            {"OpImageSampleProjDrefImplicitLod", 93, true, true, false},
                                            {"OpImageSampleProjDrefExplicitLod", 94, true, true, false},
                                            {"OpImageFetch", 95, true, true, false},
                                            {"OpImageGather", 96, true, true, false},
                                            {"OpImageDrefGather", 97, true, true, false},
                                            {"OpImageRead", 98, true, true, false},
                                            {"OpImageWrite", 99, false, false, false},
                                            {"OpImage", 100, true, true, false},
                                            {"OpImageQueryFormat", 101, true, true, false},
                                            {"OpImageQueryOrder", 102, true, true, false},
                                            {"OpImageQuerySizeLod", 103, true, true, false},
                                            {"OpImageQuerySize", 104, true, true, false},
                                            {"OpImageQueryLod", 105, true, true, false},
                                            {"OpImageQueryLevels", 106, true, true, false},
                                            {"OpImageQuerySamples", 107, true, true, false},
                                            {"OpConvertFToU", 109, true, true, false},
                                            {"OpConvertFToS", 110, true, true, false},
                                            {"OpConvertSToF", 111, true, true, false},
                                            {"OpConvertUToF", 112, true, true, false},
                                            {"OpUConvert", 113, true, true, false},
                                            {"OpSConvert", 114, true, true, false},
                                            {"OpFConvert", 115, true, true, false},
                                            {"OpQuantizeToF16", 116, true, true, false},
                                            {"OpConvertPtrToU", 117, true, true, false},
                                            {"OpSatConvertSToU", 118, true, true, false},
                                            {"OpSatConvertUToS", 119, true, true, false},
                                            {"OpConvertUToPtr", 120, true, true, false},
                                            {"OpPtrCastToGeneric", 121, true, true, false},
                                            {"OpGenericCastToPtr", 122, true, true, false},
                                            {"OpGenericCastToPtrExplicit", 123, true, true, false},
                                            {"OpBitcast", 124, true, true, false},
                                            {"OpSNegate", 126, true, true, false},
                                            {"OpFNegate", 127, true, true, false},
                                            {"OpIAdd", 128, true, true, false},
                                            {"OpFAdd", 129, true, true, false},
                                            {"OpISub", 130, true, true, false},
                                            {"OpFSub", 131, true, true, false},
                                            {"OpIMul", 132, true, true, false},
                                            {"OpFMul", 133, true, true, false},
                                            {"OpUDiv", 134, true, true, false},
                                            {"OpSDiv", 135, true, true, false},
                                            {"OpFDiv", 136, true, true, false},
                                            {"OpUMod", 137, true, true, false},
                                            {"OpSRem", 138, true, true, false},
                                            {"OpSMod", 139, true, true, false},
                                            {"OpFRem", 140, true, true, false},
                                            {"OpFMod", 141, true, true, false},
                                            {"OpVectorTimesScalar", 142, true, true, false},
                                            {"OpMatrixTimesScalar", 143, true, true, false},
                                            {"OpVectorTimesMatrix", 144, true, true, false},
                                            {"OpMatrixTimesVector", 145, true, true, false},
                                            {"OpMatrixTimesMatrix", 146, true, true, false},
                                            {"OpOuterProduct", 147, true, true, false},
                                            {"OpDot", 148, true, true, false},
                                            {"OpIAddCarry", 149, true, true, false},
                                            {"OpISubBorrow", 150, true, true, false},
                                            {"OpUMulExtended", 151, true, true, false},
                                            {"OpSMulExtended", 152, true, true, false},
                                            {"OpAny", 154, true, true, false},
                                            {"OpAll", 155, true, true, false},
                                            {"OpIsNan", 156, true, true, false},
                                            {"OpIsInf", 157, true, true, false},
                                            {"OpIsFinite", 158, true, true, false},
                                            {"OpIsNormal", 159, true, true, false},
                                            {"OpSignBitSet", 160, true, true, false},
                                            {"OpLessOrGreater", 161, true, true, false},
                                            {"OpOrdered", 162, true, true, false},
                                            {"OpUnordered", 163, true, true, false},
                                            {"OpLogicalEqual", 164, true, true, false},
                                            {"OpLogicalNotEqual", 165, true, true, false},
                                            {"OpLogicalOr", 166, true, true, false},
                                            {"OpLogicalAnd", 167, true, true, false},
                                            {"OpLogicalNot", 168, true, true, false},
                                            {"OpSelect", 169, true, true, false},
                                            {"OpIEqual", 170, true, true, false},
                                            {"OpINotEqual", 171, true, true, false},
                                            {"OpUGreaterThan", 172, true, true, false},
                                            {"OpSGreaterThan", 173, true, true, false},
                                            {"OpUGreaterThanEqual", 174, true, true, false},
                                            {"OpSGreaterThanEqual", 175, true, true, false},
                                            {"OpULessThan", 176, true, true, false},
                                            {"OpSLessThan", 177, true, true, false},
                                            {"OpULessThanEqual", 178, true, true, false},
                                            {"OpSLessThanEqual", 179, true, true, false},
                                            {"OpFOrdEqual", 180, true, true, false},
                                            {"OpFUnordEqual", 181, true, true, false},
                                            {"OpFOrdNotEqual", 182, true, true, false},
                                            {"OpFUnordNotEqual", 183, true, true, false},
                                            {"OpFOrdLessThan", 184, true, true, false},
                                            {"OpFUnordLessThan", 185, true, true, false},
                                            {"OpFOrdGreaterThan", 186, true, true, false},
                                            {"OpFUnordGreaterThan", 187, true, true, false},
                                            {"OpFOrdLessThanEqual", 188, true, true, false},
                                            {"OpFUnordLessThanEqual", 189, true, true, false},
                                            {"OpFOrdGreaterThanEqual", 190, true, true, false},
                                            {"OpFUnordGreaterThanEqual", 191, true, true, false},
                                            {"OpShiftRightLogical", 194, true, true, false},
                                            {"OpShiftRightArithmetic", 195, true, true, false},
                                            {"OpShiftLeftLogical", 196, true, true, false},
                                            {"OpBitwiseOr", 197, true, true, false},
                                            {"OpBitwiseXor", 198, true, true, false},
                                            {"OpBitwiseAnd", 199, true, true, false},
                                            {"OpNot", 200, true, true, false},
                                            {"OpBitFieldInsert", 201, true, true, false},
                                            {"OpBitFieldSExtract", 202, true, true, false},
                                            {"OpBitFieldUExtract", 203, true, true, false},
                                            {"OpBitReverse", 204, true, true, false},
                                            {"OpBitCount", 205, true, true, false},
                                            {"OpDPdx", 207, true, true, false},
                                            {"OpDPdy", 208, true, true, false},
                                            {"OpFwidth", 209, true, true, false},
                                            {"OpDPdxFine", 210, true, true, false},
                                            {"OpDPdyFine", 211, true, true, false},
                                            {"OpFwidthFine", 212, true, true, false},
                                            {"OpDPdxCoarse", 213, true, true, false},
                                            {"OpDPdyCoarse", 214, true, true, false},
                                            {"OpFwidthCoarse", 215, true, true, false},
                                            {"OpEmitVertex", 218, false, false, false},
                                            {"OpEndPrimitive", 219, false, false, false},
                                            {"OpEmitStreamVertex", 220, false, false, false},
                                            {"OpEndStreamPrimitive", 221, false, false, false},
                                            {"OpControlBarrier", 224, false, false, false},
                                            {"OpMemoryBarrier", 225, false, false, false},
                                            {"OpAtomicLoad", 227, true, true, false},
                                            {"OpAtomicStore", 228, false, false, false},
                                            {"OpAtomicExchange", 229, true, true, false},
                                            {"OpAtomicCompareExchange", 230, true, true, false},
                                            {"OpAtomicCompareExchangeWeak", 231, true, true, false},
                                            {"OpAtomicIIncrement", 232, true, true, false},
                                            {"OpAtomicIDecrement", 233, true, true, false},
                                            {"OpAtomicIAdd", 234, true, true, false},
                                            {"OpAtomicISub", 235, true, true, false},
                                            {"OpAtomicSMin", 236, true, true, false},
                                            {"OpAtomicUMin", 237, true, true, false},
                                            {"OpAtomicSMax", 238, true, true, false},
                                            {"OpAtomicUMax", 239, true, true, false},
                                            {"OpAtomicAnd", 240, true, true, false},
                                            {"OpAtomicOr", 241, true, true, false},
                                            {"OpAtomicXor", 242, true, true, false},
                                            {"OpPhi", 245, true, true, false},
                                            {"OpLoopMerge", 246, false, false, false},
                                            {"OpSelectionMerge", 247, false, false, false},
                                            {"OpLabel", 248, true, false, false},
                                            {"OpBranch", 249, false, false, false},
                                            {"OpBranchConditional", 250, false, false, false},
                                            {"OpSwitch", 251, false, false, false},
                                            {"OpKill", 252, false, false, false},
                                            {"OpReturn", 253, false, false, false},
                                            {"OpReturnValue", 254, false, false, false},
                                            {"OpUnreachable", 255, false, false, false},
                                            {"OpLifetimeStart", 256, false, false, false},
                                            {"OpLifetimeStop", 257, false, false, false},
                                            {"OpGroupAsyncCopy", 259, true, true, false},
                                            {"OpGroupWaitEvents", 260, false, false, false},
                                            {"OpGroupAll", 261, true, true, false},
                                            {"OpGroupAny", 262, true, true, false},
                                            {"OpGroupBroadcast", 263, true, true, false},
                                            {"OpGroupIAdd", 264, true, true, false},
                                            {"OpGroupFAdd", 265, true, true, false},
                                            {"OpGroupFMin", 266, true, true, false},
                                            {"OpGroupUMin", 267, true, true, false},
                                            {"OpGroupSMin", 268, true, true, false},
                                            {"OpGroupFMax", 269, true, true, false},
                                            {"OpGroupUMax", 270, true, true, false},
                                            {"OpGroupSMax", 271, true, true, false},
                                            {"OpReadPipe", 274, true, true, false},
                                            {"OpWritePipe", 275, true, true, false},
                                            {"OpReservedReadPipe", 276, true, true, false},
                                            {"OpReservedWritePipe", 277, true, true, false},
                                            {"OpReserveReadPipePackets", 278, true, true, false},
                                            {"OpReserveWritePipePackets", 279, true, true, false},
                                            {"OpCommitReadPipe", 280, false, false, false},
                                            {"OpCommitWritePipe", 281, false, false, false},
                                            {"OpIsValidReserveId", 282, true, true, false},
                                            {"OpGetNumPipePackets", 283, true, true, false},
                                            {"OpGetMaxPipePackets", 284, true, true, false},
                                            {"OpGroupReserveReadPipePackets", 285, true, true, false},
                                            {"OpGroupReserveWritePipePackets", 286, true, true, false},
                                            {"OpGroupCommitReadPipe", 287, false, false, false},
                                            {"OpGroupCommitWritePipe", 288, false, false, false},
                                            {"OpEnqueueMarker", 291, true, true, false},
                                            {"OpEnqueueKernel", 292, true, true, false},
                                            {"OpGetKernelNDrangeSubGroupCount", 293, true, true, false},
                                            {"OpGetKernelNDrangeMaxSubGroupSize", 294, true, true, false},
                                            {"OpGetKernelWorkGroupSize", 295, true, true, false},
                                            {"OpGetKernelPreferredWorkGroupSizeMultiple", 296, true, true, false},
                                            {"OpRetainEvent", 297, false, false, false},
                                            {"OpReleaseEvent", 298, false, false, false},
                                            {"OpCreateUserEvent", 299, true, true, false},
                                            {"OpIsValidEvent", 300, true, true, false},
                                            {"OpSetUserEventStatus", 301, false, false, false},
                                            {"OpCaptureEventProfilingInfo", 302, false, false, false},
                                            {"OpGetDefaultQueue", 303, true, true, false},
                                            {"OpBuildNDRange", 304, true, true, false},
                                            {"OpImageSparseSampleImplicitLod", 305, true, true, false},
                                            {"OpImageSparseSampleExplicitLod", 306, true, true, false},
                                            {"OpImageSparseSampleDrefImplicitLod", 307, true, true, false},
                                            {"OpImageSparseSampleDrefExplicitLod", 308, true, true, false},
                                            {"OpImageSparseSampleProjImplicitLod", 309, true, true, false},
                                            {"OpImageSparseSampleProjExplicitLod", 310, true, true, false},
                                            {"OpImageSparseSampleProjDrefImplicitLod", 311, true, true, false},
                                            {"OpImageSparseSampleProjDrefExplicitLod", 312, true, true, false},
                                            {"OpImageSparseFetch", 313, true, true, false},
                                            {"OpImageSparseGather", 314, true, true, false},
                                            {"OpImageSparseDrefGather", 315, true, true, false},
                                            {"OpImageSparseTexelsResident", 316, true, true, false},
                                            {"OpNoLine", 317, false, false, false},
                                            {"OpAtomicFlagTestAndSet", 318, true, true, false},
                                            {"OpAtomicFlagClear", 319, false, false, false},
                                            {"OpImageSparseRead", 320, true, true, false},
                                            {"OpSizeOf", 321, true, true, false},
                                            {"OpTypePipeStorage", 322, true, false, false},
                                            {"OpConstantPipeStorage", 323, true, true, false},
                                            {"OpCreatePipeFromPipeStorage", 324, true, true, false},
                                            {"OpGetKernelLocalSizeForSubgroupCount", 325, true, true, false},
                                            {"OpGetKernelMaxNumSubgroups", 326, true, true, false},
                                            {"OpTypeNamedBarrier", 327, true, false, false},
                                            {"OpNamedBarrierInitialize", 328, true, true, false},
                                            {"OpMemoryNamedBarrier", 329, false, false, false},
                                            {"OpModuleProcessed", 330, false, false, false},
                                            {"OpExecutionModeId", 331, false, false, false},
                                            {"OpDecorateId", 332, false, false, false},
                                            {"OpSubgroupBallotKHR", 4421, true, true, false},
                                            {"OpSubgroupFirstInvocationKHR", 4422, true, true, false},
                                            {"OpSubgroupReadInvocationKHR", 4432, true, true, false},
                                            {"OpGroupIAddNonUniformAMD", 5000, true, true, false},
                                            {"OpGroupFAddNonUniformAMD", 5001, true, true, false},
                                            {"OpGroupFMinNonUniformAMD", 5002, true, true, false},
                                            {"OpGroupUMinNonUniformAMD", 5003, true, true, false},
                                            {"OpGroupSMinNonUniformAMD", 5004, true, true, false},
                                            {"OpGroupFMaxNonUniformAMD", 5005, true, true, false},
                                            {"OpGroupUMaxNonUniformAMD", 5006, true, true, false},
                                            {"OpGroupSMaxNonUniformAMD", 5007, true, true, false}};
#pragma GCC diagnostic pop
static const SPIRVOpCode OP_CODE_NULL{};
const SPIRVOpCode& FindOpCode( uint32_t code )
{
  for( auto&& opcode : OP_CODE_ARRAY )
  {
    if( opcode.code == code )
      return opcode;
  }
  return OP_CODE_NULL;
}

} // namespace
} // namespace SpirV
} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif //DALI_GRAPHICS_VULKAN_SPIRV_SPIRV_OPCODES_H
