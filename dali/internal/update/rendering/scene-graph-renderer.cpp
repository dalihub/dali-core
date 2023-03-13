/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include "scene-graph-renderer.h"

// INTERNAL INCLUDES
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/render-shader.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace // unnamed namespace
{
#ifdef DEBUG_ENABLED
Debug::Filter* gSceneGraphRendererLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SG_RENDERER");
#endif

// Memory pool used to allocate new renderers. Memory used by this pool will be released when shutting down DALi
MemoryPoolObjectAllocator<Renderer>& GetRendererMemoryPool()
{
  static MemoryPoolObjectAllocator<Renderer> gRendererMemoryPool;
  return gRendererMemoryPool;
}

// Flags for re-sending data to renderer.
enum Flags
{
  RESEND_GEOMETRY                    = 1 << 0,
  RESEND_FACE_CULLING_MODE           = 1 << 1,
  RESEND_BLEND_COLOR                 = 1 << 2,
  RESEND_BLEND_BIT_MASK              = 1 << 3,
  RESEND_PREMULTIPLIED_ALPHA         = 1 << 4,
  RESEND_INDEXED_DRAW_FIRST_ELEMENT  = 1 << 5,
  RESEND_INDEXED_DRAW_ELEMENTS_COUNT = 1 << 6,
  RESEND_DEPTH_WRITE_MODE            = 1 << 7,
  RESEND_DEPTH_TEST_MODE             = 1 << 8,
  RESEND_DEPTH_FUNCTION              = 1 << 9,
  RESEND_RENDER_MODE                 = 1 << 10,
  RESEND_STENCIL_FUNCTION            = 1 << 11,
  RESEND_STENCIL_FUNCTION_MASK       = 1 << 12,
  RESEND_STENCIL_FUNCTION_REFERENCE  = 1 << 13,
  RESEND_STENCIL_MASK                = 1 << 14,
  RESEND_STENCIL_OPERATION_ON_FAIL   = 1 << 15,
  RESEND_STENCIL_OPERATION_ON_Z_FAIL = 1 << 16,
  RESEND_STENCIL_OPERATION_ON_Z_PASS = 1 << 17,
  RESEND_WRITE_TO_COLOR_BUFFER       = 1 << 18,
  RESEND_SHADER                      = 1 << 19,
  RESEND_DRAW_COMMANDS               = 1 << 20,
  RESEND_SET_RENDER_CALLBACK         = 1 << 21
};

} // Anonymous namespace

RendererKey Renderer::NewKey()
{
  void* ptr = GetRendererMemoryPool().AllocateRawThreadSafe();
  auto  key = GetRendererMemoryPool().GetKeyFromPtr(static_cast<Renderer*>(ptr));
  new(ptr) Renderer();
  return RendererKey(key);
}

