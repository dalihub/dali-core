#ifndef NATIVEIMAGEEXTENSIONS_H
#define NATIVEIMAGEEXTENSIONS_H

#endif // NATIVEIMAGEEXTENSIONS_H

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <vulkan/vulkan.h>
//#include <vulkan/vk_tizen.h>
//#include <vulkan/vk_icd.h>
#include <dlfcn.h>
#include <tbm_surface.h>
#include <tbm_surface_internal.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/images/pixel.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 * NativeImageExtensions class provides Native image extension support for vulkan
 */
class NativeImageExtensions
{
public:
  /**
   * Constructor
   */
  NativeImageExtensions(Any tbmsurface);

  /**
   * Destructor
   */
  ~NativeImageExtensions();


public:   // EGLImageKHR extension support

  void  Initialize();                 // load extension function proc

  void  CreateImage(VkDevice device);      // create vkImage

  void  DestroyImage();                // Destroy resource

  void  TargetTexture();               // create vkImageView and vkSampler

private:
  VkDevice      mDevice;
  VkImage       mImage;
  VkImageView   mImageview;
  VkSampler     mSampler;
  tbm_surface_h tbm_surface;

  bool mImageKHRInitialized;             ///< Flag for whether extended KHR functions loaded
  bool mImageKHRInitializeFailed;        ///< Flag to avoid trying to reload extended KHR functions,
};

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali
