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
#include <dali/public-api/object/any.h>

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

namespace Dali
{

Any::Any()
: mContainer( NULL )
{
}

Any::~Any()
{
  // Call the implementation deletion function, which will invalidate mContainer

  if ( NULL != mContainer )
  {
    mContainer->mDeleteFunc( mContainer );
    mContainer = NULL;
  }
}

Any& Any::operator=( const Any& any )
{
  if( &any != this )
  {
    if( NULL == any.mContainer )
    {
      delete mContainer;
      mContainer = NULL;
    }
    else
    {
      AnyContainerBase* tmp = mContainer;

      if( NULL != mContainer )
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
