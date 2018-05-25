#ifndef DALI_GRAPHICS_API_RENDER_COMMAND_H
#define DALI_GRAPHICS_API_RENDER_COMMAND_H

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

// EXTERNAL INCLUDES
#include <tuple>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-accessor.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <dali/graphics-api/graphics-api-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace API
{
class Shader;
class Texture;
class Buffer;
class Sampler;
class Pipeline;


/**
 * @brief Interface class for RenderCommand types in the graphics API.
 *
 * @startuml
 *
 * skinparam defaultFontName Ubuntu Mono
 * class RenderCommand {
 *  -- protected --
 *  #VertexAttributeBufferBinding[]  mVertexBufferBindings
 *  #UniformBufferBinding[]          mUniformBufferBindings
 *  #TextureBinding[]                mTextureBindings
 *  #SamplerBinding[]                mSamplerBindings
 *  #IndexBufferBinding              mIndexBufferBinding
 *  #RenderTargetBinding             mRenderTargetBinding
 *  #DrawCommand                     mDrawCommand
 *  #PushConstantsBinding[]          mPushConstantsBindings
 *  #RenderState                     mRenderState
 *
 * -- public API --
 *  +RenderCommand& BindVertextBuffers()
 *  +RenderCommand& BindUniformBuffers()
 *  +RenderCommand& BindTextures()
 *  +RenderCommand& BindSamplers()
 *  +RenderCommand& PushConstants()
 *  +RenderCommand& BindRenderState()
 *  +RenderCommand& Draw()
 *  -- static API ( helper functions )--
 *  {static} NewVertexAttributeBufferBindings()
 *  {static} NewVertexAttributeBufferBindings()
 *  {static} NewVertexAttributeBufferBindings()
 *  {static} NewTextureBindings()
 *  {static} NewPushConstantsBindings()
 * }
 *
 * class VertexAttributeBufferBinding {
 * Accessor<Buffer>   buffer
 * uint32_t           location
 * uint32_t           offset
 * uint32_t           stride
 * InputAttributeRate rate
 * void*              pNext
 * }
 *
 * class UniformBufferBinding {
 *   #Accessor<Buffer> buffer
 *   #uint32_t         offset
 *   #uint32_t         dataSize
 *   #uint32_t         binding
 *   #void*            pNext
 * }
 *
  class IndexBufferBinding {
    #Accessor<Buffer> buffer
    #uint32_t         offset
    #IndexType        type
    #void*            pNext
  }

  class RenderTargetBinding {
    #Accessor<Framebuffer>                 framebuffer
    #std::vector<Framebuffer::ClearColor>  clearColors
    #Framebuffer::DepthStencilClearColor   dsClearColor
    #void*                                 pNext
  }

  class DrawCommand {
      #DrawType drawType;
      #uint32_t firstVertex
      #uint32_t firstIndex
      #uint32_t vertexCount
      #uint32_t indicesCount
      #uint32_t firstInstance
      #uint32_t instanceCount
      #void*    pNext
  }

  class PushConstantsBinding {
    #void*     data
    #uint32_t  size
    #uint32_t  binding
    #void*    pNext
  }

  class RenderState {
    #Accessor<Shader> shader
    #void*    pNext
  }

  class TextureBinding {
    #Accessor<Texture> texture
    #Accessor<Sampler> sampler
    #uint32_t          binding
    #void*             pNext
  }

  class SamplerBinding {
    #Accessor<Sampler> sampler
    #uint32_t binding
    #void*    pNext
  }

 note as RenderStateWIP
 Other render states like
 blending etc. should be added
 as public fields of this structure.
 end note

 RenderStateWIP .. RenderState

 * note as N1
 * Each state is described as POD
 * structure which is a Vulkan
 * approach.
 * end note
 *
 * note as N2
 * Field pNext may be used by the
 * implementation to pass additional
 * data.
 * end note
 *
 * N2 .. VertexAttributeBufferBinding::pNext
 * N1 .. VertexAttributeBufferBinding
 * N1 .. UniformBufferBinding
 * N1 .. IndexBufferBinding
 * N1 .. RenderTargetBinding
 *

 *
 * RenderCommand *-right- VertexAttributeBufferBinding
 * RenderCommand *-right- UniformBufferBinding
 * RenderCommand *-left- IndexBufferBinding
 * RenderCommand *-left- RenderTargetBinding
 * RenderCommand *-up- RenderState
 * RenderCommand *-up- DrawCommand
 * RenderCommand *-down- PushConstantsBinding
 * RenderCommand *-down- SamplerBinding
 * RenderCommand *-down- TextureBinding
 * @enduml
 */
class RenderCommand
{
public:

  static constexpr uint32_t BINDING_INDEX_DONT_CARE { 0xffffffff };

  enum class InputAttributeRate
  {
    PER_VERTEX,
    PER_INSTANCE
  };

  enum class IndexType
  {
    INDEX_TYPE_UINT16,
    INDEX_TYPE_UINT32,
  };

  enum class DrawType
  {
    UNDEFINED_DRAW,
    VERTEX_DRAW,
    INDEXED_DRAW,
  };

  enum class Topology
  {
    TRIANGLES,
    TRIANGLE_FAN,
    TRIANGLE_STRIP,
    LINES
  };

  /**
   * Describes buffer attribute binding
   *
   */
  struct VertexAttributeBufferBinding
  {
    VertexAttributeBufferBinding() = default;

    Accessor<Buffer> buffer{ nullptr };
    uint32_t location { 0u };
    uint32_t offset { 0u };
    uint32_t stride { 0u };
    InputAttributeRate rate { InputAttributeRate::PER_VERTEX };

    uint32_t binding { 0u };
    void*    pNext{ nullptr };

    VertexAttributeBufferBinding& SetBuffer( Accessor<Buffer> value )
    {
      buffer = value;
      return *this;
    }
    VertexAttributeBufferBinding& SetLocation( uint32_t value )
    {
      location = value;
      return *this;
    }
    VertexAttributeBufferBinding& SetOffset( uint32_t value )
    {
      offset = value;
      return *this;
    }
    VertexAttributeBufferBinding& SetStride( uint32_t value )
    {
      stride = value;
      return *this;
    }
    VertexAttributeBufferBinding& SetBinding( uint32_t value )
    {
      binding = value;
      return *this;
    }
    VertexAttributeBufferBinding& SetInputAttributeRate( InputAttributeRate value)
    {
      rate = value;
      return *this;
    }
  };

  /**
   * Structure describes uniform buffer binding
   */
  struct UniformBufferBinding
  {
    UniformBufferBinding() :
    buffer( nullptr ), offset( 0u ), dataSize( 0u ), binding( 0u ) {}
    Accessor<Buffer> buffer;
    uint32_t offset;
    uint32_t dataSize;
    uint32_t binding;
    void*    pNext{ nullptr };

    UniformBufferBinding& SetBuffer( Accessor<Buffer> value )
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
  };

  /**
   *
   */
  struct TextureBinding
  {
    Accessor<Texture> texture;
    Accessor<Sampler> sampler;
    uint32_t binding;
    void*    pNext{ nullptr };
    TextureBinding() : texture(nullptr), sampler( nullptr ), binding( 0u ) {}

    TextureBinding& SetTexture( Accessor<Texture> value )
    {
      texture = value;
      return *this;
    }
    TextureBinding& SetSampler( Accessor<Sampler> value )
    {
      sampler = value;
      return *this;
    }
    TextureBinding& SetBinding( uint32_t value )
    {
      binding = value;
      return *this;
    }
  };

  /**
   *
   */
  struct SamplerBinding
  {
    Accessor<Sampler> sampler;
    uint32_t binding;
    void*    pNext{ nullptr };
    SamplerBinding& SetSampler( Accessor<Sampler> value )
    {
      sampler = value;
      return *this;
    }
    SamplerBinding& SetBinding( uint32_t value )
    {
      binding = value;
      return *this;
    }
  };

  /**
   *
   */
  struct IndexBufferBinding
  {
    Accessor<Buffer> buffer { nullptr };
    uint32_t offset { 0u };
    IndexType type { IndexType::INDEX_TYPE_UINT16 };
    void*    pNext{ nullptr };
    IndexBufferBinding() = default;

    IndexBufferBinding& SetBuffer( Accessor<Buffer> value )
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
  };

  struct RenderTargetBinding
  {
    Accessor<Framebuffer>                 framebuffer { nullptr };
    std::vector<Framebuffer::ClearColor>  clearColors {};
    Framebuffer::DepthStencilClearColor   dsClearColor {};
    void*    pNext{ nullptr };
    RenderTargetBinding() = default;

    RenderTargetBinding& SetFramebuffer( Accessor<Framebuffer> value )
    {
      framebuffer = value;
      return *this;
    }

    RenderTargetBinding& SetClearColors( std::vector<Framebuffer::ClearColor>&& value )
    {
      clearColors = value;
      return *this;
    }

    RenderTargetBinding& SetFramebuffer( Framebuffer::DepthStencilClearColor value )
    {
      dsClearColor = value;
      return *this;
    }
  };

  struct DrawCommand
  {
    DrawCommand() :
     drawType( DrawType::UNDEFINED_DRAW ){}
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
  };

  /**
   *
   */
  struct PushConstantsBinding
  {
    void*     data;
    uint32_t  size;
    uint32_t  binding;
    void*    pNext{ nullptr };
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
  };

  /**
   *
   */
  struct RenderState
  {
    struct BlendState
    {
      bool blendingEnabled { false };
    };

    RenderState() = default;

    Accessor<Shader> shader { nullptr };
    BlendState blendState {};
    Topology topology{ Topology::TRIANGLES };

    RenderState& SetShader( Accessor<Shader> value )
    {
      shader = value;
      return *this;
    }

    RenderState& SetBlendState( BlendState value )
    {
      blendState = value;
      return *this;
    }

    RenderState& SetTopology( Topology value )
    {
      topology = value;
      return *this;
    }

    void*    pNext{ nullptr };
  };

  RenderCommand()
  : mVertexBufferBindings(),
    mUniformBufferBindings(),
    mTextureBindings(),
    mIndexBufferBinding(),
    mRenderTargetBinding(),
    mDrawCommand(),
    mPushConstantsBindings(),
    mRenderState(),
    mPipeline( nullptr )
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
  RenderCommand& BindVertexBuffers( std::vector<VertexAttributeBufferBinding>&& bindings )
  {
    mVertexBufferBindings = std::move( bindings );
    return *this;
  }

  RenderCommand& BindUniformBuffers( std::vector<UniformBufferBinding>&& bindings )
  {
    mUniformBufferBindings = std::move( bindings );
    return *this;
  }

  RenderCommand& BindTextures( std::vector<TextureBinding>&& bindings )
  {
    mTextureBindings = std::move( bindings );
    return *this;
  }

  RenderCommand& BindSamplers( std::vector<SamplerBinding>&& bindings )
  {
    mSamplerBindings = std::move( bindings );
    return *this;
  }

  RenderCommand& PushConstants( std::vector<PushConstantsBinding>&& bindings )
  {
    mPushConstantsBindings = bindings;
    return *this;
  }

  RenderCommand& BindRenderState( RenderState& renderState )
  {
    mRenderState = renderState;
    return *this;
  }

  RenderCommand& BindRenderTarget( const RenderTargetBinding& binding )
  {
    mRenderTargetBinding = binding;
    return *this;
  }

  RenderCommand& BindRenderTarget( RenderTargetBinding&& binding )
  {
    mRenderTargetBinding = std::move(binding);
    return *this;
  }

  RenderCommand& Draw( DrawCommand&& drawCommand )
  {
    mDrawCommand = drawCommand;
    return *this;
  }

  RenderCommand& BindPipeline( const Accessor<Pipeline>& pipeline )
  {
    mPipeline = pipeline;
    return *this;
  }

  RenderCommand& BindIndexBuffer( const IndexBufferBinding& binding )
  {
    mIndexBufferBinding = binding;
    return *this;
  }

  static std::vector<VertexAttributeBufferBinding> NewVertexAttributeBufferBindings()
  {
    return std::vector<VertexAttributeBufferBinding>{};
  }

  /**
   * Makes a copy ( or moves ) all bindings from the source array
   * @param bindings
   * @return
   */
  static std::vector<VertexAttributeBufferBinding> NewVertexAttributeBufferBindings( std::vector<VertexAttributeBufferBinding>&& bindings )
  {
    return std::vector<VertexAttributeBufferBinding>{ std::move(bindings) };
  }

  static std::vector<VertexAttributeBufferBinding> NewVertexAttributeBufferBindings( const std::vector<VertexAttributeBufferBinding>& bindings )
  {
    return std::vector<VertexAttributeBufferBinding>{ bindings };
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

  // Getters
  const std::vector<VertexAttributeBufferBinding>& GetVertexBufferBindings() const
  {
    return mVertexBufferBindings;
  }

  const auto& GetUniformBufferBindings() const
  {
    return mUniformBufferBindings;
  }

  const std::vector<TextureBinding>& GetTextureBindings() const
  {
    return mTextureBindings;
  }

  const IndexBufferBinding& GetIndexBufferBinding() const
  {
    return mIndexBufferBinding;
  }

  const auto& GetRenderTargetBinding() const
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

  const RenderState& GetRenderState() const
  {
    return mRenderState;
  }

  Accessor<Pipeline> GetPipeline() const
  {
    return mPipeline;
  }

protected:

  std::vector<VertexAttributeBufferBinding> mVertexBufferBindings;
  std::vector<UniformBufferBinding>         mUniformBufferBindings;
  std::vector<TextureBinding>               mTextureBindings;
  std::vector<SamplerBinding>               mSamplerBindings;
  IndexBufferBinding                        mIndexBufferBinding;
  RenderTargetBinding                       mRenderTargetBinding;
  DrawCommand                               mDrawCommand;
  std::vector<PushConstantsBinding>         mPushConstantsBindings;
  RenderState                               mRenderState;
  Accessor<Pipeline>                        mPipeline;

};

} // namespace API
} // namespace Graphics
} // namespace Dali


