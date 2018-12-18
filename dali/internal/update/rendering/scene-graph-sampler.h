#ifndef DALI_INTERNAL_SCENE_GRAPH_SAMPLER_H
#define DALI_INTERNAL_SCENE_GRAPH_SAMPLER_H

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
 */

#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-types.h>
#include <dali/integration-api/graphics/graphics.h>

#include <memory>


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

struct Sampler
{

  typedef Dali::FilterMode::Type FilterMode;
  typedef Dali::WrapMode::Type   WrapMode;

  /**
   * Constructor
   */
  Sampler()
  : mGraphics( nullptr ),
    mGfxSampler( nullptr ),
    mMinificationFilter(FilterMode::DEFAULT),
    mMagnificationFilter(FilterMode::DEFAULT),
    mSWrapMode(WrapMode::DEFAULT),
    mTWrapMode(WrapMode::DEFAULT),
    mRWrapMode(WrapMode::DEFAULT),
    mIsDirty(true)
  {}

  /**
   * Destructor
   */
  ~Sampler()
  {}

  /**
   * Sets the filter modes for an existing sampler
   * @param[in] sampler The sampler
   * @param[in] minFilterMode The filter to use under minification
   * @param[in] magFilterMode The filter to use under magnification
   */
  void SetFilterMode( unsigned int minFilterMode, unsigned int magFilterMode )
  {
    mMinificationFilter = static_cast<Dali::FilterMode::Type>(minFilterMode);
    mMagnificationFilter = static_cast<Dali::FilterMode::Type>(magFilterMode );
    mIsDirty = true;
  }

  /**
   * Sets the wrap mode for an existing sampler
   * @param[in] sampler The sampler
   * @param[in] rWrapMode Wrapping mode in z direction
   * @param[in] sWrapMode Wrapping mode in x direction
   * @param[in] tWrapMode Wrapping mode in y direction
   */
  void SetWrapMode( unsigned int rWrapMode, unsigned int sWrapMode, unsigned int tWrapMode )
  {
    mRWrapMode = static_cast<Dali::WrapMode::Type>(rWrapMode);
    mSWrapMode = static_cast<Dali::WrapMode::Type>(sWrapMode);
    mTWrapMode = static_cast<Dali::WrapMode::Type>(tWrapMode);
    mIsDirty = true;
  }

  bool operator==(const Sampler& rhs) const
  {
    return ( ( mMinificationFilter == rhs.mMinificationFilter ) &&
             ( mMagnificationFilter == rhs.mMagnificationFilter ) &&
             ( mSWrapMode == rhs.mSWrapMode ) &&
             ( mTWrapMode == rhs.mTWrapMode ) &&
             ( mRWrapMode == rhs.mRWrapMode ) );
  }

  bool operator!=(const Sampler& rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * Returns Graphics API sampler object
   * @return Pointer to API sampler or nullptr
   */
  const Dali::Graphics::API::Sampler* GetGfxObject() const
  {
    return mGfxSampler.get();
  }

  void DestroyGraphicsObjects()
  {
    mGfxSampler.reset();
  }

  inline Graphics::API::SamplerAddressMode GetGfxSamplerAddressMode( WrapMode mode ) const
  {
    switch(mode)
    {
      case WrapMode::REPEAT: return Graphics::API::SamplerAddressMode::REPEAT;
      case WrapMode::MIRRORED_REPEAT: return Graphics::API::SamplerAddressMode::MIRRORED_REPEAT;
      case WrapMode::CLAMP_TO_EDGE: return Graphics::API::SamplerAddressMode::CLAMP_TO_EDGE;
      case WrapMode::DEFAULT: return Graphics::API::SamplerAddressMode::CLAMP_TO_EDGE;
    }
    return {};
  }

  inline Graphics::API::SamplerMipmapMode GetGfxSamplerMipmapMode( FilterMode mode ) const
  {
    switch(mode)
    {
      case FilterMode::LINEAR_MIPMAP_LINEAR: return Graphics::API::SamplerMipmapMode::LINEAR;
      case FilterMode::NEAREST_MIPMAP_LINEAR: return Graphics::API::SamplerMipmapMode::LINEAR;
      case FilterMode::NEAREST_MIPMAP_NEAREST: return Graphics::API::SamplerMipmapMode::NEAREST;
      case FilterMode::LINEAR_MIPMAP_NEAREST: return Graphics::API::SamplerMipmapMode::NEAREST;
      case FilterMode::DEFAULT: return Graphics::API::SamplerMipmapMode::LINEAR;
      default: return {};
    }
    return {};
  }

  inline Graphics::API::SamplerFilter GetGfxFilter( FilterMode mode ) const
  {
    switch(mode)
    {
      case FilterMode::LINEAR: return Graphics::API::SamplerFilter::LINEAR;
      case FilterMode::NEAREST: return Graphics::API::SamplerFilter::NEAREST;
      case FilterMode::DEFAULT: return Graphics::API::SamplerFilter::LINEAR;
      case FilterMode::NONE: return Graphics::API::SamplerFilter::NEAREST;
      default: return {};
    }
    return {};
  }

  void Initialize( Integration::Graphics::Graphics& graphics )
  {
    mGraphics = &graphics;
    mGfxSampler.reset( nullptr );
    mGfxSampler = mGraphics->GetController().CreateSampler(
                    mGraphics->GetController().GetSamplerFactory()
                    .SetMinFilter( GetGfxFilter( mMinificationFilter ) )
                    .SetMagFilter( GetGfxFilter( mMagnificationFilter ) )
                    .SetAddressModeU( GetGfxSamplerAddressMode( mSWrapMode ) )
                    .SetAddressModeV( GetGfxSamplerAddressMode( mTWrapMode ) )
                    .SetAddressModeW( GetGfxSamplerAddressMode( mRWrapMode ) )
                    .SetMipmapMode( GetGfxSamplerMipmapMode( mMinificationFilter ) )
                    );
  }

  Integration::Graphics::Graphics*              mGraphics;   ///< Graphics interface
  std::unique_ptr<Dali::Graphics::API::Sampler> mGfxSampler; ///< Graphics Sampler object, default sampler is nullptr

  FilterMode  mMinificationFilter   : 4;    ///< The minify filter
  FilterMode  mMagnificationFilter  : 4;    ///< The magnify filter
  WrapMode    mSWrapMode            : 4;    ///< The horizontal wrap mode
  WrapMode    mTWrapMode            : 4;    ///< The vertical wrap mode
  WrapMode    mRWrapMode            : 4;    ///< The vertical wrap mode
  bool        mIsDirty              : 1;    ///< The 'dirty' flag
};


inline void SetFilterModeMessage( EventThreadServices& eventThreadServices, SceneGraph::Sampler& sampler, unsigned int minFilterMode, unsigned int magFilterMode )
{
  typedef MessageValue2< SceneGraph::Sampler, unsigned int, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &Sampler::SetFilterMode, minFilterMode, magFilterMode );
}

inline void SetWrapModeMessage( EventThreadServices& eventThreadServices, SceneGraph::Sampler& sampler, unsigned int rWrapMode, unsigned int sWrapMode, unsigned int tWrapMode )
{
  typedef MessageValue3< SceneGraph::Sampler, unsigned int, unsigned int, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &sampler, &Sampler::SetWrapMode, rWrapMode, sWrapMode, tWrapMode );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_SCENE_GRAPH_SAMPLER_H
