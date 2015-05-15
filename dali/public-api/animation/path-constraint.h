#ifndef __DALI_PATH_CONSTRAINT_H__
#define __DALI_PATH_CONSTRAINT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/animation/path.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class PathConstraint;
}
/**
 * @brief
 *
 * PathConstraint applies constraints to objects to follow a path.
 * A Vector3 property will be constrained to the position of the path and a Rotation property will be constrained to follow
 * the tangent of the path given a forward vector in object's local space.
 *
 */
class DALI_IMPORT_API PathConstraint : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the PathConstraint class.
   */
  struct Property
  {
    enum
    {
      RANGE   =  DEFAULT_OBJECT_PROPERTY_START_INDEX, ///< name "range" type Vector2
    };
  };

  /**
   * @brief Create an initialized PathConstraint handle.
   *
   * @return a handle to a newly allocated Dali resource.
   */
  static PathConstraint New( Dali::Path path, const Vector2& range );

  /**
   * @brief Downcast an Object handle to PathConstraint handle.
   *
   * If handle points to a PathConstraint object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a PathConstraint object or an uninitialized handle
   */
  static PathConstraint DownCast( BaseHandle handle );

  /**
   * @brief Create an uninitialized PathConstraint handle.
   *
   * This can be initialized with PathConstraint::New(). Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  PathConstraint();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PathConstraint();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  PathConstraint(const PathConstraint& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  PathConstraint& operator=(const PathConstraint& rhs);

  /**
   * @brief Applies the path constraint to the target property
   *
   * @param[in] source Property used as parameter for the path
   * @param[in] target Property to be constrained
   * @param[in] forward Vector in object's local space which will be aligned to the tangent of the path (only needed for Rotation properties)
   */
  void Apply( Dali::Property source, Dali::Property target, const Vector3& forward = Vector3() );

  /**
   * @brief Removes the path constraint in the target object
   *
   * @param[in] target A handle to an object constrained by the PathConstraint
  */
  void Remove( Dali::Handle target );

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali::New() methods.
   *
   * @param[in] path A pointer to an internal path resource
   */
  explicit DALI_INTERNAL PathConstraint(Internal::PathConstraint* path);
};

} // namespace Dali

#endif // __DALI_KEY_FRAMES_H__
