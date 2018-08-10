#ifndef DALI_VULKAN_161117_RENDER_COMMAND_POOL_H
#define DALI_VULKAN_161117_RENDER_COMMAND_POOL_H

#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-render-command.h>

#include <vector>
#include <deque>
#include <memory>


namespace Dali
{
namespace Graphics
{
namespace API
{
class RenderCommand;
}
}
namespace Internal
{
namespace SceneGraph
{
// Very simple per-frame stack allocator, all objects allocated here will live
// only one frame
template<class T>
struct GraphicsStackAllocator
{
  class Handle
  {
    friend class GraphicsStackAllocator<T>;
  public:
    Handle() = delete;
    Handle( Handle&& ) = default;
    Handle( const Handle& ) = delete;
    Handle(GraphicsStackAllocator& allocator, uint32_t index )
    : mAllocator( &allocator ),
      mIndex( index )
    {
    }

  public:

    T* Get() const
    {
      return &mAllocator->mPool[mIndex];
    }

    T* operator->()
    {
      return &mAllocator->mPool[mIndex];
    }

    operator T&()
    {
      return mAllocator->mPool[mIndex];
    }

  private:

    GraphicsStackAllocator* mAllocator;
    uint32_t                mIndex;
  };

  GraphicsStackAllocator( uint32_t defaultIncrease )
  : mPool(),
    mDefaultIncrease( defaultIncrease ),
    mPoolIndex( 0u )
  {
  }

  void Rollback( bool freeResources )
  {
    if( freeResources )
    {
      mPool.clear();
    }
    mPoolIndex = 0u;
    //puts("rollback");
  }

  template<class... ARGS>
  Handle Allocate( bool construct, ARGS&&... args )
  {
    if(mPoolIndex == mPool.size() )
    {
      mPool.resize( mPool.size() + mDefaultIncrease );
    }

    if( construct )
    {
      new(&mPool[mPoolIndex])T(std::forward<ARGS...>( args... ));
    }

    return Handle( *this, mPoolIndex++ );
  }

  Handle Allocate( bool construct )
  {
    if(mPoolIndex == mPool.size() )
    {
      mPool.resize( mPool.size() + mDefaultIncrease );
    }

    if( construct )
    {
      new(&mPool[mPoolIndex])T();
    }

    auto retval =  Handle( *this, mPoolIndex );
    mPoolIndex++;
    return retval;
  }

  std::deque<T>           mPool;
  uint32_t                mDefaultIncrease;
  uint32_t                mPoolIndex;
};

/**
 * Internal pool allocating transient render command pools
 * The pool is used by GraphicsAlgorithms in order to allocate
 * per-render-item render commands.
 */
class RenderCommandPool
{

public:

  RenderCommandPool() = delete;

  RenderCommandPool( Graphics::API::Controller& controller, uint32_t initialiCapacity, bool fixed )
  : mController( controller ),
    mInitialCapacity( initialiCapacity ),
    mAllocationIndex( 0u ),
    mIsFixedSize( fixed )
  {
  }

  ~RenderCommandPool() = default;

  void Reset()
  {
    mAllocationIndex = 0;
  }

  void Trim()
  {
    if( mAllocationIndex == 0 )
    {
      mPool.clear();
    }
    else
    {
      mPool.resize( mAllocationIndex + 1 );
    }
  }

  Graphics::API::RenderCommand* Allocate( const Graphics::API::RenderCommand* baseCommand )
  {
    if( mAllocationIndex == mPool.size() )
    {
      // if pool cannot resize any more return nullptr
      if( mIsFixedSize && !mPool.empty() )
      {
        return nullptr;
      }

      ResizeBy( mInitialCapacity );
    }

    auto retval = mPool[mAllocationIndex].get();

    if( baseCommand ) // todo: should add Clone()?
    {
      retval->BindIndexBuffer( baseCommand->GetIndexBufferBinding() );
      retval->BindPipeline( baseCommand->GetPipeline() );
      retval->BindUniformBuffers( baseCommand->GetUniformBufferBindings() );
      retval->BindRenderTarget( baseCommand->GetRenderTargetBinding() );
      retval->BindTextures( baseCommand->GetTextureBindings() );
      retval->BindVertexBuffers( baseCommand->GetVertexBufferBindings() );
      retval->PushConstants( baseCommand->GetPushConstantsBindings() );
      retval->Draw( baseCommand->GetDrawCommand() );
    }

    mAllocationIndex++;
    return retval;
  }

  void ResizeBy( uint32_t count )
  {
    Resize( uint32_t( count + mPool.size() ) );
  }

  void Resize( uint32_t count )
  {
    auto currentSize = mPool.size();
    mPool.resize( count );

    for( auto i = currentSize; i < mPool.size(); ++i )
    {
      mPool[i] = std::move( mController.AllocateRenderCommand() );
    }
  }

private:

  Graphics::API::Controller& mController;
  uint32_t mInitialCapacity;
  uint32_t mAllocationIndex;
  bool mIsFixedSize;

  std::vector<std::unique_ptr<Graphics::API::RenderCommand>> mPool;
};
}
}
}

#endif //DALI_VULKAN_161117_RENDER_COMMAND_POOL_H
