//
// Created by adam.b on 03/05/17.
//

#include <dali/graphics/vulkan/image-view.h>
#include <dali/graphics/vulkan/image.h>
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
namespace Internal
{
class ImageView : public VkObject
{
public:
  ImageView() = delete;
  ImageView( const Image& image, const vk::ImageViewCreateInfo& info );

private:

  Image mImage { nullptr };
  vk::ImageViewCreateInfo mCreateInfo { };
};

ImageView::ImageView( const Image& image, const vk::ImageViewCreateInfo& info )
: VkObject()
{

}

}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali