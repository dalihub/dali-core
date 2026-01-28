/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-renderer.h>

// INTERNAL INCLUDES
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/common/owner-key-type.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-uniform-block.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/render-shader.h>
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>
#include <dali/internal/update/controllers/render-manager-dispatcher.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

#include <dali/internal/update/common/property-resetter.h>

#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
#ifdef DEBUG_ENABLED
Debug::Filter* gSceneGraphRendererLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SG_RENDERER");
#endif

namespace // unnamed namespace
{
Dali::Internal::SceneGraph::MemoryPoolCollection*                                 gMemoryPoolCollection = nullptr;
static constexpr Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType gMemoryPoolType       = Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType::RENDERER;

inline Vector4 AdjustExtents(const Vector4& updateArea, const Dali::Extents& updateAreaExtents)
{
  if(DALI_LIKELY(updateAreaExtents == Dali::Extents()))
  {
    return updateArea;
  }
  const float left   = updateArea.x - updateArea.z * 0.5f - static_cast<float>(updateAreaExtents.start);
  const float right  = updateArea.x + updateArea.z * 0.5f + static_cast<float>(updateAreaExtents.end);
  const float top    = updateArea.y - updateArea.w * 0.5f - static_cast<float>(updateAreaExtents.top);
  const float bottom = updateArea.y + updateArea.w * 0.5f + static_cast<float>(updateAreaExtents.bottom);
  return Vector4((left + right) * 0.5f, (top + bottom) * 0.5f, (right - left), (bottom - top));
}

enum DirtyUpdateFlags
{
  NOT_CHECKED = 0,
  CHECKED     = 0x80,

  DIRTY_FLAG_SHIFT   = 0,
  UPDATED_FLAG_SHIFT = 1,

  IS_DIRTY_MASK   = 1 << DIRTY_FLAG_SHIFT,
  IS_UPDATED_MASK = (1 << UPDATED_FLAG_SHIFT) | IS_DIRTY_MASK,
};

template<typename... ParameterType>
void CallRenderFunction(Render::RendererKey& rendererKey, void (Render::Renderer::*member)(ParameterType...), ParameterType... parameter)
{
  if(DALI_LIKELY(rendererKey))
  {
    ((*rendererKey.Get()).*member)(parameter...);
  }
}

template<typename ParameterType>
void CallRenderFunction(Render::RendererKey& rendererKey, void (Render::Renderer::*member)(const ParameterType&), const ParameterType& parameter)
{
  if(DALI_LIKELY(rendererKey))
  {
    ((*rendererKey.Get()).*member)(parameter);
  }
}

} // Anonymous namespace

RendererKey Renderer::NewKey()
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "SG::Renderer::RegisterMemoryPoolCollection not called!");
  void* ptr = gMemoryPoolCollection->AllocateRawThreadSafe(gMemoryPoolType);
  auto  key = gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, ptr);
  new(ptr) Renderer();
  return RendererKey(key);
}

void Renderer::RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection)
{
  gMemoryPoolCollection = &memoryPoolCollection;
}

void Renderer::UnregisterMemoryPoolCollection()
{
  gMemoryPoolCollection = nullptr;
}

Renderer::Renderer()
: mRenderer{},
  mTextureSet(nullptr),
  mGeometry(nullptr),
  mShader(nullptr),
  mVisualProperties(nullptr),
  mBlendColor(nullptr),
  mStencilParameters(RenderMode::AUTO, StencilFunction::ALWAYS, 0xFF, 0x00, 0xFF, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP),
  mIndexedDrawFirstElement(0u),
  mIndexedDrawElementsCount(0u),
  mBlendBitmask(0u),
  mUpdateAreaExtents(),
  mDepthFunction(DepthFunction::LESS),
  mFaceCullingMode(FaceCullingMode::NONE),
  mBlendMode(BlendMode::AUTO),
  mDepthWriteMode(DepthWriteMode::AUTO),
  mDepthTestMode(DepthTestMode::AUTO),
  mRenderingBehavior(DevelRenderer::Rendering::IF_REQUIRED),
  mUpdateDecay(Renderer::Decay::INITIAL),
  mVisualPropertiesDirtyFlags(CLEAN_FLAG),
  mRegenerateUniformMap(false),
  mPremultipledAlphaEnabled(false),
  mUseSharedUniformBlock(true),
  mDirtyUpdated(NOT_CHECKED),
  mMixColor(Color::WHITE),
  mDepthIndex(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::operator delete(void* ptr)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "SG::Renderer::RegisterMemoryPoolCollection not called!");
  gMemoryPoolCollection->FreeThreadSafe(gMemoryPoolType, ptr);
}