Renderer::Renderer()
: mSceneController(nullptr),
  mRenderer{},
  mTextureSet(nullptr),
  mGeometry(nullptr),
  mShader(nullptr),
  mBlendColor(nullptr),
  mStencilParameters(RenderMode::AUTO, StencilFunction::ALWAYS, 0xFF, 0x00, 0xFF, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP),
  mIndexedDrawFirstElement(0u),
  mIndexedDrawElementsCount(0u),
  mBlendBitmask(0u),
  mResendFlag(0u),
  mDepthFunction(DepthFunction::LESS),
  mFaceCullingMode(FaceCullingMode::NONE),
  mBlendMode(BlendMode::AUTO),
  mDepthWriteMode(DepthWriteMode::AUTO),
  mDepthTestMode(DepthTestMode::AUTO),
  mRenderingBehavior(DevelRenderer::Rendering::IF_REQUIRED),
  mUpdateDecay(Renderer::Decay::INITIAL),
  mRegenerateUniformMap(false),
  mPremultipledAlphaEnabled(false),
  mDirtyFlag(true),
  mOpacity(1.0f),
  mDepthIndex(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::operator delete(void* ptr)
{
  GetRendererMemoryPool().FreeThreadSafe(static_cast<Renderer*>(ptr));
}

Renderer* Renderer::Get(RendererKey::KeyType rendererKey)
{
  return GetRendererMemoryPool().GetPtrFromKey(rendererKey);
}

RendererKey Renderer::GetKey(const Renderer& renderer)
{
  return RendererKey(GetRendererMemoryPool().GetKeyFromPtr(const_cast<Renderer*>(&renderer)));
}

RendererKey Renderer::GetKey(Renderer* renderer)
{
  return RendererKey(GetRendererMemoryPool().GetKeyFromPtr(renderer));
}

bool Renderer::PrepareRender(BufferIndex updateBufferIndex)
{
  bool rendererUpdated        = mResendFlag || mRenderingBehavior == DevelRenderer::Rendering::CONTINUOUSLY || mUpdateDecay > 0;
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

  if(mResendFlag != 0)
  {
    Render::Renderer* rendererPtr = mRenderer.Get();
    if(mResendFlag & RESEND_GEOMETRY)
    {
      typedef MessageValue1<Render::Renderer, Render::Geometry*> DerivedType;
      uint32_t*                                                  slot = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetGeometry, mGeometry);
    }

    if(mResendFlag & RESEND_DRAW_COMMANDS)
    {
      using DerivedType = MessageValue2<Render::Renderer, Dali::DevelRenderer::DrawCommand*, uint32_t>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetDrawCommands, mDrawCommands.data(), mDrawCommands.size());
    }

    if(mResendFlag & RESEND_FACE_CULLING_MODE)
    {
      using DerivedType = MessageValue1<Render::Renderer, FaceCullingMode::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetFaceCullingMode, mFaceCullingMode);
    }

    if(mResendFlag & RESEND_BLEND_BIT_MASK)
    {
      using DerivedType = MessageValue1<Render::Renderer, uint32_t>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetBlendingBitMask, mBlendBitmask);
    }

    if(mResendFlag & RESEND_BLEND_COLOR)
    {
      using DerivedType = MessageValue1<Render::Renderer, Vector4>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetBlendColor, GetBlendColor());
    }

    if(mResendFlag & RESEND_PREMULTIPLIED_ALPHA)
    {
      using DerivedType = MessageValue1<Render::Renderer, bool>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::EnablePreMultipliedAlpha, mPremultipledAlphaEnabled);
    }

    if(mResendFlag & RESEND_INDEXED_DRAW_FIRST_ELEMENT)
    {
      using DerivedType = MessageValue1<Render::Renderer, uint32_t>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetIndexedDrawFirstElement, mIndexedDrawFirstElement);
    }

    if(mResendFlag & RESEND_INDEXED_DRAW_ELEMENTS_COUNT)
    {
      using DerivedType = MessageValue1<Render::Renderer, uint32_t>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetIndexedDrawElementsCount, mIndexedDrawElementsCount);
    }

    if(mResendFlag & RESEND_DEPTH_WRITE_MODE)
    {
      using DerivedType = MessageValue1<Render::Renderer, DepthWriteMode::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetDepthWriteMode, mDepthWriteMode);
    }

    if(mResendFlag & RESEND_DEPTH_TEST_MODE)
    {
      using DerivedType = MessageValue1<Render::Renderer, DepthTestMode::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetDepthTestMode, mDepthTestMode);
    }

    if(mResendFlag & RESEND_DEPTH_FUNCTION)
    {
      using DerivedType = MessageValue1<Render::Renderer, DepthFunction::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetDepthFunction, mDepthFunction);
    }

    if(mResendFlag & RESEND_RENDER_MODE)
    {
      using DerivedType = MessageValue1<Render::Renderer, RenderMode::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetRenderMode, mStencilParameters.renderMode);
    }

    if(mResendFlag & RESEND_STENCIL_FUNCTION)
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilFunction::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetStencilFunction, mStencilParameters.stencilFunction);
    }

    if(mResendFlag & RESEND_STENCIL_FUNCTION_MASK)
    {
      using DerivedType = MessageValue1<Render::Renderer, int>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetStencilFunctionMask, mStencilParameters.stencilFunctionMask);
    }

    if(mResendFlag & RESEND_STENCIL_FUNCTION_REFERENCE)
    {
      using DerivedType = MessageValue1<Render::Renderer, int>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetStencilFunctionReference, mStencilParameters.stencilFunctionReference);
    }

    if(mResendFlag & RESEND_STENCIL_MASK)
    {
      using DerivedType = MessageValue1<Render::Renderer, int>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetStencilMask, mStencilParameters.stencilMask);
    }

    if(mResendFlag & RESEND_STENCIL_OPERATION_ON_FAIL)
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilOperation::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetStencilOperationOnFail, mStencilParameters.stencilOperationOnFail);
    }

    if(mResendFlag & RESEND_STENCIL_OPERATION_ON_Z_FAIL)
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilOperation::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetStencilOperationOnZFail, mStencilParameters.stencilOperationOnZFail);
    }

    if(mResendFlag & RESEND_STENCIL_OPERATION_ON_Z_PASS)
    {
      using DerivedType = MessageValue1<Render::Renderer, StencilOperation::Type>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetStencilOperationOnZPass, mStencilParameters.stencilOperationOnZPass);
    }

    if(mResendFlag & RESEND_SHADER)
    {
      using DerivedType = MessageValue1<Render::Renderer, bool>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetShaderChanged, true);
    }

    if(mResendFlag & RESEND_SET_RENDER_CALLBACK)
    {
      using DerivedType = MessageValue1<Render::Renderer, Dali::RenderCallback*>;
      uint32_t* slot    = mSceneController->GetRenderQueue().ReserveMessageSlot(updateBufferIndex, sizeof(DerivedType));
      new(slot) DerivedType(rendererPtr, &Render::Renderer::SetRenderCallback, mRenderCallback);
    }

    SetUpdated(true);

    mResendFlag = 0;
  }

  // Ensure collected map is up to date
  UpdateUniformMap(updateBufferIndex);

  return rendererUpdated;
}

void Renderer::SetTextures(TextureSet* textureSet)
{
  DALI_ASSERT_DEBUG(textureSet != NULL && "Texture set pointer is NULL");

  mTextureSet = textureSet;

  mDirtyFlag = true;
  SetUpdated(true);
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

  mShader                 = shader;
  mShaderMapChangeCounter = 0u;
  mRegenerateUniformMap   = true;
  mResendFlag |= RESEND_GEOMETRY | RESEND_SHADER;
  mDirtyFlag = true;
}

void Renderer::SetGeometry(Render::Geometry* geometry)
{
  DALI_ASSERT_DEBUG(geometry != NULL && "Geometry pointer is NULL");
  mGeometry = geometry;

  if(mRenderer)
  {
    mResendFlag |= RESEND_GEOMETRY;
  }
}

void Renderer::SetDepthIndex(int depthIndex)
{
  mDepthIndex = depthIndex;

  mDirtyFlag = true;
  SetUpdated(true);
}

void Renderer::SetFaceCullingMode(FaceCullingMode::Type faceCullingMode)
{
  mFaceCullingMode = faceCullingMode;
  mResendFlag |= RESEND_FACE_CULLING_MODE;
}

FaceCullingMode::Type Renderer::GetFaceCullingMode() const
{
  return mFaceCullingMode;
}

void Renderer::SetBlendMode(BlendMode::Type blendingMode)
{
  mBlendMode = blendingMode;

  mDirtyFlag = true;
  SetUpdated(true);
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
    mResendFlag |= RESEND_BLEND_BIT_MASK;
    mDirtyFlag = true;
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

  mResendFlag |= RESEND_BLEND_COLOR;
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
  mIndexedDrawFirstElement = firstElement;
  mResendFlag |= RESEND_INDEXED_DRAW_FIRST_ELEMENT;
}

uint32_t Renderer::GetIndexedDrawFirstElement() const
{
  return mIndexedDrawFirstElement;
}

