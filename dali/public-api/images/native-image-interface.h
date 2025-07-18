#ifndef DALI_NATIVE_IMAGE_INTERFACE_H
#define DALI_NATIVE_IMAGE_INTERFACE_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/any.h>
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
   * @brief Enumeration for PrepareTexture result.
   * If it is normal case, result value between [NO_ERROR_MIN ~ NO_ERROR_MAX]
   * If it is error case, result value between [ERROR_MIN ~ ERROR_MAX]
   * @SINCE_2_4.28
   */
  enum PrepareTextureResult
  {
    NO_ERROR_MIN = 0,
    NO_ERROR     = NO_ERROR_MIN, ///< Everything works okay without any error.     @SINCE_2_4.28
    IMAGE_CHANGED,               ///< Not an error. just for notify image data changed. @SINCE_2_4.28
    NO_ERROR_MAX = 0x0f,

    ERROR_MIN             = NO_ERROR_MAX + 1,
    NOT_INITIALIZED_IMAGE = ERROR_MIN, ///< Native image data not initialized yet. @SINCE_2_4.28
    NOT_INITIALIZED_GRAPHICS,          ///< Graphics interface not initialized yet. @SINCE_2_4.28
    NOT_SUPPORTED,                     ///< Native image not supported for this platform. @SINCE_2_4.28
    ERROR_MAX = 0xff,

    UNKNOWN_ERROR = ERROR_MAX,
  };

  /**
   * @brief Creates the resource for the NativeImage.
   *
   * e.g. For the EglImageKHR extension, this corresponds to calling eglCreateImageKHR().
   * @SINCE_1_9.23
   * @return false If the initialization fails
   * @pre The graphics subsystem has been initialized
   */
  virtual bool CreateResource() = 0;

  /**
   * @brief Destroys the resource for the NativeImage.
   *
   * e.g. For the EglImageKHR extension, this corresponds to calling eglDestroyImageKHR().
   * @SINCE_1_9.23
   * @pre The graphics subsystem has been initialized
   */
  virtual void DestroyResource() = 0;

  /**
   * @brief Uses the NativeImage as a texture for rendering.
   *
   * @SINCE_1_0.0
   * @return An error code from the graphics subsystem.
   * @pre The graphics subsystem has been initialized
   */
  virtual uint32_t TargetTexture() = 0;

  /**
   * @brief Called internally when the texture is bound in the GPU
   *
   * The correct texture sampler has already been bound before the function gets called.
   * @SINCE_1_0.0
   * @pre The graphics subsystem has been initialized
   * @return The result of PrepareTexture API.
   */
  virtual PrepareTextureResult PrepareTexture() = 0;

  /**
   * @brief Returns the width of the NativeImage.
   *
   * @SINCE_1_0.0
   * @return Width
   */
  virtual uint32_t GetWidth() const = 0;

  /**
   * @brief Returns the height of the NativeImage.
   *
   * @SINCE_1_0.0
   * @return Height
   */
  virtual uint32_t GetHeight() const = 0;

  /**
   * @brief Queries whether blending is required.
   * @SINCE_1_0.0
   * @return True if blending is required
   */
  virtual bool RequiresBlending() const = 0;

  /**
   * @brief Get the texture target for binding native image as texture.
   *
   * @SINCE_1_9.23
   * @return Texture target.
   */
  virtual int GetTextureTarget() const = 0;

  /**
   * @brief Apply custom fragment prefix for rendering native image.
   *
   * @param[in,out] shader shader code that will be applied fragment prefix.
   * @return True if the shader is changed.
   */
  virtual bool ApplyNativeFragmentShader(std::string& shader) = 0;

  /**
   * @brief Get custom sampler type name for rendering native image.
   *
   * @SINCE_1_9.23
   * @return Custom sampler type name.
   */
  virtual const char* GetCustomSamplerTypename() const = 0;

  /**
   * @brief Retrieves the internal native image.
   *
   * @SINCE_1_9.23
   * @return Any object containing the internal native image source
   */
  virtual Any GetNativeImageHandle() const = 0;

  /**
   * @brief Determine if the source data for the native image has changed.
   *
   * @SINCE_1_9.23
   * @return true if the source data has changed.
   */
  virtual bool SourceChanged() const = 0;

  /**
   * @brief Retrieves the updated area.
   *
   * @SINCE_2_2.31
   * @return The updated area
   * @note The x and y are the offset of the top left corner of the area from the top left of the image.
   */
  virtual Rect<uint32_t> GetUpdatedArea() = 0;

  /**
   * @brief Retrieves the extension for the interface.
   *
   * @SINCE_1_0.0
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetExtension()
  {
    return nullptr;
  }

protected:
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   *
   * The implementation should destroy the NativeImage resources.
   * @SINCE_1_0.0
   */
  ~NativeImageInterface() override = default;
};

/**
 * @brief Pointer to Dali::NativeImageInterface.
 * @SINCE_1_0.0
 */
using NativeImageInterfacePtr = Dali::IntrusivePtr<NativeImageInterface>;

/**
 * @}
 */
} // namespace Dali

#endif // DALI_NATIVE_IMAGE_INTERFACE_H
