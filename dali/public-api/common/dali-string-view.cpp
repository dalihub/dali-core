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
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>

// EXTERNAL INCLUDES
#include <new>
#include <string_view>

static_assert(sizeof(std::string_view) <= 16,
              "std::string_view exceeds Dali::StringView inline storage (16 bytes). "
              "Implementation must be updated.");
static_assert(alignof(std::string_view) <= 8,
              "std::string_view alignment exceeds Dali::StringView inline storage alignment (8).");

namespace
{

inline std::string_view& GetView(void* storage)
{
  return *static_cast<std::string_view*>(storage);
}

inline const std::string_view& GetView(const void* storage)
{
  return *static_cast<const std::string_view*>(storage);
}

} // unnamed namespace

namespace Dali
{

StringView::StringView()
{
  new(mStorage) std::string_view();
}

StringView::StringView(const char* str)
{
  if(str)
  {
    new(mStorage) std::string_view(str);
  }
  else
  {
    new(mStorage) std::string_view();
  }
}

StringView::StringView(const char* str, size_t length)
{
  new(mStorage) std::string_view(str, length);
}

StringView::StringView(const String& str)
{
  const char* cstr = str.CStr();
  if(cstr)
  {
    new(mStorage) std::string_view(cstr, str.Size());
  }
  else
  {
    new(mStorage) std::string_view();
  }
}

StringView::~StringView()
{
  GetView(mStorage).~basic_string_view();
}

StringView::StringView(const StringView& other)
{
  new(mStorage) std::string_view(GetView(other.mStorage));
}

StringView& StringView::operator=(const StringView& other)
{
  GetView(mStorage) = GetView(other.mStorage);
  return *this;
}

// Basic Operations
size_t StringView::Size() const
{
  return GetView(mStorage).size();
}

bool StringView::Empty() const
{
  return GetView(mStorage).empty();
}

const char* StringView::Data() const
{
  return GetView(mStorage).data();
}

bool StringView::operator==(const StringView& rhs) const
{
  return GetView(mStorage) == GetView(rhs.mStorage);
}

bool StringView::operator==(const char* rhs) const
{
  return GetView(mStorage) == rhs;
}

bool StringView::operator==(const String& rhs) const
{
  const char* cstr = rhs.CStr();
  return GetView(mStorage) == (cstr ? std::string_view(cstr, rhs.Size()) : std::string_view());
}

} // namespace Dali
