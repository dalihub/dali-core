#ifndef DALI_GRAPHICS_API_RENDER_COMMAND_H
#define DALI_GRAPHICS_API_RENDER_COMMAND_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <tuple>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <dali/graphics-api/graphics-api-buffer.h>
#include <dali/graphics-api/graphics-api-types.h>
#include <dali/graphics-api/graphics-api-pipeline.h>

#include <cstring>
namespace Dali
{
namespace Graphics
{
class Shader;
class Texture;
class Buffer;
class Sampler;
class Pipeline;

/**
 * RenderCommand update flag bits
 */
constexpr uint32_t RENDER_COMMAND_UPDATE_PIPELINE_BIT           = 1 << 0;
constexpr uint32_t RENDER_COMMAND_UPDATE_UNIFORM_BUFFER_BIT     = 1 << 1;
constexpr uint32_t RENDER_COMMAND_UPDATE_VERTEX_ATTRIBUTE_BIT   = 1 << 2;
constexpr uint32_t RENDER_COMMAND_UPDATE_TEXTURE_BIT            = 1 << 3;
constexpr uint32_t RENDER_COMMAND_UPDATE_SAMPLER_BIT            = 1 << 4;
constexpr uint32_t RENDER_COMMAND_UPDATE_INDEX_BUFFER_BIT       = 1 << 5;
constexpr uint32_t RENDER_COMMAND_UPDATE_RENDER_TARGET_BIT      = 1 << 6;
constexpr uint32_t RENDER_COMMAND_UPDATE_DRAW_BIT               = 1 << 7;
constexpr uint32_t RENDER_COMMAND_UPDATE_PUSH_CONSTANTS_BIT     = 1 << 8;
constexpr uint32_t RENDER_COMMAND_UPDATE_ALL_BITS               = 0xffff;

/**
 * Class which defines a render operation.
 * First, bind the relevant graphics objects to the command, finally call the Draw method.
 */
class RenderCommand
{
public:

  static constexpr uint32_t BINDING_INDEX_DONT_CARE { 0xffffffff };

  enum class IndexType
  {
    INDEX_TYPE_UINT16,
    INDEX_TYPE_UINT32,
  };

  /**
   * DrawType defines whether vertices are read contiguously, or use a secondary index
   */
  enum class DrawType
  {
    UNDEFINED_DRAW,
    VERTEX_DRAW,
    INDEXED_DRAW,
  };

  /**
   * Structure describes uniform buffer binding
   */
  struct UniformBufferBinding
  {
    UniformBufferBinding()
    : buffer( nullptr ),
      offset( 0u ),
      dataSize( 0u ),
      binding( 0u )
    {
    }

    const Buffer*  buffer;
    uint32_t offset;
    uint32_t dataSize;
    uint32_t binding;
    void*    pNext{ nullptr };

    UniformBufferBinding& SetBuffer( const Buffer* value )
    {
      buffer = value;
      return *this;
    }
    UniformBufferBinding& SetOffset( uint32_t value )
    {
      offset = value;
      return *this;
    }
    UniformBufferBinding& SetDataSize( uint32_t value )
    {
      dataSize = value;
      return *this;
    }
    UniformBufferBinding& SetBinding( uint32_t value )
    {
      binding = value;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& ss, const UniformBufferBinding& object);
  };

  /**
   * Structure describes texture binding
   */
  struct TextureBinding
  {
    const Texture*  texture { nullptr };
    const Sampler*  sampler { nullptr };
    uint32_t        binding { 0u };
    void*           pNext   { nullptr };
    TextureBinding() = default;

    TextureBinding& SetTexture( const Texture* value )
    {
      texture = value;
      return *this;
    }

    TextureBinding& SetSampler( const Sampler* value )
    {
      sampler = value;
      return *this;
    }

    TextureBinding& SetBinding( uint32_t value )
    {
      binding = value;
      return *this;
    }

    bool operator==( const TextureBinding& rhs)
    {
      return (texture == rhs.texture) && (sampler == rhs.sampler);
    }

    friend std::ostream& operator<<(std::ostream& ss, const TextureBinding& object);
  };

  /**
   * structure defining the sampler binding
   */
  struct SamplerBinding
  {
    const Sampler* sampler { nullptr };
    uint32_t       binding { 0u };
    void*          pNext   { nullptr };

    SamplerBinding& SetSampler( const Sampler* value )
    {
      sampler = value;
      return *this;
    }
    SamplerBinding& SetBinding( uint32_t value )
    {
      binding = value;
      return *this;
    }
    friend std::ostream& operator<<(std::ostream& ss, const SamplerBinding& object);
  };

  /**
   * structure defining the index buffer binding
   */
  struct IndexBufferBinding
  {
    const Buffer* buffer { nullptr };
    uint32_t      offset { 0u };
    IndexType     type   { IndexType::INDEX_TYPE_UINT16 };
    void*         pNext  { nullptr };

    IndexBufferBinding() = default;

    IndexBufferBinding& SetBuffer( const Buffer* value )
    {
      buffer = value;
      return *this;
    }

