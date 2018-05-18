#ifndef __DALI_NATIVE_IMAGE_H__
#define __DALI_NATIVE_IMAGE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/native-image-interface.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class NativeImage;
}

/**
 * @DEPRECATED_1_2.41
 *
 * @brief NativeImage represents a platform specific image resource.
 *
 * Its data is provided by native resources, such as shared bitmap memory or pixmap.
 * @SINCE_1_0.0
 */
class DALI_CORE_API  NativeImage : public Image
{
public:

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Constructor with creates an uninitialized NativeImage object.
   *
   * Use NativeImage::New(...) to create an initialised object.
   * @SINCE_1_0.0
   */
  NativeImage() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
   ~NativeImage() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  NativeImage( const NativeImage& handle ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  NativeImage& operator=( const NativeImage& rhs ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Triggers asynchronous creation of backing GL texture immediately.
   *
   * The normal policy is for a GL texture to created lazily when needed.
   * This function forces the allocation of a texture to happen at the earliest
   * opportunity.
   *
   * @SINCE_1_0.0
   * @note If the application loses its GL context, native images may lose their
   * GL textures. This function can be called again after context regain to force
   * the creation of the GL texture if still needed.
   */
  void CreateGlTexture() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Creates a new NativeImage, which used native resources.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @SINCE_1_0.0
   * @param[in] nativeImageInterface An reference to the object of the interface implementation
   * @return A handle to a newly allocated object
   */
  static NativeImage New( NativeImageInterface& nativeImageInterface ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Downcasts a handle to NativeImage handle.
   *
   * If handle points to a NativeImage object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return handle to a NativeImage or an uninitialized handle
   */
  static NativeImage DownCast( BaseHandle handle ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Gets custom fragment prefix for rendering a native image.
   *
   * @return String for custom fragment prefix
   */
  const char* GetCustomFragmentPreFix() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Gets custom sampler type name for rendering a native image.
   *
   * @return String for custom sampler type name
   */
  const char* GetCustomSamplerTypename() DALI_DEPRECATED_API;

public: // Not intended for application developers

  explicit DALI_INTERNAL NativeImage( Internal::NativeImage* ) DALI_DEPRECATED_API;

};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_NATIVE_IMAGE_H__
