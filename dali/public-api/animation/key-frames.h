#ifndef __DALI_KEY_FRAMES_H__
#define __DALI_KEY_FRAMES_H__

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
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property-types.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

namespace Internal DALI_INTERNAL
{
class KeyFrames;
}

/**
 * @brief A set of key frames for a property that can be animated using Dali::Animation::AnimateBetween().
 *
 * This allows the generation of key frame objects from individual
 * Property::Values. The type of the key frame is specified by the
 * type of the first value to be added. Adding key frames with a
 * different Property::Value type will result in a run time assert.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API KeyFrames : public BaseHandle
{
public:
  /**
   * @brief Create an initialized KeyFrames handle.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static KeyFrames New();

  /**
   * @brief Downcast a handle to KeyFrames handle.
   *
   * If handle points to a KeyFrames object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a KeyFrames object or an uninitialized handle
   */
  static KeyFrames DownCast( BaseHandle handle );

  /**
   * @brief Create an uninitialized KeyFrame handle.
   *
   * This can be initialized with KeyFrame::New().
   * Calling member functions with an uninitialized KeyFrames handle is not allowed.
   * @SINCE_1_0.0
   */
  KeyFrames();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~KeyFrames();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  KeyFrames(const KeyFrames& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  KeyFrames& operator=(const KeyFrames& rhs);

  /**
   * @brief Gets the type of the key frame.
   *
   * If no key frames have been added, this returns Property::NONE.
   * @SINCE_1_0.0
   * @return The key frame property type.
   */
  Property::Type GetType() const;

  /**
   * @brief Add a key frame.
   *
   * The key frames should be added in time order.
   * @SINCE_1_0.0
   * @param[in] progress A progress value between 0.0 and 1.0.
   * @param[in] value A value.
   */
  void Add(float progress, Property::Value value);

  /**
   * @brief Add a key frame.
   *
   * The key frames should be added in time order.
   * @SINCE_1_0.0
   * @param[in] progress A progress value between 0.0 and 1.0.
   * @param[in] value A value.
   * @param[in] alpha The alpha function used to blend to the next keyframe
   */
  void Add(float progress, Property::Value value, AlphaFunction alpha);


public: // Not intended for application developers
  /**
   * @internal
   * @brief This constructor is used by KeyFrames::New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] keyFrames A pointer to an internal KeyFrame resource
   */
  explicit DALI_INTERNAL KeyFrames(Internal::KeyFrames* keyFrames);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_KEY_FRAMES_H__
