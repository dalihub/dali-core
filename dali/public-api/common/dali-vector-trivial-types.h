#ifndef DALI_VECTOR_TRIVIAL_TYPE_H
#define DALI_VECTOR_TRIVIAL_TYPE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector-base.h>

namespace DALI_NAMESPACE
{
/// @cond internal
/**
 * @brief Vector algorithm variant for trivial types.
 *
 * Trivial types do not need destructor or copy constructor called, so the
 * library copies them with memcpy/memmove: every element operation is null.
 * @SINCE_1_0.0
 */
template<bool IsTrivial, typename T>
class VectorAlgorithms : public VectorBase
{
protected: // API for deriving classes
  using SizeType = VectorBase::SizeType;

  /**
   * @brief Empty constructor.
   * @SINCE_1_0.0
   */
  VectorAlgorithms() = default;

  /**
   * @brief Empty destructor.
   * @SINCE_1_0.0
   */
  ~VectorAlgorithms() = default;

  /**
   * @brief Element operations for the paths that copy elements.
   * @SINCE_3_0.1
   */
  static constexpr const ElementOperations& CopyOperations()
  {
    return OPERATIONS;
  }

  /**
   * @brief Element operations for the paths that only move or destroy elements.
   * @SINCE_3_0.1
   */
  static constexpr const ElementOperations& MoveOperations()
  {
    return OPERATIONS;
  }

private:
  static constexpr ElementOperations OPERATIONS{sizeof(T), nullptr, nullptr, nullptr};
};
/// @endcond

} //namespace DALI_NAMESPACE

#endif // DALI_VECTOR_TRIVIAL_TYPE_H
