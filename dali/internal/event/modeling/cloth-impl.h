#ifndef __DALI_INTERNAL_CLOTH_H__
#define __DALI_INTERNAL_CLOTH_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/devel-api/geometry/cloth.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/geometry/geometry-impl.h>

namespace Dali
{
class MeshData;

namespace Internal
{
class Cloth;

typedef IntrusivePtr<Cloth> ClothIPtr; ///< smart pointer to a Cloth

/**
 * A single cloth in a 3D model
 */
class Cloth : public BaseObject
{
public: // construction, destruction and initialisation

  /**
   * Create a new cloth.
   * @param[in] clothData the mesh data
   * @return A smart-pointer to the newly allocated Cloth.
   */
  static ClothIPtr New( Geometry& clothData );

  /**
   * Construct a new Cloth. This is not discardable or scalable.
   * @param[in] clothData the mesh data
   */
  Cloth( Geometry& clothData );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Cloth();

private:
  // Undefined
  Cloth(const Cloth&);

  // Undefined
  Cloth& operator=(const Cloth& rhs);
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Cloth& GetImplementation(Dali::Cloth& cloth)
{
  DALI_ASSERT_ALWAYS( cloth && "Cloth handle is empty" );

  BaseObject& handle = cloth.GetBaseObject();

  return static_cast<Internal::Cloth&>(handle);
}

inline const Internal::Cloth& GetImplementation(const Dali::Cloth& cloth)
{
  DALI_ASSERT_ALWAYS( cloth && "Cloth handle is empty" );

  const BaseObject& handle = cloth.GetBaseObject();

  return static_cast<const Internal::Cloth&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_CLOTH_H__
