#ifndef __DALI_IMAGE_H__
#define __DALI_IMAGE_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Image;
}

/**
 * @brief An Image object represents an image resource that can be added to ImageActors.
 *
 * Image objects can be shared between ImageActors. This is practical if you have a visual element on screen
 * which is repeatedly used. An example would be a button background image.
 * The image resource is discarded when all ImageActors using the Image object are discarded or in case they
 * were created with ReleasePolicy::Unused, taken off stage.
 * Note: if a resource was shared between Image objects it exists until its last reference is gone.
 *
 * Image objects are responsible for the underlying resource's lifetime.
 *
 * <i>ReleasePolicies</i>
 * - Unused: release resource once ImageActor using it is taken off stage.
 * - Never: keep resource alive until Image object is thrown away.
 *
 * <i>Resolution of conflicting policies</i>
 * If the same image is created more than once with conflicting policies, ReleasePolicy "Never" overrides "Unused".
 *
 * Signals
 * | %Signal Name           | Method                       |
 * |------------------------|------------------------------|
 * | uploaded               | @ref UploadedSignal()        |
 */
class DALI_IMPORT_API Image : public BaseHandle
{
public:
  /**
   * @brief Resource management options.
   */

  /**
   * @brief ReleasePolicy controls the way images are deleted from memory.
   */
  enum ReleasePolicy
  {
    UNUSED, ///< release resource once image is not in use anymore (eg. all actors using it become offstage). Reload when resource is required again.
    NEVER   ///< keep image data for the lifetime of the object. (default)
  };

  /**
   * @brief Type of signal for Image Uploaded.
   */
  typedef Signal< void (Image) > ImageSignalType;

public:

  /**
   * @brief Constructor which creates an empty Image handle.
   *
   * Use Image::New(...) to create an initialised handle.
   */
  Image();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Image();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Image(const Image& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Image& operator=(const Image& rhs);

  /**
   * @brief Downcast an Object handle to Image handle.
   *
   * If handle points to a Image object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Image object or an uninitialized handle
   */
  static Image DownCast( BaseHandle handle );

  /**
   * @brief Return resource release policy.
   *
   * @return resource release policy
   */
  ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Returns the width of the image.
   *
   * Returns either the requested width or the actual loaded width if no specific size was requested.
   *
   * @return width of the image in pixels.
   */
  unsigned int GetWidth() const;

  /**
   * @brief Returns the height of the image.
   *
   * Returns either the requested height or the actual loaded height if no specific size was requested.
   *
   * @return height of the image in pixels.
   */
  unsigned int GetHeight() const;

public: // Signals

  /**
   * @brief This signal is emitted when the image data gets uploaded to GL.
   *
   * It Will be sent after an actor using the image is added to
   * the stage, when such a staged image is reloaded, or when a staged
   * BufferImage calls Update().
   * @return A signal object to Connect() with.
   */
  ImageSignalType& UploadedSignal();

public: // Not intended for application developers

  explicit DALI_INTERNAL Image(Internal::Image*);
};

} // namespace Dali

#endif // __DALI_IMAGE_H__
