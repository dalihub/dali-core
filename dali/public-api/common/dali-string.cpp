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
 */

// CLASS INCLUDE
#include <dali/public-api/common/dali-string.h>

// EXTERNAL INCLUDES
#include <new>
#include <string>

static_assert(sizeof(std::string) <= 32,
              "std::string exceeds Dali::String inline storage (32 bytes). "
              "Implementation must be updated to use heap allocation.");
static_assert(alignof(std::string) <= 8,
              "std::string alignment exceeds Dali::String inline storage alignment (8).");

namespace
{

inline std::string& GetString(void* storage)
{
  return *static_cast<std::string*>(storage);
}

inline const std::string& GetString(const void* storage)
{
  return *static_cast<const std::string*>(storage);
}

} // unnamed namespace

namespace Dali
{

String::String()
{
  // Use placement new to create string.
  new(mStorage) std::string();
}

String::String(const char* str)
{
  if(str)
  {
    new(mStorage) std::string(str);
  }
  else
  {
    new(mStorage) std::string();
  }
}

String::~String()
{
  GetString(mStorage).~basic_string();
}

String::String(const String& other)
{
  new(mStorage) std::string(GetString(other.mStorage));
}

String& String::operator=(const String& other)
{
  GetString(mStorage) = GetString(other.mStorage);
  return *this;
}

String::String(String&& other) noexcept
{
  new(mStorage) std::string(std::move(GetString(other.mStorage)));
}

String& String::operator=(String&& other) noexcept
{
  GetString(mStorage) = std::move(GetString(other.mStorage));
  return *this;
}

// Basic Operations
size_t String::Size() const
{
  return GetString(mStorage).size();
}

bool String::Empty() const
{
  return GetString(mStorage).empty();
}

const char* String::CStr() const
{
  return GetString(mStorage).c_str();
}

char& String::operator[](size_t position)
{
  return GetString(mStorage)[position];
}

const char& String::operator[](size_t position) const
{
  return GetString(mStorage)[position];
}

String& String::operator+=(char character)
{
  GetString(mStorage) += character;
  return *this;
}

String& String::operator+=(const String& rhs)
{
  GetString(mStorage) += GetString(rhs.mStorage);
  return *this;
}

// Comparison operators
bool String::operator==(const String& rhs) const
{
  return GetString(mStorage) == GetString(rhs.mStorage);
}

bool String::operator!=(const String& rhs) const
{
  return GetString(mStorage) != GetString(rhs.mStorage);
}

bool String::operator<(const String& rhs) const
{
  return GetString(mStorage) < GetString(rhs.mStorage);
}

bool String::operator>(const String& rhs) const
{
  return GetString(mStorage) > GetString(rhs.mStorage);
}

bool String::operator<=(const String& rhs) const
{
  return GetString(mStorage) <= GetString(rhs.mStorage);
}

bool String::operator>=(const String& rhs) const
{
  return GetString(mStorage) >= GetString(rhs.mStorage);
}

} // namespace Dali
