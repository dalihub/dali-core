/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/ray-test.h>

// EXTERNAL INCLUDES
#include <algorithm> ///< for std::min, std::max

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

using Dali::Internal::SceneGraph::Node;

namespace
{
constexpr float RAY_TEST_ABSOLUTE_EPSILON = Dali::Epsilon<10>::value;   ///< 0.0000011920928955078125
constexpr float RAY_TEST_RELATIVE_EPSILON = Dali::Epsilon<1000>::value; ///< 0.00011920928955078125

/**
 * @brief Get the epsilon value that we allow to test.
 * It will be used when some numeric error occured.
 *
 * @param targetScale Scale of target value
 * @return The epsilon value for target scale touch case.
 */
constexpr float GetEpsilon(const float targetScale)
{
  return std::max(RAY_TEST_ABSOLUTE_EPSILON, RAY_TEST_RELATIVE_EPSILON * targetScale);
}
} // namespace

namespace Dali
{
namespace Internal
{
RayTest::RayTest()
{
}

bool RayTest::SphereTest(const Internal::Actor& actor, const Vector4& rayOrigin, const Vector4& rayDir) const
{
  /*
   http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection

   Mathematical Formulation

   Given the above mentioned sphere, a point 'p' lies on the surface of the sphere if

   ( p - c ) dot ( p - c ) = r^2

   Given a ray with a point of origin 'o', and a direction vector 'd':

   ray(t) = o + td, t >= 0

   we can find the t at which the ray intersects the sphere by setting ray(t) equal to 'p'

   (o + td - c ) dot ( o + td - c ) = r^2

   To solve for t we first expand the above into a more recognisable quadratic equation form

   ( d dot d )t^2 + 2( o - c ) dot dt + ( o - c ) dot ( o - c ) - r^2 = 0

   or

   At2 + Bt + C = 0

   where

   A = d dot d
   B = 2( o - c ) dot d
   C = ( o - c ) dot ( o - c ) - r^2

   which can be solved using a standard quadratic formula.

   Note that in the absence of positive, real, roots, the ray does not intersect the sphere.

   Practical Simplification

   In a renderer, we often differentiate between world space and object space. In the object space
   of a sphere it is centred at origin, meaning that if we first transform the ray from world space
   into object space, the mathematical solution presented above can be simplified significantly.

   If a sphere is centred at origin, a point 'p' lies on a sphere of radius r2 if

   p dot p = r^2

   and we can find the t at which the (transformed) ray intersects the sphere by

   ( o + td ) dot ( o + td ) = r^2

   According to the reasoning above, we expand the above quadratic equation into the general form

   At2 + Bt + C = 0

   which now has coefficients:

   A = d dot d
   B = 2( d dot o )
   C = o dot o - r^2
   */

  // Early out if not on the scene
  if(!actor.OnScene())
  {
    return false;
  }

  const Node&       node            = actor.GetNode();
  const BufferIndex bufferIndex     = EventThreadServices::Get().GetEventBufferIndex();
  const Vector3&    translation     = node.GetWorldPosition(bufferIndex);
  const Vector3&    size            = node.GetSize(bufferIndex);
  const Vector3&    scale           = node.GetWorldScale(bufferIndex);
  const Rect<int>&  touchAreaOffset = actor.GetTouchAreaOffset(); // (left, right, bottom, top)

  // Transforms the ray to the local reference system. As the test is against a sphere, only the translation and scale are needed.
  const Vector3 rayOriginLocal(rayOrigin.x - translation.x - (touchAreaOffset.left + touchAreaOffset.right) * 0.5, rayOrigin.y - translation.y - (touchAreaOffset.top + touchAreaOffset.bottom) * 0.5, rayOrigin.z - translation.z);

  // Computing the radius is not needed, a square radius is enough so can just use size but we do need to scale the sphere
  const float width  = size.width * scale.width + touchAreaOffset.right - touchAreaOffset.left;
  const float height = size.height * scale.height + touchAreaOffset.bottom - touchAreaOffset.top;

  // Correction numeric error.
  const float epsilon = GetEpsilon(std::max(width, height));

  float squareSphereRadius = 0.5f * (width * width + height * height) + epsilon;

  float a  = rayDir.Dot(rayDir);                                      // a
  float b2 = rayDir.Dot(rayOriginLocal);                              // b/2
  float c  = rayOriginLocal.Dot(rayOriginLocal) - squareSphereRadius; // c

  return (b2 * b2 - a * c) >= 0.0f;
}

bool RayTest::ActorTest(const Internal::Actor& actor, const Vector4& rayOrigin, const Vector4& rayDir, Vector2& hitPointLocal, float& distance) const
{
  bool hit = false;

  if(actor.OnScene())
  {
    const Node& node = actor.GetNode();

    // Transforms the ray to the local reference system.
    // Calculate the inverse of Model matrix
    Matrix invModelMatrix(false /*don't init*/);
    invModelMatrix = node.GetWorldMatrix(0);
    invModelMatrix.Invert();

    Vector4 rayOriginLocal(invModelMatrix * rayOrigin);
    Vector4 rayDirLocal(invModelMatrix * rayDir - invModelMatrix.GetTranslation());

    // Test with the actor's XY plane (Normal = 0 0 1 1).
    float a = -rayOriginLocal.z;
    float b = rayDirLocal.z;

    if(fabsf(b) > Math::MACHINE_EPSILON_1)
    {
      // Ray travels distance * rayDirLocal to intersect with plane.
      distance = a / b;

      const Vector2&   size            = Vector2(node.GetSize(EventThreadServices::Get().GetEventBufferIndex()));
      const Rect<int>& touchAreaOffset = actor.GetTouchAreaOffset(); // (left, right, bottom, top)
      hitPointLocal.x                  = rayOriginLocal.x + rayDirLocal.x * distance + size.x * 0.5f;
      hitPointLocal.y                  = rayOriginLocal.y + rayDirLocal.y * distance + size.y * 0.5f;

      // Correction numeric error.
      const float epsilon = GetEpsilon(std::max(size.x, size.y));

      // Test with the actor's geometry.
      hit = (hitPointLocal.x >= touchAreaOffset.left - epsilon) && (hitPointLocal.x <= (size.x + touchAreaOffset.right + epsilon) && (hitPointLocal.y >= touchAreaOffset.top - epsilon) && (hitPointLocal.y <= (size.y + touchAreaOffset.bottom + epsilon)));
    }
  }

  return hit;
}

} // namespace Internal

} // namespace Dali
