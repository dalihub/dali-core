#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER_MESSAGES_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER_MESSAGES_H

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

#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace Dali::Internal::SceneGraph
{
class TextureSet;

/// Messages
inline void SetTexturesMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, const TextureSet& textureSet)
{
  using LocalType = MessageValue1<Renderer, TextureSet*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&renderer, &Renderer::SetTextures, const_cast<TextureSet*>(&textureSet));
}

inline void SetGeometryMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, const Render::Geometry& geometry)
{
  using LocalType = MessageValue1<Renderer, Render::Geometry*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&renderer, &Renderer::SetGeometry, const_cast<Render::Geometry*>(&geometry));
}

inline void SetShaderMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, const Shader& shader)
{
  using LocalType = MessageValue1<Renderer, Shader*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&renderer, &Renderer::SetShader, const_cast<Shader*>(&shader));
}

inline void SetDepthIndexMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, int depthIndex)
{
  using LocalType = MessageValue1<Renderer, int>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&renderer, &Renderer::SetDepthIndex, depthIndex);
}

inline void SetFaceCullingModeMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, FaceCullingMode::Type faceCullingMode)
{
  using LocalType = MessageValue1<Renderer, FaceCullingMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetFaceCullingMode, faceCullingMode);
}

inline void SetBlendModeMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, BlendMode::Type blendingMode)
{
  using LocalType = MessageValue1<Renderer, BlendMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetBlendMode, blendingMode);
}

inline void SetBlendingOptionsMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, uint32_t options)
{
  using LocalType = MessageValue1<Renderer, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetBlendingOptions, options);
}

inline void SetBlendColorMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, const Vector4& blendColor)
{
  using LocalType = MessageValue1<Renderer, Vector4>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetBlendColor, blendColor);
}

inline void SetIndexedDrawFirstElementMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, uint32_t firstElement)
{
  using LocalType = MessageValue1<Renderer, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetIndexedDrawFirstElement, firstElement);
}

inline void SetIndexedDrawElementsCountMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, uint32_t elementsCount)
{
  using LocalType = MessageValue1<Renderer, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetIndexedDrawElementsCount, elementsCount);
}

inline void SetEnablePreMultipliedAlphaMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, bool preMultiplied)
{
  using LocalType = MessageValue1<Renderer, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::EnablePreMultipliedAlpha, preMultiplied);
}

inline void SetDepthWriteModeMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, DepthWriteMode::Type depthWriteMode)
{
  using LocalType = MessageValue1<Renderer, DepthWriteMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetDepthWriteMode, depthWriteMode);
}

inline void SetDepthTestModeMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, DepthTestMode::Type depthTestMode)
{
  using LocalType = MessageValue1<Renderer, DepthTestMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetDepthTestMode, depthTestMode);
}

inline void SetDepthFunctionMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, DepthFunction::Type depthFunction)
{
  using LocalType = MessageValue1<Renderer, DepthFunction::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetDepthFunction, depthFunction);
}

inline void SetRenderModeMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, RenderMode::Type mode)
{
  using LocalType = MessageValue1<Renderer, RenderMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetRenderMode, mode);
}

inline void SetStencilFunctionMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, StencilFunction::Type stencilFunction)
{
  using LocalType = MessageValue1<Renderer, StencilFunction::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetStencilFunction, stencilFunction);
}

inline void SetStencilFunctionMaskMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, int mask)
{
  using LocalType = MessageValue1<Renderer, int>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetStencilFunctionMask, mask);
}

inline void SetStencilFunctionReferenceMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, int stencilFunctionReference)
{
  using LocalType = MessageValue1<Renderer, int>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetStencilFunctionReference, stencilFunctionReference);
}

inline void SetStencilMaskMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, int stencilMask)
{
  using LocalType = MessageValue1<Renderer, int>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetStencilMask, stencilMask);
}

inline void SetStencilOperationOnFailMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, StencilOperation::Type stencilOperation)
{
  using LocalType = MessageValue1<Renderer, StencilOperation::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetStencilOperationOnFail, stencilOperation);
}

inline void SetStencilOperationOnZFailMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, StencilOperation::Type stencilOperation)
{
  using LocalType = MessageValue1<Renderer, StencilOperation::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetStencilOperationOnZFail, stencilOperation);
}

inline void SetStencilOperationOnZPassMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, StencilOperation::Type stencilOperation)
{
  using LocalType = MessageValue1<Renderer, StencilOperation::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetStencilOperationOnZPass, stencilOperation);
}

inline void BakeOpacityMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, float opacity)
{
  using LocalType = MessageDoubleBuffered1<Renderer, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::BakeOpacity, opacity);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<Renderer*>(&renderer),
                                  const_cast<AnimatableProperty<float>*>(&renderer.mOpacity),
                                  SceneGraph::BakerResetter::Lifetime::BAKE));
  SceneGraph::AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

inline void SetRenderingBehaviorMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, DevelRenderer::Rendering::Type renderingBehavior)
{
  using LocalType = MessageValue1<Renderer, DevelRenderer::Rendering::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetRenderingBehavior, renderingBehavior);
}

inline void SetDrawCommandsMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size)
{
  using LocalType = MessageValue2<Renderer, Dali::DevelRenderer::DrawCommand*, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetDrawCommands, pDrawCommands, size);
}

inline void SetRenderCallbackMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, Dali::RenderCallback* callback)
{
  using LocalType = MessageValue1<Renderer, Dali::RenderCallback*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  new(slot) LocalType(&renderer, &Renderer::SetRenderCallback, callback);
}

inline void SetInstanceCountMessage(EventThreadServices& eventThreadServices, const Renderer& renderer, uint32_t instanceCount)
{
  using LocalType = MessageValue1<SceneGraph::Renderer, uint32_t>;
  uint32_t* slot  = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));
  new(slot) LocalType(&renderer, &SceneGraph::Renderer::SetInstanceCount, instanceCount);
}

} // namespace Dali::Internal::SceneGraph

#endif //DALI_INTERNAL_SCENE_GRAPH_RENDERER_MESSAGES_H