Renderer* Renderer::Get(RendererKey::KeyType rendererKey)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "SG::Renderer::RegisterMemoryPoolCollection not called!");
  return static_cast<Renderer*>(gMemoryPoolCollection->GetPtrFromKey(gMemoryPoolType, rendererKey));
}

RendererKey Renderer::GetKey(const Renderer& renderer)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "SG::Renderer::RegisterMemoryPoolCollection not called!");
  return RendererKey(gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, static_cast<void*>(const_cast<Renderer*>(&renderer))));
}

RendererKey Renderer::GetKey(Renderer* renderer)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "SG::Renderer::RegisterMemoryPoolCollection not called!");
  return RendererKey(gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, static_cast<void*>(renderer)));
}

bool Renderer::PrepareRender(BufferIndex updateBufferIndex)
{
  bool rendererUpdated        = Updated() || mRenderingBehavior == DevelRenderer::Rendering::CONTINUOUSLY || mUpdateDecay > 0;
  auto shaderMapChangeCounter = mShader ? mShader->GetUniformMap().GetChangeCounter() : 0u;
  bool shaderMapChanged       = mShader && (mShaderMapChangeCounter != shaderMapChangeCounter);
  if(shaderMapChanged)
  {
    mShaderMapChangeCounter = shaderMapChangeCounter;
  }

  if(mUniformMapChangeCounter != mUniformMaps.GetChangeCounter() || shaderMapChanged)
  {
    // The map has changed since the last time we checked.
    rendererUpdated       = true;
    mRegenerateUniformMap = true;
    mUpdateDecay          = Renderer::Decay::INITIAL; // Render at least twice if the map has changed/actor has been added

    // Update local counters to identify any future changes to maps
    // (unlikely, but allowed by API).
    mUniformMapChangeCounter = mUniformMaps.GetChangeCounter();
  }
  if(mUpdateDecay > 0)
  {
    mUpdateDecay = static_cast<Renderer::Decay>(static_cast<int>(mUpdateDecay) - 1);
  }

  // Age down visual properties dirty flag
  if(mVisualPropertiesDirtyFlags != CLEAN_FLAG)
  {
    DALI_ASSERT_DEBUG(mVisualProperties && "Visual Property not created yet! something wrong (maybe event message flush ordering issue)");

    rendererUpdated |= mVisualProperties->PrepareProperties();
    mVisualPropertiesDirtyFlags >>= 1;
  }

  if(mRenderingBehavior == DevelRenderer::Rendering::CONTINUOUSLY || mVisualPropertiesDirtyFlags != CLEAN_FLAG) // We don't check mUpdateDecay
  {
    SetUpdated(true);
  }

  // Ensure collected map is up to date
  UpdateUniformMap(updateBufferIndex);

  return rendererUpdated;
}

void Renderer::SetTextures(TextureSet* textureSet)
{
  DALI_ASSERT_DEBUG(textureSet != NULL && "Texture set pointer is NULL");

  if(mTextureSet != textureSet)
  {
    mTextureSet = textureSet;

    SetUpdated(true);
  }
}

const Vector<Render::TextureKey>* Renderer::GetTextures() const
{
  return mTextureSet ? &(mTextureSet->GetTextures()) : nullptr;
}

const Vector<Render::Sampler*>* Renderer::GetSamplers() const
{
  return mTextureSet ? &(mTextureSet->GetSamplers()) : nullptr;
}

void Renderer::SetShader(Shader* shader)
{
  DALI_ASSERT_DEBUG(shader != NULL && "Shader pointer is NULL");

  if(mShader != shader)
  {
    mShader                 = shader;
    mShaderMapChangeCounter = 0u;
    mRegenerateUniformMap   = true;

    CallRenderFunction(mRenderer, &Render::Renderer::SetShaderChanged, true);
    SetUpdated(true);
  }
}

void Renderer::SetGeometry(Render::Geometry* geometry)
{
  DALI_ASSERT_DEBUG(geometry != NULL && "Geometry pointer is NULL");
  if(mGeometry != geometry)
  {
    mGeometry = geometry;

    CallRenderFunction(mRenderer, &Render::Renderer::SetGeometry, mGeometry);
    SetUpdated(true);
  }
}

