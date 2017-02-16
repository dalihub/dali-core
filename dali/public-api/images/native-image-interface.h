#ifndef __DALI_INTEGRATION_NATIVE_IMAGE_INTERFACE_H__
#define __DALI_INTEGRATION_NATIVE_IMAGE_INTERFACE_H__

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
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

/**
 * @brief Abstract interface to provide platform-specific support for handling image data.
 *
 * For example, an implementation could use EGL extensions, etc.
 * @SINCE_1_0.0
 */
class NativeImageInterface : public Dali::RefObject
{
public:

  class Extension; ///< Forward declare future extension interface

  /**
   * @brief Creates the GL resource for the NativeImage.
   *
   * e.g. For the EglImageKHR extension, this corresponds to calling eglCreateImageKHR().
   * @SINCE_1_0.0
   * @return false If the initialization fails
   * @pre There is a GL context for the current thread.
   */
  virtual bool GlExtensionCreate() = 0;

  /**
   * @brief Destroys the GL resource for the NativeImage.
   *
   * e.g. For the EglImageKHR extension, this corresponds to calling eglDestroyImageKHR().
   * @SINCE_1_0.0
   * @pre There is a GL context for the current thread.
   */
  virtual void GlExtensionDestroy() = 0;

  /**
   * @brief Uses the NativeImage as a texture for rendering.
   *
   * @SINCE_1_0.0
   * @return A GL error code
   * @pre There is a GL context for the current thread.
   */
  virtual unsigned int TargetTexture() = 0;

  /**
   * @brief Called internally for each Bind call for this texture to allow implementation specific operations.
   *
   * The correct texture sampler has already been bound before the function gets called.
   * @SINCE_1_0.0
   * @pre glAbstraction is being used by context in current thread
   */
  virtual void PrepareTexture() = 0;

  /**
   * @brief Returns the width of the NativeImage.
   *
   * @SINCE_1_0.0
   * @return Width
   */
  virtual unsigned int GetWidth() const = 0;

  /**
   * @brief Returns the height of the NativeImage.
   *
   * @SINCE_1_0.0
   * @return Height
   */
  virtual unsigned int GetHeight() const = 0;

 /**
  * @brief Queries whether blending is required.
  * @SINCE_1_0.0
  * @return True if blending is required
  */
  virtual bool RequiresBlending() const = 0;

  /**
   * @brief Retrieves the extension for the interface.
   *
   * @SINCE_1_0.0
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetExtension()
  {
    return NULL;
  }

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   *
   * The implementation should destroy the NativeImage resources.
   * @SINCE_1_0.0
   */
  virtual ~NativeImageInterface()
  {
  }

};

/**
 * @brief Pointer to Dali::NativeImageInterface.
 * @SINCE_1_0.0
 */
typedef Dali::IntrusivePtr<NativeImageInterface>  NativeImageInterfacePtr;

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_INTEGRATION_NATIVE_IMAGE_INTERFACE_H__
