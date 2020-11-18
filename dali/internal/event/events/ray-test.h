#ifndef DALI_INTERNAL_RAY_TEST_H
#define DALI_INTERNAL_RAY_TEST_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

struct Vector2;
struct Vector4;

namespace Internal
{

class Actor;
class EventThreadServices;

/**
 * Contains methods to perform ray tests on given actors.
 *
 * Stores a reference to the EventThreadServices so limit the number of times this is created
 * to avoid repeated calls to EventThreadServices::Get().
 */
class RayTest
{
public:

  /// Constructor
  RayTest();

  /// Default Destructor
  ~RayTest() = default;

  /**
   * Performs a ray-sphere test with the given pick-ray and the given actor's bounding sphere.
   *
   * @param[in] actor The actor to perform the ray-sphere test on
   * @param[in] rayOrigin The ray origin in the world's reference system
   * @param[in] rayDir The ray director vector in the world's reference system
   * @return True if the ray intersects the actor's bounding sphere
   *
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   */
  bool SphereTest(const Internal::Actor& actor, const Vector4& rayOrigin, const Vector4& rayDir) const;

  /**
   * Performs a ray-actor test with the given pick-ray and the given actor's geometry.
   *
   * @param[in] actor The actor to perform the ray-sphere test on
   * @param[in] rayOrigin The ray origin in the world's reference system.
   * @param[in] rayDir The ray director vector in the world's reference system.
   * @param[out] hitPointLocal The hit point in the Actor's local reference system.
   * @param[out] distance The distance from the hit point to the camera.
   * @return True if the ray intersects the actor's geometry.
   *
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   */
  bool ActorTest(const Internal::Actor& actor, const Vector4& rayOrigin, const Vector4& rayDir, Vector2& hitPointLocal, float& distance) const;

private:
  const EventThreadServices& mEventThreadServices;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RAY_TEST_H
