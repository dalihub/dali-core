#ifndef DALI_GRAPHICS_VULKAN_HPP_WRAPPER_H
#define DALI_GRAPHICS_VULKAN_HPP_WRAPPER_H

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

// Vulkan without exceptions
#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <vulkan/vulkan.hpp>

#define VK_EXT_image_drm_format_modifier                                         (1)
#define VK_EXT_IMAGE_DRM_FORMAT_MODIFIER_SPEC_VERSION                            (1)
#define VK_EXT_IMAGE_DRM_FORMAT_MODIFIER_EXTENSION_NAME "VK_EXT_image_drm_format_modifier"

#define VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT                    (-1000158000)

#define VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT                (1000158000)
#define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT     (1000158002)
#define VK_STRUCTURE_TYPE_IMAGE_EXCPLICIT_DRM_FORMAT_MODIFIER_CREATE_INFO_EXT    (1000158004)

#define VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT                                  (1000158000)

#define VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT                                   (0x00000080)
#define VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT                                   (0x00000100)
#define VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT                                   (0x00000200)

typedef struct VkDrmFormatModifierPropertiesEXT
{
    uint64_t drmFormatModifier;
    uint32_t drmFormatModifierPlaneCount;
    VkFormatFeatureFlags drmFormatModifierTilingFeatures;
} VkDrmFormatModifierPropertiesEXT;

typedef struct VkDrmFormatModifierPropertiesListEXT
{
    VkStructureType sType;
    void *pNext;
    uint32_t drmFormatModifierCount;
    VkDrmFormatModifierPropertiesEXT *pDrmFormatModifierProperties;
} VkDrmFormatModifierPropertiesListEXT;

typedef struct VkPhysicalDeviceImageDrmFormatModifierInfoEXT
{
    VkStructureType sType;
    const void *pNext;
    uint64_t drmFormatModifier;
} VkPhysicalDeviceImageDrmFormatModifierInfoEXT;

typedef struct VkImageDrmFormatModifierExplicitCreateInfoEXT
{
    VkStructureType sType;
    const void *pNext;
    uint64_t drmFormatModifier;
    uint32_t drmFormatModifierPlaneCount;
    const VkSubresourceLayout *pPlaneLayouts;
} VkImageDrmFormatModifierExplicitCreateInfoEXT;

#pragma GCC diagnostic pop

#endif // DALI_GRAPHICS_VULKAN_HPP_WRAPPER_H
