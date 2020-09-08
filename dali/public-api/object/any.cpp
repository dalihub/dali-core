/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace Dali
{

Any::Any()
: mContainer( nullptr )
{
}

Any::~Any()
{
  // Call the implementation deletion function, which will invalidate mContainer

  if ( nullptr != mContainer )
  {
    mContainer->mDeleteFunc( mContainer );
    mContainer = nullptr;
  }
}

Any& Any::operator=( const Any& any )
{
  if( &any != this )
  {
    if( nullptr == any.mContainer )
    {
      delete mContainer;
      mContainer = nullptr;
    }
    else
    {
      AnyContainerBase* tmp = mContainer;

      if( nullptr != mContainer )
      {
        // Check if two Any types have the same type. Avoids assignments of values with different types.
        if( mContainer->GetType() != any.GetType() )
        {
          AssertAlways( "Any::operator=( const Any& Any ). Trying to assign two values with different types." );
        }
      }

      // Clone the correct templated object
      mContainer = any.mContainer->mCloneFunc( *any.mContainer );

      // Deletes previous container.
      delete tmp;
    }
  }

  return *this;
}

const std::type_info& Any::GetType() const
{
  return mContainer ? mContainer->GetType() : typeid( void );
}

void Any::AssertAlways( const char* assertMessage )
{
  DALI_LOG_ERROR_NOFN( assertMessage );
  throw Dali::DaliException( assertMessage, "" );
}


} //namespace Dali