void Renderer::SetIndexedDrawElementsCount(uint32_t elementsCount)
{
  mIndexedDrawElementsCount = elementsCount;
  mResendFlag |= RESEND_INDEXED_DRAW_ELEMENTS_COUNT;
}

uint32_t Renderer::GetIndexedDrawElementsCount() const
{
  return mIndexedDrawElementsCount;
}

void Renderer::EnablePreMultipliedAlpha(bool preMultipled)
{
  mPremultipledAlphaEnabled = preMultipled;
  mResendFlag |= RESEND_PREMULTIPLIED_ALPHA;
}

bool Renderer::IsPreMultipliedAlphaEnabled() const
{
  return mPremultipledAlphaEnabled;
}

void Renderer::SetDepthWriteMode(DepthWriteMode::Type depthWriteMode)
{
  mDepthWriteMode = depthWriteMode;
  mResendFlag |= RESEND_DEPTH_WRITE_MODE;
}

DepthWriteMode::Type Renderer::GetDepthWriteMode() const
{
  return mDepthWriteMode;
}

void Renderer::SetDepthTestMode(DepthTestMode::Type depthTestMode)
{
  mDepthTestMode = depthTestMode;
  mResendFlag |= RESEND_DEPTH_TEST_MODE;
}

DepthTestMode::Type Renderer::GetDepthTestMode() const
{
  return mDepthTestMode;
}

void Renderer::SetDepthFunction(DepthFunction::Type depthFunction)
{
  mDepthFunction = depthFunction;
  mResendFlag |= RESEND_DEPTH_FUNCTION;
}

DepthFunction::Type Renderer::GetDepthFunction() const
{
  return mDepthFunction;
}

void Renderer::SetRenderMode(RenderMode::Type mode)
{
  mStencilParameters.renderMode = mode;
  mResendFlag |= RESEND_RENDER_MODE;
}

void Renderer::SetStencilFunction(StencilFunction::Type stencilFunction)
{
  mStencilParameters.stencilFunction = stencilFunction;
  mResendFlag |= RESEND_STENCIL_FUNCTION;
}

void Renderer::SetStencilFunctionMask(int stencilFunctionMask)
{
  mStencilParameters.stencilFunctionMask = stencilFunctionMask;
  mResendFlag |= RESEND_STENCIL_FUNCTION_MASK;
}

void Renderer::SetStencilFunctionReference(int stencilFunctionReference)
{
  mStencilParameters.stencilFunctionReference = stencilFunctionReference;
  mResendFlag |= RESEND_STENCIL_FUNCTION_REFERENCE;
}

void Renderer::SetStencilMask(int stencilMask)
{
  mStencilParameters.stencilMask = stencilMask;
  mResendFlag |= RESEND_STENCIL_MASK;
}

void Renderer::SetStencilOperationOnFail(StencilOperation::Type stencilOperationOnFail)
{
  mStencilParameters.stencilOperationOnFail = stencilOperationOnFail;
  mResendFlag |= RESEND_STENCIL_OPERATION_ON_FAIL;
}

void Renderer::SetStencilOperationOnZFail(StencilOperation::Type stencilOperationOnZFail)
{
  mStencilParameters.stencilOperationOnZFail = stencilOperationOnZFail;
  mResendFlag |= RESEND_STENCIL_OPERATION_ON_Z_FAIL;
}

void Renderer::SetStencilOperationOnZPass(StencilOperation::Type stencilOperationOnZPass)
{
  mStencilParameters.stencilOperationOnZPass = stencilOperationOnZPass;
  mResendFlag |= RESEND_STENCIL_OPERATION_ON_Z_PASS;
}

void Renderer::SetRenderCallback(RenderCallback* callback)
{
  mRenderCallback = callback;
  mResendFlag |= RESEND_SET_RENDER_CALLBACK;
  mDirtyFlag = true;
}

const Render::Renderer::StencilParameters& Renderer::GetStencilParameters() const
{
  return mStencilParameters;
}

