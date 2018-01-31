#include <dali/graphics/vulkan/vulkan-graphics-controller.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
// TODO: @todo temporarily ignore missing return type, will be fixed later
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
API::Accessor<API::Shader> Controller::CreateShader( const API::BaseFactory<API::Shader>& factory )
{
}

API::Accessor<API::Texture> Controller::CreateTexture( const API::BaseFactory<API::Texture>& factory )
{
}

API::Accessor<API::TextureSet> Controller::CreateTextureSet( const API::BaseFactory<API::TextureSet>& factory )
{
}

API::Accessor<API::DynamicBuffer> Controller::CreateDynamicBuffer( const API::BaseFactory<API::DynamicBuffer>& factory )
{
}

API::Accessor<API::StaticBuffer> Controller::CreateStaticBuffer( const API::BaseFactory<API::StaticBuffer>& factory )
{
}

API::Accessor<API::Sampler> Controller::CreateSampler( const API::BaseFactory<API::Sampler>& factory )
{
}

API::Accessor<API::Framebuffer> Controller::CreateFramebuffer( const API::BaseFactory<API::Framebuffer>& factory )
{
}
#pragma GCC diagnostic pop

void Controller::GetRenderItemList()
{
}

void Controller::SubmitCommand( API::RenderCommand&& command )
{
}

void Controller::BeginFrame()
{
}

void Controller::EndFrame()
{
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
