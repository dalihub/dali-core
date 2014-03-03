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

// CLASS HEADER
#include <dali/internal/event/text/utf8-impl.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

size_t UTF8SequenceLength(const unsigned char leadByte)
{
  size_t length = 0;

  if ((leadByte & 0x80) == 0 )          //ASCII character (lead bit zero)
  {
    length = 1;
  }
  else if (( leadByte & 0xe0 ) == 0xc0 ) //110x xxxx
  {
    length = 2;
  }
  else if (( leadByte & 0xf0 ) == 0xe0 ) //1110 xxxx
  {
    length = 3;
  }
  else if (( leadByte & 0xf8 ) == 0xf0 ) //1111 0xxx
  {
    length = 4;
  }
  else
  {
    DALI_LOG_WARNING("Unrecognized lead byte  %c\n", leadByte);
  }

  return length;
}

uint32_t UTF8Read(const unsigned char* utf8Data, const size_t sequenceLength)
{
  uint32_t code = 0;

  if (sequenceLength == 1)
  {
    code = *utf8Data;
  }
  else if (sequenceLength == 2)
  {
    code = *utf8Data++ & 0x1f;
    code <<= 6;
    code |= *utf8Data  & 0x3f;
  }
  else if (sequenceLength == 3)
  {
    code =  *utf8Data++ & 0x0f;
    code <<= 6;
    code |= *utf8Data++ & 0x3f;
    code <<= 6;
    code |= *utf8Data   & 0x3f;
  }
  else if (sequenceLength == 4)
  {
    code =  *utf8Data++ & 0x07;
    code <<= 6;
    code |= *utf8Data++ & 0x3f;
    code <<= 6;
    code |= *utf8Data++ & 0x3f;
    code <<= 6;
    code |= *utf8Data   & 0x3f;
  }

  return code;
}

size_t UTF8Write(const uint32_t code, unsigned char* utf8Data)
{
  size_t sequenceLength = 0;
  if (code < 0x80u)
  {
    *utf8Data = code;
    sequenceLength = 1;
  }
  else if (code < 0x800u)
  {
    *utf8Data++ = static_cast<uint8_t>( code >> 6)          | 0xc0; // lead byte for 2 byte sequence
    *utf8Data   = static_cast<uint8_t>( code        & 0x3f) | 0x80; // continuation byte
    sequenceLength = 2;
  }
  else if (code < 0x10000u)
  {
    *utf8Data++ = static_cast<uint8_t>( code >> 12)         | 0xe0; // lead byte for 2 byte sequence
    *utf8Data++ = static_cast<uint8_t>((code >> 6)  & 0x3f) | 0x80; // continuation byte
    *utf8Data   = static_cast<uint8_t>( code        & 0x3f) | 0x80; // continuation byte
    sequenceLength = 3;
  }
  else if (code < 0x200000u)
  {
    *utf8Data++ = static_cast<uint8_t>( code >> 18)         | 0xf0; // lead byte for 2 byte sequence
    *utf8Data++ = static_cast<uint8_t>((code >> 12) & 0x3f) | 0x80; // continuation byte
    *utf8Data++ = static_cast<uint8_t>((code >> 6)  & 0x3f) | 0x80; // continuation byte
    *utf8Data   = static_cast<uint8_t>( code        & 0x3f) | 0x80; // continuation byte
    sequenceLength = 4;
  }

  return sequenceLength;
}

size_t UTF8Tokenize(const unsigned char* utf8Data, const size_t utf8DataLength, TextArray& tokens)
{
  size_t dataLength = utf8DataLength;

  while (dataLength)
  {
    uint32_t code;
    size_t sequenceLength = UTF8SequenceLength(*utf8Data);

    if (!sequenceLength)
    {
      break;
    }

    if (sequenceLength > dataLength)
    {
      // utf8 data error
      break;
    }

    code = UTF8Read(utf8Data, sequenceLength);
    tokens.push_back(code);

    utf8Data += sequenceLength;
    dataLength -= sequenceLength;
  }
  return tokens.size();
}

} // namespace Internal

} // namespace Dali