void Renderer::SetDepthIndex(int depthIndex)
{
  if(mDepthIndex != depthIndex)
  {
    mDepthIndex = depthIndex;

    SetUpdated(true);
  }
}

void Renderer::SetFaceCullingMode(FaceCullingMode::Type faceCullingMode)
{
  if(mFaceCullingMode != faceCullingMode)
  {
    mFaceCullingMode = faceCullingMode;

    CallRenderFunction(mRenderer, &Render::Renderer::SetFaceCullingMode, mFaceCullingMode);
    SetUpdated(true);
  }
}

FaceCullingMode::Type Renderer::GetFaceCullingMode() const
{
  return mFaceCullingMode;
}

void Renderer::SetBlendMode(BlendMode::Type blendingMode)
{
  if(mBlendMode != blendingMode)
  {
    mBlendMode = blendingMode;

    SetUpdated(true);
  }
}

BlendMode::Type Renderer::GetBlendMode() const
{
  return mBlendMode;
}

void Renderer::SetBlendingOptions(uint32_t options)
{
  if(mBlendBitmask != options)
  {
    mBlendBitmask = options;

    CallRenderFunction(mRenderer, &Render::Renderer::SetBlendingBitMask, mBlendBitmask);
    SetUpdated(true);
  }
}

uint32_t Renderer::GetBlendingOptions() const
{
  return mBlendBitmask;
}

void Renderer::SetBlendColor(const Vector4& blendColor)
{
  if(blendColor == Color::TRANSPARENT)
  {
    mBlendColor = nullptr;
  }
  else
  {
    if(!mBlendColor)
    {
      mBlendColor = new Vector4(blendColor);
    }
    else
    {
      *mBlendColor = blendColor;
    }
  }

  CallRenderFunction(mRenderer, &Render::Renderer::SetBlendColor, GetBlendColor());
  SetUpdated(true);
}

Vector4 Renderer::GetBlendColor() const
{
  if(mBlendColor)
  {
    return *mBlendColor;
  }
  return Color::TRANSPARENT;
}

void Renderer::SetIndexedDrawFirstElement(uint32_t firstElement)
{
  if(mIndexedDrawFirstElement != firstElement)
  {
    mIndexedDrawFirstElement = firstElement;

    CallRenderFunction(mRenderer, &Render::Renderer::SetIndexedDrawFirstElement, mIndexedDrawFirstElement);
    SetUpdated(true);
  }
}

uint32_t Renderer::GetIndexedDrawFirstElement() const
{
  return mIndexedDrawFirstElement;
}

void Renderer::SetIndexedDrawElementsCount(uint32_t elementsCount)
{
  if(mIndexedDrawElementsCount != elementsCount)
  {
    mIndexedDrawElementsCount = elementsCount;

    CallRenderFunction(mRenderer, &Render::Renderer::SetIndexedDrawElementsCount, mIndexedDrawElementsCount);
    SetUpdated(true);
  }
}

uint32_t Renderer::GetIndexedDrawElementsCount() const
{
  return mIndexedDrawElementsCount;
}

void Renderer::EnablePreMultipliedAlpha(bool preMultipled)
{
  if(mPremultipledAlphaEnabled != preMultipled)
  {
    mPremultipledAlphaEnabled = preMultipled;

    CallRenderFunction(mRenderer, &Render::Renderer::EnablePreMultipliedAlpha, mPremultipledAlphaEnabled);
    SetUpdated(true);
  }
}

bool Renderer::IsPreMultipliedAlphaEnabled() const
{
  return mPremultipledAlphaEnabled;
}

void Renderer::SetDepthWriteMode(DepthWriteMode::Type depthWriteMode)
{
  mDepthWriteMode = depthWriteMode;

  CallRenderFunction(mRenderer, &Render::Renderer::SetDepthWriteMode, mDepthWriteMode);
}

DepthWriteMode::Type Renderer::GetDepthWriteMode() const
{
  return mDepthWriteMode;
}

void Renderer::SetDepthTestMode(DepthTestMode::Type depthTestMode)
{
  mDepthTestMode = depthTestMode;

  CallRenderFunction(mRenderer, &Render::Renderer::SetDepthTestMode, mDepthTestMode);
}

DepthTestMode::Type Renderer::GetDepthTestMode() const
{
  return mDepthTestMode;
}

void Renderer::SetDepthFunction(DepthFunction::Type depthFunction)
{
  mDepthFunction = depthFunction;

  CallRenderFunction(mRenderer, &Render::Renderer::SetDepthFunction, mDepthFunction);
}