void Renderer::BakeOpacity(BufferIndex updateBufferIndex, float opacity)
{
  mOpacity.Bake(updateBufferIndex, opacity);

  mDirtyFlag = true;
  SetUpdated(true);
}

float Renderer::GetOpacity(BufferIndex updateBufferIndex) const
{
  return mOpacity[updateBufferIndex];
}

void Renderer::SetRenderingBehavior(DevelRenderer::Rendering::Type renderingBehavior)
{
  mRenderingBehavior = renderingBehavior;
  SetUpdated(true);
}

DevelRenderer::Rendering::Type Renderer::GetRenderingBehavior() const
{
  return mRenderingBehavior;
}

// Called when SceneGraph::Renderer is added to update manager ( that happens when an "event-thread renderer" is created )
void Renderer::ConnectToSceneGraph(SceneController& sceneController, BufferIndex bufferIndex)
{
  mRegenerateUniformMap = true;
  mSceneController      = &sceneController;

  mRenderer = Render::Renderer::NewKey(this, mGeometry, mBlendBitmask, GetBlendColor(), static_cast<FaceCullingMode::Type>(mFaceCullingMode), mPremultipledAlphaEnabled, mDepthWriteMode, mDepthTestMode, mDepthFunction, mStencilParameters);

  mSceneController->GetRenderMessageDispatcher().AddRenderer(mRenderer);
}

// Called just before destroying the scene-graph renderer ( when the "event-thread renderer" is no longer referenced )
void Renderer::DisconnectFromSceneGraph(SceneController& sceneController, BufferIndex bufferIndex)
{
  // Remove renderer from RenderManager
  if(mRenderer)
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer(mRenderer);
    mRenderer = Render::RendererKey{};
  }
  mSceneController = nullptr;
}

Render::RendererKey Renderer::GetRenderer()
{
  return mRenderer;
}

Renderer::OpacityType Renderer::GetOpacityType(BufferIndex updateBufferIndex, const Node& node) const
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
      float alpha = node.GetWorldColor(updateBufferIndex).a * mOpacity[updateBufferIndex];
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

      bool shaderRequiresBlending(mShader->HintEnabled(Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT));
      if(shaderRequiresBlending || (mTextureSet && mTextureSet->HasAlpha()))
      {
        opacityType = Renderer::TRANSLUCENT;
      }

      // renderer should determine opacity using the actor color
      float alpha = node.GetWorldColor(updateBufferIndex).a * mOpacity[updateBufferIndex];
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
      float alpha = node.GetWorldColor(updateBufferIndex).a;
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

  uint64_t                                  hash                   = 0xc70f6907UL;
  const SceneGraph::UniformMapDataProvider& uniformMapDataProvider = GetUniformMapDataProvider();
  const SceneGraph::CollectedUniformMap&    collectedUniformMap    = uniformMapDataProvider.GetCollectedUniformMap();
  for(uint32_t i = 0u, count = collectedUniformMap.Count(); i < count; ++i)
  {
    hash = collectedUniformMap.mUniformMap[i].propertyPtr->Hash(updateBufferIndex, hash);
  }
  if(mUniformsHash != hash)
  {
    mUniformsHash = hash;
    SetUpdated(true);
  }
}

void Renderer::SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size)
{
  mDrawCommands.clear();
  mDrawCommands.insert(mDrawCommands.end(), pDrawCommands, pDrawCommands + size);
  mResendFlag |= RESEND_DRAW_COMMANDS;
}

bool Renderer::IsDirty() const
{
  // Check whether the opacity property has changed
  return (mDirtyFlag || !mOpacity.IsClean());
}

void Renderer::ResetDirtyFlag()
{
  mDirtyFlag = false;

  SetUpdated(false);
}

