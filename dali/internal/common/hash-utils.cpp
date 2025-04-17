/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// CLASS HEADERS
#include <dali/internal/common/hash-utils.h>

// EXTERNAL INCLUDES
#include <cstdint> // uint8_t

namespace Dali::Internal::HashUtils
{
HashType HashRawByteBufferMultipleComponent(const uint8_t* __restrict__ buffer, std::size_t bufferSize, HashType& hash)
{
  // TODO : use ARM_NEON here in future
  static constexpr HashType SCALE_LEVEL_1 = Dali::Power<33, 1>::value;
  static constexpr HashType SCALE_LEVEL_2 = Dali::Power<33, 2>::value;
  static constexpr HashType SCALE_LEVEL_3 = Dali::Power<33, 3>::value;
  static constexpr HashType SCALE_LEVEL_4 = Dali::Power<33, 4>::value;
  static constexpr HashType SCALE_LEVEL_5 = Dali::Power<33, 5>::value;
  static constexpr HashType SCALE_LEVEL_6 = Dali::Power<33, 6>::value;
  static constexpr HashType SCALE_LEVEL_7 = Dali::Power<33, 7>::value;
  static constexpr HashType SCALE_LEVEL_8 = Dali::Power<33, 8>::value;

  while(bufferSize & (~0x7)) // bufferSize >= 8
  {
    // clang-format off
    hash = hash * SCALE_LEVEL_8 +
           *(buffer + 0) * SCALE_LEVEL_7 +
           *(buffer + 1) * SCALE_LEVEL_6 +
           *(buffer + 2) * SCALE_LEVEL_5 +
           *(buffer + 3) * SCALE_LEVEL_4 +
           *(buffer + 4) * SCALE_LEVEL_3 +
           *(buffer + 5) * SCALE_LEVEL_2 +
           *(buffer + 6) * SCALE_LEVEL_1 +
           *(buffer + 7);
    // clang-format on
    buffer += 8;
    bufferSize -= 8;
  }

  if(bufferSize & 0x4) // bufferSize >= 4. Note that 0 <= bufferSize < 8 in here.
  {
    // clang-format off
    hash = hash * SCALE_LEVEL_4 +
           *(buffer + 0) * SCALE_LEVEL_3 +
           *(buffer + 1) * SCALE_LEVEL_2 +
           *(buffer + 2) * SCALE_LEVEL_1 +
           *(buffer + 3);
    // clang-format on
    buffer += 4;
    bufferSize &= 0x3;
  }

  // Now bufferSize is 0, 1, 2, 3. We can optimize here by switch-case
  switch(bufferSize)
  {
    case 3:
    {
      hash = hash * 33 + *(buffer++);
      DALI_FALLTHROUGH;
    }
    case 2:
    {
      hash = hash * 33 + *(buffer++);
      DALI_FALLTHROUGH;
    }
    case 1:
    {
      hash = hash * 33 + *(buffer++);
      DALI_FALLTHROUGH;
    }
    case 0:
    default:
    {
      break;
    }
  }

  return hash;
}

} // namespace Dali::Internal::HashUtils
