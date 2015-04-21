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
#include <dali/public-api/geometry/cloth.h>

// INTERNAL INCLUDES
#include <dali/public-api/geometry/mesh-factory.h>
#include <dali/internal/event/modeling/cloth-impl.h>

namespace Dali
{

namespace
{

} // namespace

Cloth::Cloth()
{
}

Cloth::~Cloth()
{
}

Cloth::Cloth(const Cloth& handle)
: Mesh(handle)
{
}

Cloth& Cloth::operator=(const Cloth& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Cloth Cloth::New( const float width, const float height, const int xSteps, const int ySteps, const Rect<float>& textureCoordinates )
{
  MeshData meshData( MeshFactory::NewPlane(width, height, xSteps, ySteps, textureCoordinates) );

  Internal::ClothIPtr clothPtr = Internal::Cloth::New( meshData );
  return Cloth( clothPtr.Get() );
}

Cloth Cloth::DownCast( BaseHandle handle )
{
  return Cloth( dynamic_cast<Dali::Internal::Cloth*>(handle.GetObjectPtr()) );
}

Cloth::Cloth(Internal::Cloth* internal)
: Mesh(internal)
{
}

} // namespace Dali