DepthFunction::Type Renderer::GetDepthFunction() const
{
  return mDepthFunction;
}

void Renderer::SetRenderMode(RenderMode::Type mode)
{
  mStencilParameters.renderMode = mode;

  CallRenderFunction(mRenderer, &Render::Renderer::SetRenderMode, mStencilParameters.renderMode);
}

void Renderer::SetStencilFunction(StencilFunction::Type stencilFunction)
{
  mStencilParameters.stencilFunction = stencilFunction;

  CallRenderFunction(mRenderer, &Render::Renderer::SetStencilFunction, mStencilParameters.stencilFunction);
}

void Renderer::SetStencilFunctionMask(int stencilFunctionMask)
{
  mStencilParameters.stencilFunctionMask = stencilFunctionMask;

  CallRenderFunction(mRenderer, &Render::Renderer::SetStencilFunctionMask, mStencilParameters.stencilFunctionMask);
}

void Renderer::SetStencilFunctionReference(int stencilFunctionReference)
{
  mStencilParameters.stencilFunctionReference = stencilFunctionReference;

  CallRenderFunction(mRenderer, &Render::Renderer::SetStencilFunctionReference, mStencilParameters.stencilFunctionReference);
}

void Renderer::SetStencilMask(int stencilMask)
{
  mStencilParameters.stencilMask = stencilMask;

  CallRenderFunction(mRenderer, &Render::Renderer::SetStencilMask, mStencilParameters.stencilMask);
}

void Renderer::SetStencilOperationOnFail(StencilOperation::Type stencilOperationOnFail)
{
  mStencilParameters.stencilOperationOnFail = stencilOperationOnFail;

  CallRenderFunction(mRenderer, &Render::Renderer::SetStencilOperationOnFail, mStencilParameters.stencilOperationOnFail);
}

void Renderer::SetStencilOperationOnZFail(StencilOperation::Type stencilOperationOnZFail)
{
  mStencilParameters.stencilOperationOnZFail = stencilOperationOnZFail;

  CallRenderFunction(mRenderer, &Render::Renderer::SetStencilOperationOnZFail, mStencilParameters.stencilOperationOnZFail);
}

void Renderer::SetStencilOperationOnZPass(StencilOperation::Type stencilOperationOnZPass)
{
  mStencilParameters.stencilOperationOnZPass = stencilOperationOnZPass;

  CallRenderFunction(mRenderer, &Render::Renderer::SetStencilOperationOnZPass, mStencilParameters.stencilOperationOnZPass);
}

void Renderer::SetUpdateAreaExtents(const Dali::Extents& updateAreaExtents)
{
  if(mUpdateAreaExtents != updateAreaExtents)
  {
    mUpdateAreaExtents = updateAreaExtents;
    SetUpdated(true);
  }
}

void Renderer::SetRenderCallback(RenderCallback* callback)
{
  if(mRenderCallback != callback)
  {
    mRenderCallback = callback;

    CallRenderFunction(mRenderer, &Render::Renderer::SetRenderCallback, mRenderCallback);
    SetUpdated(true);
  }
}

void Renderer::TerminateRenderCallback(bool invokeCallback)
{
  CallRenderFunction(mRenderer, &Render::Renderer::TerminateRenderCallback, invokeCallback);
  SetRenderCallback(nullptr);
}

const Render::Renderer::StencilParameters& Renderer::GetStencilParameters() const
{
  return mStencilParameters;
}

void Renderer::BakeMixColor(BufferIndex updateBufferIndex, const Vector4& mixColor)
{
  mMixColor.Bake(updateBufferIndex, mixColor);

  SetUpdated(true);
}

void Renderer::BakeMixColorComponent(BufferIndex updateBufferIndex, float componentValue, uint8_t componentIndex)
{
  switch(componentIndex)
  {
    case 0:
    {
      mMixColor.BakeX(updateBufferIndex, componentValue);
      break;
    }
    case 1:
    {
      mMixColor.BakeY(updateBufferIndex, componentValue);
      break;
    }
    case 2:
    {
      mMixColor.BakeZ(updateBufferIndex, componentValue);
      break;
    }
    case 3:
    {
      mMixColor.BakeW(updateBufferIndex, componentValue);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(0 && "Invalid mix color component comes!");
      break;
    }
  }

  SetUpdated(true);
}

