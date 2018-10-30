#ifndef VULKAN_EXTENSIONS_H
#define VULKAN_EXTENSIONS_H

#if 0
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
#endif

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

#endif
