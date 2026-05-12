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

// CLASS HEADER
#include <dali/public-api/object/any.h>

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <dali/internal/event/common/demangler.h>

namespace Dali
{
Any::Any()
: mContainer(nullptr)
{
}

Any::~Any()
{
  // Call the implementation deletion function, which will invalidate mContainer

  if(nullptr != mContainer)
  {
    mContainer->mDeleteFunc(mContainer);
    mContainer = nullptr;
  }
}

Any& Any::operator=(const Any& any)
{
  if(&any != this)
  {
    if(nullptr == any.mContainer)
    {
      if(mContainer)
      {
        mContainer->mDeleteFunc(mContainer);
        mContainer = nullptr;
      }
    }
    else
    {
      AnyContainerBase* tmp = mContainer;

      if(nullptr != mContainer)
      {
        // Check if two Any types have the same type. Avoids assignments of values with different types.
        if(mContainer->GetType() != any.GetType())
        {
          AssertAlways("Any::operator=( const Any& Any ). Trying to assign two values with different types.");
        }
      }

      // Clone the correct templated object
      mContainer = any.mContainer->mCloneFunc(*any.mContainer);

      // Deletes previous container.
      if(tmp)
      {
        tmp->mDeleteFunc(tmp);
      }
    }
  }

  return *this;
}

Any& Any::operator=(Any&& any) noexcept
{
  if(&any != this)
  {
    // Deletes previous container.
    if(mContainer)
    {
      mContainer->mDeleteFunc(mContainer);
    }

    // Move the correct templated object
    mContainer = any.mContainer;

    // Remove input value's container.
    any.mContainer = nullptr;
  }

  return *this;
}

const TypeInfoId& Any::GetType() const
{
  static const TypeInfoId voidType;
  return mContainer ? mContainer->GetType() : voidType;
}

Dali::String Any::DemangleTypeName(const char* mangledName)
{
  if(!mangledName || mangledName[0] == '\0')
  {
    return Dali::String();
  }

  // DemangleTypeInfoName returns a std::string. Capture it in a named local
  // variable before calling c_str() — calling c_str() on a temporary would
  // be undefined behaviour because the temporary is destroyed before the
  // Dali::String constructor completes.
  std::string demangled = Dali::Internal::DemangleTypeInfoName(mangledName);
  return Dali::String(demangled.c_str());
}

void Any::AssertAlways(const char* assertMessage)
{
  DALI_LOG_ERROR_NOFN(assertMessage);
  throw Dali::DaliException(assertMessage, "");
}

} // namespace Dali
