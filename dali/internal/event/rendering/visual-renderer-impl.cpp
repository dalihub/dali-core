/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/event/rendering/visual-renderer-impl.h>

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
/**
 * Properties: |name                              |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY("transformOffset", VECTOR2, true, true, true, Dali::VisualRenderer::Property::TRANSFORM_OFFSET)
DALI_PROPERTY("transformSize", VECTOR2, true, true, true, Dali::VisualRenderer::Property::TRANSFORM_SIZE)
DALI_PROPERTY("transformOrigin", VECTOR2, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_ORIGIN)
DALI_PROPERTY("transformAnchorPoint", VECTOR2, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT)
DALI_PROPERTY("transformOffsetSizeMode", VECTOR4, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE)
DALI_PROPERTY("extraSize", VECTOR2, true, true, true, Dali::VisualRenderer::Property::EXTRA_SIZE)
DALI_PROPERTY("visualMixColor", VECTOR3, true, true, true, Dali::VisualRenderer::Property::VISUAL_MIX_COLOR)
DALI_PROPERTY("preMultipliedAlpha", FLOAT, true, false, false, Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA)
DALI_PROPERTY_TABLE_END(Dali::VisualRenderer::Property::DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX, VisualRendererDefaultProperties)

// Property string to enumeration tables:

DALI_ENUM_TO_STRING_TABLE_BEGIN(TRANSFORM_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::VisualRenderer::TransformPolicy::Type, RELATIVE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::VisualRenderer::TransformPolicy::Type, ABSOLUTE)
DALI_ENUM_TO_STRING_TABLE_END(TRANSFORM_POLICY)

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType(typeid(Dali::VisualRenderer), typeid(Dali::Renderer), Create, VisualRendererDefaultProperties);

} // unnamed namespace

VisualRendererPtr VisualRenderer::New()
{
  // create scene object first so it's guaranteed to exist for the event side
  auto sceneObject = SceneGraph::Renderer::New();

  sceneObject->SetVisualProperties(new AnimatableVisualProperties());

  OwnerPointer<SceneGraph::Renderer> transferOwnership(sceneObject);
  // pass the pointer to base for message passing
  VisualRendererPtr rendererPtr(new VisualRenderer(sceneObject));

  rendererPtr->AddUniformMappings(); // Ensure properties are mapped to uniforms

  // transfer scene object ownership to update manager
  EventThreadServices&       eventThreadServices = rendererPtr->GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager       = eventThreadServices.GetUpdateManager();
  AddRendererMessage(updateManager, transferOwnership);

  eventThreadServices.RegisterObject(rendererPtr.Get());
  return rendererPtr;
}

VisualRenderer::VisualRenderer(const SceneGraph::Renderer* sceneObject)
: Renderer(sceneObject)
{
}

VisualRenderer::~VisualRenderer()
{
  // The scene object will be deleted by ~Renderer
}

const SceneGraph::Renderer& VisualRenderer::GetVisualRendererSceneObject() const
{
  return static_cast<const SceneGraph::Renderer&>(GetSceneObject());
}

void VisualRenderer::SetDefaultProperty(Property::Index        index,
                                        const Property::Value& propertyValue)
{
  if(index < Dali::VisualRenderer::Property::DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    Renderer::SetDefaultProperty(index, propertyValue);
  }
  else
  {
    switch(index)
    {
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
      {
        if(propertyValue.Get(mPropertyCache.mTransformOffset))
        {
          const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
          auto                        visualProperties = sceneObject.GetVisualProperties();

          if(visualProperties)
          {
            BakeMessage<Vector2>(GetEventThreadServices(), visualProperties->mTransformOffset, mPropertyCache.mTransformOffset);
          }
        }
        break;
      }

      case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
      {
        if(propertyValue.Get(mPropertyCache.mTransformSize))
        {
          const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
          auto                        visualProperties = sceneObject.GetVisualProperties();
          if(visualProperties)
          {
            BakeMessage<Vector2>(GetEventThreadServices(), visualProperties->mTransformSize, mPropertyCache.mTransformSize);
          }
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
      {
        if(propertyValue.Get(mPropertyCache.mTransformOrigin))
        {
          const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
          auto                        visualProperties = sceneObject.GetVisualProperties();
          if(visualProperties)
          {
            BakeMessage<Vector2>(GetEventThreadServices(), visualProperties->mTransformOrigin, mPropertyCache.mTransformOrigin);
          }
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
      {
        if(propertyValue.Get(mPropertyCache.mTransformAnchorPoint))
        {
          const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
          auto                        visualProperties = sceneObject.GetVisualProperties();
          if(visualProperties)
          {
            BakeMessage<Vector2>(GetEventThreadServices(), visualProperties->mTransformAnchorPoint, mPropertyCache.mTransformAnchorPoint);
          }
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
      {
        if(propertyValue.Get(mPropertyCache.mTransformOffsetSizeMode))
        {
          const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
          auto                        visualProperties = sceneObject.GetVisualProperties();
          if(visualProperties)
          {
            BakeMessage<Vector4>(GetEventThreadServices(), visualProperties->mTransformOffsetSizeMode, mPropertyCache.mTransformOffsetSizeMode);
          }
        }
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE:
      {
        if(propertyValue.Get(mPropertyCache.mExtraSize))
        {
          const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
          auto                        visualProperties = sceneObject.GetVisualProperties();
          if(visualProperties)
          {
            BakeMessage<Vector2>(GetEventThreadServices(), visualProperties->mExtraSize, mPropertyCache.mExtraSize);
          }
        }
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
      {
        if(propertyValue.Get(mPropertyCache.mMixColor))
        {
          const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
          auto                        visualProperties = sceneObject.GetVisualProperties();
          if(visualProperties)
          {
            BakeMessage<Vector3>(GetEventThreadServices(), visualProperties->mMixColor, mPropertyCache.mMixColor);
          }
        }
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA:
      {
        float preMultipliedAlpha;
        if(propertyValue.Get(preMultipliedAlpha))
        {
          if(!Equals(mPropertyCache.mPreMultipliedAlpha, preMultipliedAlpha))
          {
            const SceneGraph::Renderer& sceneObject      = GetVisualRendererSceneObject();
            auto                        visualProperties = sceneObject.GetVisualProperties();
            if(visualProperties)
            {
              mPropertyCache.mPreMultipliedAlpha = preMultipliedAlpha;
              BakeMessage<float>(GetEventThreadServices(), visualProperties->mPreMultipliedAlpha, preMultipliedAlpha);
            }
          }
        }
        break;
      }
    }
  }
}

Property::Value VisualRenderer::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  if(index < Dali::VisualRenderer::Property::DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    value = Renderer::GetDefaultProperty(index);
  }
  else
  {
    switch(index)
    {
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
      {
        value = mPropertyCache.mTransformOffset;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
      {
        value = mPropertyCache.mTransformSize;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
      {
        value = mPropertyCache.mTransformOrigin;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
      {
        value = mPropertyCache.mTransformAnchorPoint;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
      {
        value = mPropertyCache.mTransformOffsetSizeMode;
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE:
      {
        value = mPropertyCache.mExtraSize;
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
      {
        value = mPropertyCache.mMixColor;
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA:
      {
        value = mPropertyCache.mPreMultipliedAlpha;
        break;
      }
      default:
      {
        break;
      }
    }
  }

  return value;
}

Property::Value VisualRenderer::GetDefaultPropertyCurrentValue(Property::Index index) const
{
  Property::Value value;

  if(index < Dali::VisualRenderer::Property::DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    value = Renderer::GetDefaultPropertyCurrentValue(index);
  }
  else
  {
    const SceneGraph::Renderer& sceneObject = GetVisualRendererSceneObject();

    switch(index)
    {
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mTransformOffset[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mTransformSize[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mTransformOrigin[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mTransformAnchorPoint[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mTransformOffsetSizeMode[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mExtraSize[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mMixColor[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA:
      {
        auto visualProperties = sceneObject.GetVisualProperties();
        if(visualProperties)
        {
          value = visualProperties->mPreMultipliedAlpha[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
    }
  }
  return value;
}

void VisualRenderer::OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType)
{
  if(index < Dali::VisualRenderer::Property::DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    Renderer::OnNotifyDefaultPropertyAnimation(animation, index, value, animationType);
  }
  else
  {
    switch(animationType)
    {
      case Animation::TO:
      case Animation::BETWEEN:
      {
        switch(index)
        {
          case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
          {
            value.Get(mPropertyCache.mTransformOffset);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
          {
            value.Get(mPropertyCache.mTransformSize);
            break;
          }
          case Dali::VisualRenderer::Property::EXTRA_SIZE:
          {
            value.Get(mPropertyCache.mExtraSize);
            break;
          }
          case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
          {
            value.Get(mPropertyCache.mMixColor);
            break;
          }
        }
        break;
      }

      case Animation::BY:
      {
        switch(index)
        {
          case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
          {
            AdjustValue<Vector2>(mPropertyCache.mTransformOffset, value);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
          {
            AdjustValue<Vector2>(mPropertyCache.mTransformSize, value);
            break;
          }
          case Dali::VisualRenderer::Property::EXTRA_SIZE:
          {
            AdjustValue<Vector2>(mPropertyCache.mExtraSize, value);
            break;
          }
          case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
          {
            AdjustValue<Vector3>(mPropertyCache.mMixColor, value);
            break;
          }
        }
        break;
      }
    }
  }
}

const SceneGraph::PropertyBase* VisualRenderer::GetSceneObjectAnimatableProperty(Property::Index index) const
{
  const SceneGraph::PropertyBase* property = nullptr;

  switch(index)
  {
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        property = &visualProperties->mTransformOffset;
      }
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        property = &visualProperties->mTransformSize;
      }
      break;
    }
    case Dali::VisualRenderer::Property::EXTRA_SIZE:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        property = &visualProperties->mExtraSize;
      }
      break;
    }
    case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        property = &visualProperties->mMixColor;
      }
      break;
    }
  }

  if(!property)
  {
    // not our property, ask base
    property = Renderer::GetSceneObjectAnimatableProperty(index);
  }

  return property;
}

const PropertyInputImpl* VisualRenderer::GetSceneObjectInputProperty(Property::Index index) const
{
  switch(index)
  {
    case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        return &visualProperties->mTransformOrigin;
      }
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        return &visualProperties->mTransformAnchorPoint;
      }
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        return &visualProperties->mTransformOffsetSizeMode;
      }
      break;
    }
    case Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA:
    {
      auto visualProperties = GetVisualRendererSceneObject().GetVisualProperties();
      if(visualProperties)
      {
        return &visualProperties->mPreMultipliedAlpha;
      }
      break;
    }
    default:
    {
      return GetSceneObjectAnimatableProperty(index);
    }
  }
  return nullptr;
}

void VisualRenderer::AddUniformMappings()
{
  AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_OFFSET, ConstString("offset"));
  AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_SIZE, ConstString("size"));
  AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, ConstString("offsetSizeMode"));
  AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_ORIGIN, ConstString("origin"));
  AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT, ConstString("anchorPoint"));
  AddUniformMapping(Dali::VisualRenderer::Property::EXTRA_SIZE, ConstString("extraSize"));
  AddUniformMapping(Dali::VisualRenderer::Property::VISUAL_MIX_COLOR, ConstString("mixColor"));
  AddUniformMapping(Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA, ConstString("preMultipliedAlpha"));
}

} // namespace Internal

} // namespace Dali
