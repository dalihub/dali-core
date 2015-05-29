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
 * @brief Abstract interface to provide platform-specific support for handling image data.
 *
 * For example, an implementation could use EGL extensions, etc.
 */
class NativeImageInterface : public Dali::RefObject
{
public:

  class Extension; ///< Forward declare future extension interface

  /**
   * @brief Create the GL resource for the NativeImage.
   *
   * e.g. For the EglImageKHR extension, this corresponds to calling eglCreateImageKHR()
   * @pre There is a GL context for the current thread.
   * @return false If the initialization fails.
   */
  virtual bool GlExtensionCreate() = 0;

  /**
   * @brief Destroy the GL resource for the NativeImage.
   *
   * e.g. For the EglImageKHR extension, this corresponds to calling eglDestroyImageKHR()
   * @pre There is a GL context for the current thread.
   */
  virtual void GlExtensionDestroy() = 0;

  /**
   * @brief Use the NativeImage as a texture for rendering.
   *
   * @pre There is a GL context for the current thread.
   * @return A GL error code
   */
  virtual unsigned int TargetTexture() = 0;

  /**
   * @brief Called in each NativeTexture::Bind() call to allow implementation specific operations.
   *
   * The correct texture sampler has already been bound before the function gets called.
   * @pre glAbstraction is being used by context in current thread
   */
  virtual void PrepareTexture() = 0;

  /**
   * @brief Returns the width of the NativeImage.
   *
   * @return width
   */
  virtual unsigned int GetWidth() const = 0;

  /**
   * @brief Returns the height of the NativeImage.
   *
   * @return height
   */
  virtual unsigned int GetHeight() const = 0;

 /**
  * Query whether blending is required
  */
  virtual bool RequiresBlending() const = 0;

  /**
   * @brief Retrieve the extension for the interface.
   *
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
   */
  virtual ~NativeImageInterface()
  {
  }

};

/**
 * @brief Pointer to Dali::NativeImageInterface
 */
typedef IntrusivePtr<NativeImageInterface>  NativeImageInterfacePtr;

} // namespace Dali

#endif // __DALI_INTEGRATION_NATIVE_IMAGE_INTERFACE_H__
