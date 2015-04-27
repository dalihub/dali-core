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
#include <dali/public-api/animation/path.h>
#include <dali/internal/event/animation/path-impl.h>

// INTERNAL INCLUDES

namespace Dali
{

Path Path::New()
{
  Internal::Path* internal = Internal::Path::New();
  return Path(internal);
}

Path Path::DownCast( BaseHandle handle )
{
  return Path( dynamic_cast<Dali::Internal::Path*>(handle.GetObjectPtr()) );
}

Path::Path()
{
}

Path::~Path()
{
}

Path::Path(const Path& handle)
:Handle(handle)
{
}

Path::Path(Internal::Path* internal)
: Handle(internal)
{
}

Path& Path::operator=(const Path& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void Path::AddPoint(const Vector3& point )
{
  GetImplementation(*this).AddPoint( point );
}

void Path::AddControlPoint(const Vector3& point )
{
  GetImplementation(*this).AddControlPoint( point );
}

void Path::GenerateControlPoints( float curvature )
{
  GetImplementation(*this).GenerateControlPoints( curvature );
}


void Path::Sample( float progress, Vector3& position, Vector3& tangent ) const
{
  GetImplementation(*this).Sample( progress, position, tangent );
}

Vector3& Path::GetPoint( size_t index )
{
  return GetImplementation(*this).GetPoint(index);
}

Vector3& Path::GetControlPoint( size_t index )
{
  return GetImplementation(*this).GetControlPoint(index);
}

size_t Path::GetPointCount() const
{
  return GetImplementation(*this).GetPointCount();
}

} // Dali
