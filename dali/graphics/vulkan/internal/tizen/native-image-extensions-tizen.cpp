
/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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


// CLASS HEADER
#include <dali/graphics/vulkan/internal/tizen/native-image-extensions-tizen.h>

#if 0
namespace
{
// function pointers assigned in InitializeEglImageKHR
PFN_vkGetInstanceProcAddr get_proc_addr= 0
PFN_vkCreateImageFromNativeBufferTIZEN create_presentable_image = 0;
} // unnamed namespace
#endif

namespace Dali
{

namespace Graphics
{

namespace Vulkan
{

/**
 * Constructor
 */
NativeImageExtensions::NativeImageExtensions(Any tbmsurface)
: mImageKHRInitialized(false),
  mImageKHRInitializeFailed(false)
{
   mImage = static_cast<VkImage>( 0 );
   mImageview = static_cast<VkImageView>( 0 );
   mSampler = static_cast<VkSampler>( 0 );
   mDevice = static_cast<VkDevice>( 0 );

   tbm_surface = AnyCast< tbm_surface_h >( tbmsurface );
}

/**
 * Destructor
 */
NativeImageExtensions::~NativeImageExtensions()
{

}

// load extension function proc
void  NativeImageExtensions::Initialize()
{

}

// create vkImage
void NativeImageExtensions::CreateImage(VkDevice device)
{
   mDevice = device;
}

// Destroy resource
void  NativeImageExtensions::DestroyImage()
{

}

 // create vkImageView and vkSampler
void  NativeImageExtensions::TargetTexture()
{

}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali
