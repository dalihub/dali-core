#ifndef __DALI_UTF8_H__
#define __DALI_UTF8_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{
/**
 * @brief Determine the length (in bytes) of a UTF-8 character.
 *
 * @param[in] leadByte The lead byte of a UTF-8 character sequence
 * @return The length of the sequence, or zero if the UTF-8 character is invalid.
 */
DALI_IMPORT_API size_t Utf8SequenceLength(const unsigned char leadByte);

} // namespace Dali


#endif // __DALI_UTF8_H__