    IndexBufferBinding& SetOffset( uint32_t value )
    {
      offset = value;
      return *this;
    }

    IndexBufferBinding& SetType( IndexType value )
    {
      type = value;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& ss, const IndexBufferBinding&);
  };

  /**
   * structure defining the framebuffer (if any) of the render target
   */
  struct RenderTargetBinding
  {
    const Framebuffer*                    framebuffer { nullptr };
    std::vector<Framebuffer::ClearColor>  clearColors {};
    Framebuffer::DepthStencilClearColor   depthStencilClearColor {};
    float framebufferWidth; // Store the framebuffer size in case we need to set viewport
    float framebufferHeight;
    void* pNext{ nullptr };

    RenderTargetBinding() = default;

    RenderTargetBinding& SetFramebuffer( const Framebuffer* value )
    {
      framebuffer = value;
      return *this;
    }

    RenderTargetBinding& SetClearColors( std::vector<Framebuffer::ClearColor>&& value )
    {
      clearColors = value;
      return *this;
    }

    RenderTargetBinding& SetDepthStencilClearColor( Framebuffer::DepthStencilClearColor value )
    {
      depthStencilClearColor = value;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& ss, const RenderTargetBinding&);
  };

  /**
   * Structure defining the draw command
   */
  struct DrawCommand
  {
    DrawCommand() : drawType( DrawType::UNDEFINED_DRAW )
    {}

    DrawType drawType;
    union
    {
      uint32_t firstVertex;
      uint32_t firstIndex;
    };
    union
    {
      uint32_t vertexCount;
      uint32_t indicesCount;
    };
    uint32_t firstInstance;
    uint32_t instanceCount;

    // dynamic scissor state
    bool        scissorTestEnable { false };
    Rect2D      scissor {};

    // dynamic viewport state
    bool        viewportEnable { false };
    Viewport    viewport {};

    void*    pNext{ nullptr };
    DrawCommand& SetDrawType( DrawType value )
    {
      drawType = value;
      return *this;
    }
    DrawCommand& SetFirstVertex( uint32_t value )
    {
      firstVertex = value;
      return *this;
    }
    DrawCommand& SetFirstIndex( uint32_t value )
    {
      firstIndex = value;
      return *this;
    }
    DrawCommand& SetVertexCount( uint32_t value )
    {
      vertexCount = value;
      return *this;
    }
    DrawCommand& SetIndicesCount( uint32_t value )
    {
      indicesCount = value;
      return *this;
    }
    DrawCommand& SetFirstInstance( uint32_t value )
    {
      firstInstance = value;
      return *this;
    }
    DrawCommand& SetInstanceCount( uint32_t value )
    {
      instanceCount = value;
      return *this;
    }

    DrawCommand& SetScissor( Rect2D value )
    {
      scissor = value;
      return *this;
    }

    DrawCommand& SetScissorTestEnable( bool value )
    {
      scissorTestEnable = value;
      return *this;
    }

    DrawCommand& SetViewport( Viewport value )
    {
      viewport = value;
      return *this;
    }

    DrawCommand& SetViewportEnable( bool value )
    {
      viewportEnable = value;
      return *this;
    }


    friend std::ostream& operator<<(std::ostream& ss, const DrawCommand&);
  };

  /**
   * Structure defining the push constants
   */
  struct PushConstantsBinding
  {
    void*     data      { nullptr };
    uint32_t  size      { 0u };
    uint32_t  binding   { 0u };
    void*     pNext     { nullptr };

    PushConstantsBinding() = default;

    PushConstantsBinding& SetData( void* value )
    {
      data = value;
      return *this;
    }
    PushConstantsBinding& SetSize( uint32_t value )
    {
      size = value;
      return *this;
    }
    PushConstantsBinding& SetBinding( uint32_t value )
    {
      binding = value;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& ss, const PushConstantsBinding&);
  };

  /**
   * Constructor
   */
  RenderCommand()
  : mVertexBufferBindings(),
    mUniformBufferBindings(),
    mTextureBindings(),
    mIndexBufferBinding(),
    mRenderTargetBinding(),
    mDrawCommand(),
    mPushConstantsBindings(),
    mPipeline( nullptr ),
    mUpdateFlags( 0u )
  {
  }

  // derived types should not be moved direcly to prevent slicing
  RenderCommand( RenderCommand&& ) = default;
  RenderCommand& operator=( RenderCommand&& ) = default;

  // not copyable
  RenderCommand( const RenderCommand& ) = delete;
  RenderCommand& operator=( const RenderCommand& ) = delete;

  virtual ~RenderCommand() = default;

  /**
   * Resource binding API
   */
  RenderCommand& BindVertexBuffers( std::vector<const Buffer*>&& buffers )
  {
    mVertexBufferBindings = std::move( buffers );
    mUpdateFlags |= RENDER_COMMAND_UPDATE_VERTEX_ATTRIBUTE_BIT;
    return *this;
  }