#endif // DALI_GRAPHICS_API_RENDER_COMMAND_H


/**
 * @brief Interface class for RenderCommand types in the graphics API.
 *
 * @startuml
 *
 * skinparam defaultFontName Ubuntu Mono
 * class RenderCommand {
 *  -- protected --
 *  #VertexAttributeBufferBinding[]  mVertexBufferBindings
 *  #UniformBufferBinding[]          mUniformBufferBindings
 *  #TextureBinding[]                mTextureBindings
 *  #SamplerBinding[]                mSamplerBindings
 *  #IndexBufferBinding              mIndexBufferBinding
 *  #RenderTargetBinding             mRenderTargetBinding
 *  #DrawCommand                     mDrawCommand
 *  #PushConstantsBinding[]          mPushConstantsBindings
 *  #RenderState                     mRenderState
 *
 * -- public API --
 *  +RenderCommand& BindVertextBuffers()
 *  +RenderCommand& BindUniformBuffers()
 *  +RenderCommand& BindTextures()
 *  +RenderCommand& BindSamplers()
 *  +RenderCommand& PushConstants()
 *  +RenderCommand& BindRenderState()
 *  +RenderCommand& Draw()
 *  -- static API ( helper functions )--
 *  {static} NewVertexAttributeBufferBindings()
 *  {static} NewVertexAttributeBufferBindings()
 *  {static} NewVertexAttributeBufferBindings()
 *  {static} NewTextureBindings()
 *  {static} NewPushConstantsBindings()
 * }
 *
 * class VertexAttributeBufferBinding {
 * Accessor<Buffer>   buffer
 * uint32_t           location
 * uint32_t           offset
 * uint32_t           stride
 * InputAttributeRate rate
 * void*              pNext
 * }
 *
 * class UniformBufferBinding {
 *   #Accessor<Buffer> buffer
 *   #uint32_t         offset
 *   #uint32_t         dataSize
 *   #uint32_t         binding
 *   #void*            pNext
 * }
 *
  class IndexBufferBinding {
    #Accessor<Buffer> buffer
    #uint32_t         offset
    #IndexType        type
    #void*            pNext
  }

  class RenderTargetBinding {
    #Accessor<Framebuffer>                 framebuffer
    #std::vector<Framebuffer::ClearColor>  clearColors
    #Framebuffer::DepthStencilClearColor   dsClearColor
    #void*                                 pNext
  }

  class DrawCommand {
      #DrawType drawType;
      #uint32_t firstVertex
      #uint32_t firstIndex
      #uint32_t vertexCount
      #uint32_t indicesCount
      #uint32_t firstInstance
      #uint32_t instanceCount
      #void*    pNext
  }

  class PushConstantsBinding {
    #void*     data
    #uint32_t  size
    #uint32_t  binding
    #void*    pNext
  }

  class RenderState {
    #Accessor<Shader> shader
    #void*    pNext
  }

  class TextureBinding {
    #Accessor<Texture> texture
    #Accessor<Sampler> sampler
    #uint32_t          binding
    #void*             pNext
  }

  class SamplerBinding {
    #Accessor<Sampler> sampler
    #uint32_t binding
    #void*    pNext
  }

 note as RenderStateWIP
 Other render states like
 blending etc. should be added
 as public fields of this structure.
 end note

 RenderStateWIP .. RenderState

 * note as N1
 * Each state is described as POD
 * structure which is a Vulkan
 * approach.
 * end note
 *
 * note as N2
 * Field pNext may be used by the
 * implementation to pass additional
 * data.
 * end note
 *
 * N2 .. VertexAttributeBufferBinding::pNext
 * N1 .. VertexAttributeBufferBinding
 * N1 .. UniformBufferBinding
 * N1 .. IndexBufferBinding
 * N1 .. RenderTargetBinding
 *

 *
 * RenderCommand *-right- VertexAttributeBufferBinding
 * RenderCommand *-right- UniformBufferBinding
 * RenderCommand *-left- IndexBufferBinding
 * RenderCommand *-left- RenderTargetBinding
 * RenderCommand *-up- RenderState
 * RenderCommand *-up- DrawCommand
 * RenderCommand *-down- PushConstantsBinding
 * RenderCommand *-down- SamplerBinding
 * RenderCommand *-down- TextureBinding
 * @enduml
 */