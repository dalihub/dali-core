#ifndef DALI_VECTOR_COMPLEX_TYPE_H
#define DALI_VECTOR_COMPLEX_TYPE_H

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
#include <new> // placement new

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector-base.h>
#include <dali/public-api/common/dali-vector-trivial-types.h>

namespace DALI_NAMESPACE
{
/// @cond internal
/**
 * @brief Vector algorithm variant for complex types.
 *
 * Complex types (types with non-trivial constructors/destructors) are reached
 * from the library through the copy/move constructor and destructor of T. The
 * algorithms themselves live in VectorBase; only these three per-element
 * primitives are compiled into the application.
 *
 * @SINCE_2_5.10
 */
template<typename T>
class VectorAlgorithms<false, T> : public VectorBase
{
protected: // API for deriving classes
  using SizeType = VectorBase::SizeType;

  /**
   * @brief Empty constructor.
   * @SINCE_2_5.10
   */
  VectorAlgorithms() = default;

  /**
   * @brief Empty destructor.
   * @SINCE_2_5.10
   */
  ~VectorAlgorithms() = default;

  /**
   * @brief Element operations for the paths that copy elements.
   *
   * Only the copying paths instantiate this, so a move-only T still compiles.
   * @SINCE_3_0.1
   */
  static const ElementOperations& CopyOperations()
  {
    static constexpr ElementOperations operations{sizeof(T), &CopyConstruct, &MoveConstruct, &Destroy};
    return operations;
  }

  /**
   * @brief Element operations for the paths that only move or destroy elements.
   * @SINCE_3_0.1
   */
  static const ElementOperations& MoveOperations()
  {
    static constexpr ElementOperations operations{sizeof(T), nullptr, &MoveConstruct, &Destroy};
    return operations;
  }

private:
  static void CopyConstruct(void* destination, const void* source)
  {
    new(destination) T(*static_cast<const T*>(source));
  }

  static void MoveConstruct(void* destination, void* source)
  {
    new(destination) T(static_cast<T&&>(*static_cast<T*>(source)));
  }

  static void Destroy(void* element)
  {
    static_cast<T*>(element)->~T();
  }
};
/// @endcond

} //namespace DALI_NAMESPACE

#endif // DALI_VECTOR_COMPLEX_TYPE_H
