#ifndef __DALI_LINEAR_CONSTRAINER_H__
#define __DALI_LINEAR_CONSTRAINER_H__

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

//EXTERNAL INCLUDES
#include <cfloat> //For FLT_MAX

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

namespace Internal DALI_INTERNAL
{
  class LinearConstrainer;
}

/**
 * @brief
 *
 * LinearConstrainer applies constraints to objects given a linear map.
 * A linear map is defined by a set of value-progress pairs.
 * Progress must be normalized between [0,1]. If no progress is defined, the values
 * are considered to be equally spaced along the x axis.
 */
class DALI_IMPORT_API LinearConstrainer : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the LinearConstrainer class.
   */
  struct Property
  {
    enum
    {
      VALUE     = DEFAULT_OBJECT_PROPERTY_START_INDEX,  ///< name "value" type Array of float
      PROGRESS,                                         ///< name "progress" type Array of float
    };
  };

  /**
   * @brief Create an initialized LinearConstrainer handle.
   *
   * @return a handle to a newly allocated Dali resource.
   */
  static LinearConstrainer New();

  /**
   * @brief Downcast an Object handle to LinearConstrainer handle.
   *
   * If handle points to a LinearConstrainer object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a LinearConstrainer object or an uninitialized handle
   */
  static LinearConstrainer DownCast( BaseHandle handle );

  /**
   * @brief Create an uninitialized LinearConstrainer handle.
   *
   * This can be initialized with PathConstrainer::New(). Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  LinearConstrainer();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~LinearConstrainer();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  LinearConstrainer(const LinearConstrainer& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  LinearConstrainer& operator=(const LinearConstrainer& rhs);

  /**
   * @brief Applies the linear constraint to the target property
   *
   * @param[in] target Property to be constrained
   * @param[in] source Property used as parameter for the path
   * @param[in] range The range of values in the source property which will be mapped to [0,1]
   * @param[in] wrap Wrapping domain. Source property will be wrapped in the domain [wrap.x,wrap.y] before mapping to [0,1]
   */
  void Apply( Dali::Property target, Dali::Property source, const Vector2& range, const Vector2& wrap = Vector2(-FLT_MAX, FLT_MAX) );

  /**
   * @brief Removes the linear constraint in the target object
   *
   * @param[in] target A handle to an object constrained by the LinearConstrainer
   */
  void Remove( Dali::Handle& target );

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali::New() methods.
   *
   * @param[in] pathConstrainer A pointer to an internal PathConstrainer resource
   */
  explicit DALI_INTERNAL LinearConstrainer(Internal::LinearConstrainer* pathConstrainer);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_LINEAR_CONSTRAINER_H__