  RenderCommand& BindUniformBuffers( const std::vector<UniformBufferBinding>* bindings )
  {
    mUniformBufferBindings = bindings;
    mUpdateFlags |= RENDER_COMMAND_UPDATE_UNIFORM_BUFFER_BIT;
    return *this;
  }

  RenderCommand& BindTextures( const std::vector<TextureBinding>* bindings )
  {
    // compare bindings
    mTextureBindings = bindings;
    mUpdateFlags |= RENDER_COMMAND_UPDATE_TEXTURE_BIT;
    return *this;
  }

  RenderCommand& BindSamplers( std::vector<SamplerBinding>&& bindings )
  {
    mSamplerBindings = std::move( bindings );
    mUpdateFlags |= RENDER_COMMAND_UPDATE_SAMPLER_BIT;
    return *this;
  }

  RenderCommand& PushConstants( std::vector<PushConstantsBinding>&& bindings )
  {
    mPushConstantsBindings = bindings;
    mUpdateFlags |= RENDER_COMMAND_UPDATE_PUSH_CONSTANTS_BIT;
    return *this;
  }

  RenderCommand& BindRenderTarget( const RenderTargetBinding& binding )
  {
    mRenderTargetBinding = binding;
    mUpdateFlags |= RENDER_COMMAND_UPDATE_RENDER_TARGET_BIT;
    return *this;
  }

  RenderCommand& BindRenderTarget( RenderTargetBinding&& binding )
  {
    mRenderTargetBinding = std::move(binding);
    mUpdateFlags |= RENDER_COMMAND_UPDATE_RENDER_TARGET_BIT;
    return *this;
  }

  RenderCommand& BindPipeline( const Pipeline* pipeline )
  {
    if( !mPipeline || mPipeline != pipeline )
    {
      mPipeline = pipeline;
      mUpdateFlags |= RENDER_COMMAND_UPDATE_PIPELINE_BIT;
    }
    return *this;
  }

  RenderCommand& BindIndexBuffer( const IndexBufferBinding& binding )
  {
    mIndexBufferBinding = binding;
    mUpdateFlags |= RENDER_COMMAND_UPDATE_INDEX_BUFFER_BIT;
    return *this;
  }

  RenderCommand& Draw( DrawCommand&& drawCommand )
  {
    mDrawCommand = drawCommand;
    mUpdateFlags |= RENDER_COMMAND_UPDATE_DRAW_BIT;
    return *this;
  }

  static std::vector<TextureBinding> NewTextureBindings()
  {
    return std::vector<TextureBinding>{};
  }

  static std::vector<PushConstantsBinding> NewPushConstantsBindings( uint32_t count )
  {
    auto retval = std::vector<PushConstantsBinding>{};
    retval.resize( count );
    return retval;
  }

  static std::vector<UniformBufferBinding> NewUniformBufferBindings( uint32_t count )
  {
    auto retval = std::vector<UniformBufferBinding>{};
    retval.resize( count );
    return retval;
  }

  // Getters
  const std::vector<const Buffer*>& GetVertexBufferBindings() const
  {
    return mVertexBufferBindings;
  }

  const auto& GetUniformBufferBindings() const
  {
    return mUniformBufferBindings;
  }

  const std::vector<TextureBinding>* GetTextureBindings() const
  {
    return mTextureBindings;
  }

  const IndexBufferBinding& GetIndexBufferBinding() const
  {
    return mIndexBufferBinding;
  }

  const RenderTargetBinding& GetRenderTargetBinding() const
  {
    return mRenderTargetBinding;
  }

  const DrawCommand& GetDrawCommand() const
  {
    return mDrawCommand;
  }

  const std::vector<PushConstantsBinding>& GetPushConstantsBindings() const
  {
    return mPushConstantsBindings;
  }

  const Pipeline* GetPipeline() const
  {
    return mPipeline;
  }

  friend std::ostream& operator<<(std::ostream& ss, const RenderCommand& object);

public:

  // list of resources
  std::vector<const Buffer*>                mVertexBufferBindings;
  const std::vector<UniformBufferBinding>*  mUniformBufferBindings;
  const std::vector<TextureBinding>*        mTextureBindings;
  std::vector<SamplerBinding>               mSamplerBindings;

  IndexBufferBinding                        mIndexBufferBinding;
  RenderTargetBinding                       mRenderTargetBinding;
  DrawCommand                               mDrawCommand;
  std::vector<PushConstantsBinding>         mPushConstantsBindings;
  const Pipeline*                           mPipeline;

protected:
  uint32_t mUpdateFlags; // update flags to be handled by implementation
};

//@todo Move to a better place? (can't go in graphics-api-types, it's a cyclic dependency)
struct TextureBindingState
{
  std::vector<RenderCommand::TextureBinding> textureBindings {};

  Extension extension{ nullptr };

  TextureBindingState& SetTextureBindings( const std::vector<RenderCommand::TextureBinding>& theTextureBindings )
  {
    textureBindings = theTextureBindings;
    return *this;
  }
};

} // namespace Graphics
} // namespace Dali


#endif // DALI_GRAPHICS_API_RENDER_COMMAND_H