Vector4 Renderer::GetMixColor(BufferIndex updateBufferIndex) const
{
  return mMixColor[updateBufferIndex];
}

void Renderer::SetRenderingBehavior(DevelRenderer::Rendering::Type renderingBehavior)
{
  if(mRenderingBehavior != renderingBehavior)
  {
    mRenderingBehavior = renderingBehavior;

    SetUpdated(true);
  }
}

DevelRenderer::Rendering::Type Renderer::GetRenderingBehavior() const
{
  return mRenderingBehavior;
}

void Renderer::AttachToNode(const Node& node)
{
  if(mVisualProperties)
  {
    DALI_ASSERT_ALWAYS(mVisualProperties->mAttachedNode == nullptr && "VisualRenderer don't allow to attach multiple nodes!");
    mVisualProperties->mAttachedNode = &node;
  }
}

void Renderer::DetachFromNode(const Node& node)
{
  if(mRenderer)
  {
    Render::Renderer* rendererPtr = mRenderer.Get();
    rendererPtr->DetachFromNodeDataProvider(static_cast<const NodeDataProvider&>(node));
  }

  if(mVisualProperties)
  {
    DALI_ASSERT_ALWAYS((mVisualProperties->mAttachedNode == &node) && "Detaching node which was not attached!");
    mVisualProperties->mAttachedNode = nullptr;
  }
}

// Called when SceneGraph::Renderer is added to update manager ( that happens when an "event-thread renderer" is created )
void Renderer::ConnectToSceneGraph(RenderManagerDispatcher& renderManagerDispacher)
{
  mRegenerateUniformMap = true;

  mRenderer = Render::Renderer::NewKey(this, mGeometry, mBlendBitmask, GetBlendColor(), static_cast<FaceCullingMode::Type>(mFaceCullingMode), mPremultipledAlphaEnabled, mDepthWriteMode, mDepthTestMode, mDepthFunction, mStencilParameters);

  OwnerKeyType<Render::Renderer> transferKeyOwnership(mRenderer);
  renderManagerDispacher.AddRenderer(transferKeyOwnership);
}

// Called just before destroying the scene-graph renderer ( when the "event-thread renderer" is no longer referenced )
void Renderer::DisconnectFromSceneGraph(RenderManagerDispatcher& renderManagerDispacher)
{
  // Remove renderer from RenderManager
  if(mRenderer)
  {
    renderManagerDispacher.RemoveRenderer(mRenderer);
    mRenderer = Render::RendererKey{};
  }
}

Render::RendererKey Renderer::GetRenderer() const
{
  return mRenderer;
}

Renderer::OpacityType Renderer::GetOpacityType(BufferIndex updateBufferIndex, uint32_t renderPass, const Node& node) const
{
  Renderer::OpacityType opacityType = Renderer::OPAQUE;

  if(node.IsTransparent())
  {
    return Renderer::TRANSPARENT;
  }

  switch(mBlendMode)
  {
    case BlendMode::ON_WITHOUT_CULL: // If the renderer should always be use blending and never want to be transparent by alpha.
    {
      opacityType = Renderer::TRANSLUCENT;
      break;
    }
    case BlendMode::ON: // If the renderer should always be use blending
    {
      float alpha = node.GetWorldColor().a * mMixColor[updateBufferIndex].a;
      if(alpha <= FULLY_TRANSPARENT)
      {
        opacityType = Renderer::TRANSPARENT;
      }
      else
      {
        opacityType = Renderer::TRANSLUCENT;
      }
      break;
    }
    case BlendMode::AUTO:
    {
      if(BlendingOptions::IsAdvancedBlendEquationIncluded(mBlendBitmask))
      {
        opacityType = Renderer::TRANSLUCENT;
        break;
      }

      if(mTextureSet && mTextureSet->HasAlpha())
      {
        opacityType = Renderer::TRANSLUCENT;
      }
      else
      {
        const auto& shaderData = mShader->GetShaderData(renderPass);
        if(shaderData && shaderData->HintEnabled(Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT))
        {
          opacityType = Renderer::TRANSLUCENT;
        }
      }

      // renderer should determine opacity using the actor color
      float alpha = node.GetWorldColor().a * mMixColor[updateBufferIndex].a;
      if(alpha <= FULLY_TRANSPARENT)
      {
        opacityType = Renderer::TRANSPARENT;
      }
      else if(alpha <= FULLY_OPAQUE)
      {
        opacityType = Renderer::TRANSLUCENT;
      }

      break;
    }
    case BlendMode::USE_ACTOR_OPACITY: // the renderer should never use blending
    {
      // renderer should determine opacity using the actor color
      float alpha = node.GetWorldColor().a;
      if(alpha <= FULLY_TRANSPARENT)
      {
        opacityType = Renderer::TRANSPARENT;
      }
      else if(alpha < FULLY_OPAQUE)
      {
        opacityType = Renderer::TRANSLUCENT;
      }
      else
      {
        opacityType = Renderer::OPAQUE;
      }
      break;
    }
    case BlendMode::OFF: // the renderer should never use blending
    default:
    {
      opacityType = Renderer::OPAQUE;
      break;
    }
  }

  return opacityType;
}

