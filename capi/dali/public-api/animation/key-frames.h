#ifndef __DALI_KEY_FRAMES_H__
#define __DALI_KEY_FRAMES_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property-types.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class KeyFrames;
}

/**
 * Handle to a KeyFrames object. This allows the generation of key frame objects from
 * individual Property::Values. The type of the key frame is specified by the type of
 * the first value to be added. Adding key frames with a different Property::Value type
 * will result in a run time assert.
 */
class KeyFrames : public BaseHandle
{
public:
  /**
   * Create an initialized KeyFrame handle.
   * @return a handle to a newly allocated Dali resource.
   */
  static KeyFrames New();

  /**
   * Downcast an Object handle to KeyFrames handle. If handle points to a KeyFrames object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a KeyFrames object or an uninitialized handle
   */
  static KeyFrames DownCast( BaseHandle handle );

  /**
   * Create an uninitialized KeyFrame handle. This can be initialized with
   * KeyFrame::New(). Calling member functions with an uninitialized Dali::Object
   * is not allowed.
   */
  KeyFrames();

  /**
   * Virtual destructor
   */
  virtual ~KeyFrames();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Gets the type of the key frame. If no key frames have been added, this returns
   * Property::NONE.
   * @return The key frame property type.
   */
  Property::Type GetType() const;

  /**
   * Add a key frame. The key frames should be added in time order.
   * @param[in] progress A value between 0.0 and 1.0.
   * @param[in] value A value.
   */
  void Add(float progress, Property::Value value);

  /**
   * Add a key frame. The key frames should be added in time order.
   * @param[in] progress A value between 0.0 and 1.0.
   * @param[in] value A value.
   * @param[in] alpha The alpha function used to blend to the next keyframe
   */
  void Add(float progress, Property::Value value, AlphaFunction alpha);


public: // Not intended for application developers
  /**
   * This constructor is used by Dali::New() methods.
   * @param[in] keyFrames A pointer to an internal KeyFrame resource
   */
  explicit DALI_INTERNAL KeyFrames(Internal::KeyFrames* keyFrames);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_KEY_FRAMES_H__
