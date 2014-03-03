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
#include <dali/internal/event/common/projection.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector4.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

bool Unproject( const Vector4& windowPos,
                const Matrix& inverseMvp,
                float viewportWidth,
                float viewportHeight,
                Vector4& objectPos )
{
  objectPos.x = windowPos.x;
  objectPos.y = windowPos.y;
  objectPos.z = windowPos.z;
  objectPos.w = 1.0f;

  objectPos.x = objectPos.x / viewportWidth;
  objectPos.y = objectPos.y / viewportHeight;

  objectPos.x = objectPos.x * 2.0f - 1.0f;
  objectPos.y = objectPos.y * 2.0f - 1.0f;
  objectPos.z = objectPos.z * 2.0f - 1.0f;

  objectPos = inverseMvp * objectPos;

  // In the case where objectPos.w is exactly zero, the unproject fails
  if ( EqualsZero( objectPos.w ) )
  {
    return false;
  }

  objectPos.x /= objectPos.w;
  objectPos.y /= objectPos.w;
  objectPos.z /= objectPos.w;

  return true;
}

bool UnprojectFull( const Vector4& windowPos,
                    const Matrix& modelView,
                    const Matrix& projection,
                    float viewportWidth,
                    float viewportHeight,
                    Vector4& objectPos )
{
  Matrix invertedMvp( false ); // Don't initialize.
  Matrix::Multiply( invertedMvp, modelView, projection );

  if (invertedMvp.Invert())
  {
    return Unproject( windowPos, invertedMvp, viewportWidth, viewportHeight, objectPos );
  }

  return false;
}

bool XyPlaneIntersect( const Vector4& pointA, const Vector4& pointB, Vector4& intersect )
{
  const Vector4* near = NULL;
  const Vector4* far = NULL;

  if ( true || (pointA.z > 0.0f && pointB.z < 0.0f ))
  {
    near = &pointA;
    far  = &pointB;
  }
  else if ( pointB.z > 0.0f && pointA.z < 0.0f )
  {
    near = &pointB;
    far  = &pointA;
  }
  else
  {
    return false; // ray does not cross xy plane
  }

  float dist = near->z / (near->z - far->z);

  intersect.x = near->x + (far->x - near->x) * dist;
  intersect.y = near->y + (far->y - near->y) * dist;
  intersect.z = 0.0f;

  return true;
}

} // namespace Internal

} // namespace Dali