uint32_t Renderer::GetMemoryPoolCapacity()
{
  return GetRendererMemoryPool().GetCapacity();
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

Vector4 Renderer::GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, const Vector4& originalUpdateArea) noexcept
{
  if(mVisualProperties)
  {
    auto& coefficient = mVisualProperties->mCoefficient;

    // Recalculate only if coefficient need to be updated.
    if(coefficient.IsUpdated())
    {
      // VisualProperty
      const Vector2 transformOffset         = mVisualProperties->mTransformOffset.Get(updateBufferIndex);
      const Vector4 transformOffsetSizeMode = mVisualProperties->mTransformOffsetSizeMode.Get(updateBufferIndex);
      const Vector2 transformSize           = mVisualProperties->mTransformSize.Get(updateBufferIndex);
      const Vector2 transformOrigin         = mVisualProperties->mTransformOrigin.Get(updateBufferIndex);
      const Vector2 transformAnchorPoint    = mVisualProperties->mTransformAnchorPoint.Get(updateBufferIndex);
      const Vector2 extraSize               = mVisualProperties->mExtraSize.Get(updateBufferIndex);

      DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform size   %5.3f %5.3f\n", transformSize.x, transformSize.y);
      DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform offset %5.3f %5.3f\n", transformOffset.x, transformOffset.y);
      DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform origin %5.3f %5.3f\n", transformOrigin.x, transformOrigin.y);
      DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "transform anchor %5.3f %5.3f\n", transformAnchorPoint.x, transformAnchorPoint.y);
      DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "extra size       %5.3f %5.3f\n", extraSize.x, extraSize.y);

      // const Vector2 visualSize = Vector2(Dali::Lerp(transformOffsetSizeMode.z, originalSize.x * transformSize.x, transformSize.x),
      //                                    Dali::Lerp(transformOffsetSizeMode.w, originalSize.y * transformSize.y, transformSize.y)) +
      //                            extraSize;
      // const Vector2 visualOffset = Vector2(Dali::Lerp(transformOffsetSizeMode.x, originalSize.x * transformOffset.x, transformOffset.x),
      //                                      Dali::Lerp(transformOffsetSizeMode.y, originalSize.y * transformOffset.y, transformOffset.y));

      // const float decoratedBorderlineWidth = std::max((1.0f + Dali::Clamp(borderlineOffset, -1.0f, 1.0f)) * borderlineWidth, 2.0f * blurRadius);
      // const Vector2 decoratedVisualSize    = visualSize + Vector2(decoratedBorderlineWidth, decoratedBorderlineWidth);

      // Note : vertexPositoin.xy = aPosition * decoratedVisualSize
      //                          + anchorPoint * visualSize
      //                          + origin * uSize.xy
      //                          + visualOffset;

      // Calculate same logic of visual's vertex shader transform.
      // minVertexPosition = -0.5f * decoratedVisualSize + transformAnchorPoint * visualSize + transformOrigin * originalSize.xy + visualOffset
      // maxVertexPosition =  0.5f * decoratedVisualSize + transformAnchorPoint * visualSize + transformOrigin * originalSize.xy + visualOffset

      // Update cached VisualTransformedUpdateSizeCoefficientCache

      // Note : vertexPosition = (XA * aPosition + XB) * originalSize + (CA * aPosition + CB) + Vector2(D, D) * aPosition

      // XA = transformSize * (1.0 - transformOffsetSizeMode.zw)
      // XB = transformSize * (1.0 - transformOffsetSizeMode.zw) * transformAnchorPoint
      //    + transformOffset * (1.0 - transformOffsetSizeMode.xy)
      //    + transformOrigin
      // CA = transformSize * transformOffsetSizeMode.zw + extraSize
      // CB = (transformSize * transformOffsetSizeMode.zw + extraSize) * transformAnchorPoint
      //    + transformOffset * transformOffsetSizeMode.xy
      // D = max((1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth, 2.0 * blurRadius)

      coefficient.coefXA = transformSize * Vector2(1.0f - transformOffsetSizeMode.z, 1.0f - transformOffsetSizeMode.w);
      coefficient.coefXB = coefficient.coefXA * transformAnchorPoint + transformOffset * Vector2(1.0f - transformOffsetSizeMode.x, 1.0f - transformOffsetSizeMode.y) + transformOrigin;
      coefficient.coefCA = transformSize * Vector2(transformOffsetSizeMode.z, transformOffsetSizeMode.w) + extraSize;
      coefficient.coefCB = coefficient.coefCA * transformAnchorPoint + transformOffset * Vector2(transformOffsetSizeMode.x, transformOffsetSizeMode.y);
    }

    float coefD = 0.0f; ///< Default as 0.0f when we don't use decorated renderer.

    if(mVisualProperties->mExtendedProperties)
    {
      const auto decoratedVisualProperties = static_cast<VisualRenderer::AnimatableDecoratedVisualProperties*>(mVisualProperties->mExtendedProperties);

      auto& decoratedCoefficient = decoratedVisualProperties->mCoefficient;

      // Recalculate only if coefficient need to be updated.
      if(decoratedCoefficient.IsUpdated())
      {
        // DecoratedVisualProperty
        const float borderlineWidth  = decoratedVisualProperties->mBorderlineWidth.Get(updateBufferIndex);
        const float borderlineOffset = decoratedVisualProperties->mBorderlineOffset.Get(updateBufferIndex);
        const float blurRadius       = decoratedVisualProperties->mBlurRadius.Get(updateBufferIndex);

        DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "borderline width  %5.3f\n", borderlineWidth);
        DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "borderline offset %5.3f\n", borderlineOffset);
        DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "blur radius       %5.3f\n", blurRadius);

        // D coefficients be used only decoratedVisual.
        // It can be calculated parallely with visual transform.
        decoratedCoefficient.coefD = std::max((1.0f + Dali::Clamp(borderlineOffset, -1.0f, 1.0f)) * borderlineWidth, 2.0f * blurRadius);
      }

      // Update coefD so we can use this value out of this scope.
      coefD = decoratedCoefficient.coefD;
    }

    // Calculate vertex position by coefficient
    // It will reduce the number of operations

    // const Vector2 minVertexPosition = (XA * -0.5 + XB) * originalSize + (CA * -0.5 + CB) + Vector2(D, D) * -0.5;
    // const Vector2 maxVertexPosition = (XA * +0.5 + XB) * originalSize + (CA * +0.5 + CB) + Vector2(D, D) * +0.5;

    // When we set
    // basicVertexPosition = XB * originalSize + CB
    // scaleVertexPosition = XA * originalSize + CA + D

    // --> minVertexPosition = basicVertexPosition + scaleVertexPosition * -0.5
    //     maxVertexPosition = basicVertexPosition + scaleVertexPosition * +0.5

    // Then, resultSize = 2.0f * max(-minVertexPosition, maxVertexPosition);
    //                  = 2.0f * max(scaleVertexPosition * 0.5 - basicVertexPosition, scaleVertexPosition * 0.5 + basicVertexPosition)
    //                  = scaleVertexPosition + 2.0f * abs(basicVertexPosition)
    // Cause transform matrix will think center of vertex is (0, 0)

    const Vector2 originalXY = Vector2(originalUpdateArea.x, originalUpdateArea.y);
    const Vector2 originalWH = Vector2(originalUpdateArea.z, originalUpdateArea.w);

    const Vector2 basicVertexPosition = coefficient.coefXB * originalWH + coefficient.coefCB;
    const Vector2 scaleVertexPosition = coefficient.coefXA * originalWH + coefficient.coefCA;

    // TODO : We need to re-generate coefficient to consitder area width/height
    const Vector4 resultArea = Vector4(originalXY.x,
                                       originalXY.y,
                                       scaleVertexPosition.x + 2.0f * abs(basicVertexPosition.x) + coefD,
                                       scaleVertexPosition.y + 2.0f * abs(basicVertexPosition.y) + coefD);

    DALI_LOG_INFO(gSceneGraphRendererLogFilter, Debug::Verbose, "%f %f %f %f--> %f %f %f %f\n", originalUpdateArea.x, originalUpdateArea.y, originalUpdateArea.z, originalUpdateArea.w, resultArea.x, resultArea.y, resultArea.z, resultArea.w);

    return resultArea;
  }
  return originalUpdateArea;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
