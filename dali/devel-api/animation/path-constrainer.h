#ifndef __DALI_PATH_CONSTRAINER_H__
#define __DALI_PATH_CONSTRAINER_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

//EXTERNAL INCLUDES
#include <cfloat> //For FLT_MAX

// INTERNAL INCLUDES
#include <dali/public-api/animation/path.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
  class PathConstrainer;
}
/**
 * @brief
 *
 * PathConstrainer applies constraints to objects to follow a path.
 * A Vector3 property will be constrained to the position of the path and a Rotation property will be constrained to follow
 * the tangent of the path given a forward vector in object's local space.
 *
 */
class DALI_CORE_API PathConstrainer : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the PathConstrainer class.
   */
  struct Property
  {
    enum
    {
      FORWARD   =  DEFAULT_OBJECT_PROPERTY_START_INDEX, ///< name "forward" type Vector3
      POINTS,                                           ///< name "points" type Array of Vector3
      CONTROL_POINTS                                    ///< name "controlPoints" type Array of Vector3
    };
  };

  /**
   * @brief Create an initialized PathConstrainer handle.
   *
   * @return a handle to a newly allocated Dali resource.
   */
  static PathConstrainer New();

  /**
   * @brief Downcast an Object handle to PathConstrainer handle.
   *
   * If handle points to a PathConstrainer object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a PathConstrainer object or an uninitialized handle
   */
  static PathConstrainer DownCast( BaseHandle handle );

  /**
   * @brief Create an uninitialized PathConstrainer handle.
   *
   * This can be initialized with PathConstrainer::New(). Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  PathConstrainer();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PathConstrainer();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  PathConstrainer(const PathConstrainer& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  PathConstrainer& operator=(const PathConstrainer& rhs);

  /**
   * @brief Applies the path constraint to the target property
   *
   * @param[in] target Property to be constrained
   * @param[in] source Property used as parameter for the path
   * @param[in] range The range of values in the source property which will be mapped to [0,1]
   * @param[in] wrap Wrapping domain. Source property will be wrapped in the domain [wrap.x,wrap.y] before mapping to [0,1]
   */
  void Apply( Dali::Property target, Dali::Property source, const Vector2& range, const Vector2& wrap = Vector2(-FLT_MAX, FLT_MAX) );

  /**
   * @brief Removes the path constraint in the target object
   *
   * @param[in] target A handle to an object constrained by the PathConstrainer
   */
  void Remove( Dali::Handle& target );

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali::New() methods.
   *
   * @param[in] pathConstrainer A pointer to an internal PathConstrainer resource
   */
  explicit DALI_INTERNAL PathConstrainer(Internal::PathConstrainer* pathConstrainer);
};

} // namespace Dali

#endif // __DALI_PATH_CONSTRAINER_H__