void Renderer::UpdateUniformMap(BufferIndex updateBufferIndex)
{
  if(mRegenerateUniformMap)
  {
    CollectedUniformMap& localMap = mCollectedUniformMap;
    localMap.Clear();

    const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();

    auto size = rendererUniformMap.Count();
    if(mShader)
    {
      size += mShader->GetUniformMap().Count();
    }

    localMap.Reserve(size);
    localMap.AddMappings(rendererUniformMap);
    if(mShader)
    {
      localMap.AddMappings(mShader->GetUniformMap());
    }
    localMap.UpdateChangeCounter();

    mRegenerateUniformMap = false;
    SetUpdated(true);
  }
}

void Renderer::SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size)
{
  CallRenderFunction(mRenderer, &Render::Renderer::SetDrawCommands, pDrawCommands, size);
}

bool Renderer::IsDirty() const
{
  // Check whether the opacity property has changed
  CheckDirtyUpdated();
  return mDirtyUpdated & IS_DIRTY_MASK;
}

void Renderer::OnMappingChanged()
{
  // Properties have been registered on the base class.
  mRegenerateUniformMap = true; // Should remain true until this renderer is added to a RenderList.
}

const CollectedUniformMap& Renderer::GetCollectedUniformMap() const
{
  return mCollectedUniformMap;
}

void Renderer::EnableSharedUniformBlock(bool enabled)
{
  if(mUseSharedUniformBlock != enabled)
  {
    mUseSharedUniformBlock = enabled;
    CallRenderFunction(mRenderer, &Render::Renderer::EnableSharedUniformBlock, mUseSharedUniformBlock);
    SetUpdated(true);
  }
}

bool Renderer::IsUpdated() const
{
  // We should check Whether
  // 1. Renderer itself's property changed
  // 2. Renderer's opacity changed
  // 3. Shader's propperties or UniformBlock's properties are changed
  // 4. Visual properties are changed
  CheckDirtyUpdated();
  return mDirtyUpdated & IS_UPDATED_MASK;
}

void Renderer::CheckDirtyUpdated() const
{
  if(mDirtyUpdated == NOT_CHECKED)
  {
    mDirtyUpdated |= CHECKED;
    mDirtyUpdated |= (Updated() || !mMixColor.IsClean()) << DIRTY_FLAG_SHIFT;
    mDirtyUpdated |= ((mShader && mShader->IsUpdated()) || (mVisualPropertiesDirtyFlags != CLEAN_FLAG)) << UPDATED_FLAG_SHIFT;

    // TODO : Can we skip this reset?
    RequestResetUpdated();
  }
}

void Renderer::ResetUpdated()
{
  mDirtyUpdated = NOT_CHECKED;
  PropertyOwner::ResetUpdated();
}

Vector4 Renderer::GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, const Vector4& originalUpdateArea) noexcept
{
  if(mVisualProperties)
  {
    return AdjustExtents(mVisualProperties->GetVisualTransformedUpdateArea(updateBufferIndex, originalUpdateArea), mUpdateAreaExtents);
  }
  return AdjustExtents(originalUpdateArea, mUpdateAreaExtents);
}

bool Renderer::IsObservingNodeDeactivated() const
{
  // TODO : Could we use this feature for general renderer?
  if(mVisualProperties)
  {
    return !mVisualProperties->mAttachedNode || mVisualProperties->mAttachedNode->IsWorldIgnored();
  }
  return false;
}

void Renderer::OnVisualRendererPropertyUpdated(bool bake)
{
  mVisualPropertiesDirtyFlags |= bake ? BAKED_FLAG : SET_FLAG;
}

uint8_t Renderer::GetUpdatedFlag() const
{
  return static_cast<uint8_t>(mVisualPropertiesDirtyFlags);
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
