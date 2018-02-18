#ifndef __DALI_INTEGRATION_NATIVE_IMAGE_INTERFACE_EXTENSION_H__
#define __DALI_INTEGRATION_NATIVE_IMAGE_INTERFACE_EXTENSION_H__

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

#include <dali/public-api/images/native-image-interface.h>

namespace Dali
{

/**
 * @brief Extension abstract interface to provide platform-specific support for handling image data.
 *
 */
class NativeImageInterface::Extension
{
public:

  class Extension2; ///< Forward declare future extension interface

  /**
   * @brief Get custom fragment prefix for rendering native image.
   *
   * @return Custom fragment prefix code as string.
   */
  virtual const char* GetCustomFragmentPreFix() = 0;

  /**
   * @brief Get custom sampler type name for rendering native image.
   *
   * @return Custom sampler type name.
   */
  virtual const char* GetCustomSamplerTypename() = 0;

  /**
   * @brief Get texture target for binding native image as texture.
   *
   * @return Texture target.
   */
   //TODO: Graphics
  virtual int GetExternalImageTextureTarget() = 0;

  /**
   * @brief Retrieve the extension for the interface.
   *
   * @return Extension2 pointer if available, NULL otherwise
   */
  virtual Extension2* GetExtension2()
  {
    return NULL;
  }

protected:

  /**
   * @brief Destructor.
   *
   */
  virtual ~Extension()
  {
  }

};

} // namespace Dali

#endif // __DALI_INTEGRATION_NATIVE_IMAGE_INTERFACE_H__
