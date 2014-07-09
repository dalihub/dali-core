#ifndef __DALI_INTERNAL_UTF8_H__
#define __DALI_INTERNAL_UTF8_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/integration-api/text-array.h>

namespace Dali
{

namespace Internal
{

/**
 * Determine the length (in bytes) of a UTF-8 character
 * @param[in] leadByte The lead byte of a UTF-8 character sequence
 * @return The length of the sequence, or zero if the UTF-8 character is invalid.
 */
size_t UTF8SequenceLength(const unsigned char leadByte);

/**
 * Converts a UTF-8 character into a UTF-32 code
 * @param[in] utf8Data       A pointer to the lead byte of the UTF-8 character
 * @param[in] sequenceLength The length of the UTF-8 character. See UTF8SequenceLength.
 * @return The UTF-32 code, (or zero if the sequenceLength is not between 1..4
 */
uint32_t UTF8Read(const unsigned char* utf8Data, const size_t sequenceLength);

/**
 * Converts a UTF-32 code into a UTF-8 sequence
 * @param[in]  code     The UTF-32 code
 * @param[out] utf8Data The UTF-8 buffer that receives the sequence
 * @return The length of the sequence written to utf8Data, or zero if the code was invalid
 */
size_t UTF8Write(const uint32_t code, unsigned char* utf8Data);

/**
 * Converts a stream of UTF-8 codes into an aarray of UTF-32 codes
 * @param[in]  utf8Data       The UTF-8 buffer containing the UTF-8 string
 * @param[in]  utf8DataLength The size of the data, in bytes, at utf8Data
 * @param[out] tokens         A vector which will receive the converted UTF-32 codes
 * @return The number of UTF-32 codes.
 */
size_t UTF8Tokenize(const unsigned char* utf8Data, const size_t utf8DataLength, Integration::TextArray& tokens);

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UTF8_H__

