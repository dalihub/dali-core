#ifndef __DALI_CLOTH_H__
#define __DALI_CLOTH_H__

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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/geometry/mesh.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class Cloth;
}

/**
 * @brief A Cloth is a specialized mesh that can be used and modified
 * by the Dynamics engine.
 */
class Cloth : public Mesh
{
public:

  /**
   * @brief Create an initialized plane aligned on the XY axis.
   *
   * @param[in] width   The width of the plane
   * @param[in] height  The height of the plane
   * @param[in] xSteps  The number of vertices along the X axis
   * @param[in] ySteps  The number of vertices along the Y axis
   * @param[in] textureCoordinates  UV coordinates.
   * @return A handle to a newly allocated Dali resource.
   */
  static Cloth New(const float width,
                   const float height,
                   const int xSteps,
                   const int ySteps,
                   const Rect<float>& textureCoordinates = Rect<float>(0.0f, 0.0f, 1.0f, 1.0f));


  /**
   * @brief Create an uninitialized Cloth; this can be initialized with Cloth::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Cloth();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Cloth();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Downcast an Object handle to a Cloth handle.
   *
   * If handle points to a Cloth object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to an object
   * @return handle to a Cloth object or an uninitialized handle
   */
  static Cloth DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] cloth A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Cloth(Internal::Cloth* cloth);
};

} // namespace Dali

#endif // __DALI_CLOTH_H__
