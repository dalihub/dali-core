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
 *
 */

// CLASS HEADER
#include <dali/internal/event/rendering/visual-renderer-impl.h>

// INTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/common/owner-key-type.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/animatable-property-messages.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-renderer-messages.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/rendering/scene-graph-visual-renderer.h>

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
DALI_PROPERTY("transformOffsetX", FLOAT, true, true, true, Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X)
DALI_PROPERTY("transformOffsetY", FLOAT, true, true, true, Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y)
DALI_PROPERTY("transformSize", VECTOR2, true, true, true, Dali::VisualRenderer::Property::TRANSFORM_SIZE)
DALI_PROPERTY("transformSizeWidth", FLOAT, true, true, true, Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH)
DALI_PROPERTY("transformSizeHeight", FLOAT, true, true, true, Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT)
DALI_PROPERTY("extraSize", VECTOR2, true, true, true, Dali::VisualRenderer::Property::EXTRA_SIZE)
DALI_PROPERTY("extraSizeWidth", FLOAT, true, true, true, Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH)
DALI_PROPERTY("extraSizeHeight", FLOAT, true, true, true, Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT)
DALI_PROPERTY("transformOrigin", VECTOR2, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_ORIGIN)
DALI_PROPERTY("transformOriginX", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X)
DALI_PROPERTY("transformOriginY", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_Y)
DALI_PROPERTY("transformPivot", VECTOR2, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_PIVOT)
DALI_PROPERTY("transformPivotX", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X)
DALI_PROPERTY("transformPivotY", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_PIVOT_Y)
DALI_PROPERTY("transformOffsetSizeMode", VECTOR4, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE)
DALI_PROPERTY("transformOffsetXPolicy", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY)
DALI_PROPERTY("transformOffsetYPolicy", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY)
DALI_PROPERTY("transformSizeWidthPolicy", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY)
DALI_PROPERTY("transformSizeHeightPolicy", FLOAT, true, false, false, Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY)
DALI_PROPERTY_TABLE_END(Dali::VisualRenderer::Property::DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX, VisualRendererDefaultProperties)

// Property string to enumeration tables:

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType(typeid(Dali::VisualRenderer), typeid(Dali::Renderer), Create, VisualRendererDefaultProperties);

constexpr uint32_t DEFAULT_VISUAL_UNIFORM_BLOCK_COUNT = 1u;

/**
 * Sets both the cached value of a property and sends a message to set the animatable property in the Update thread.
 * @tparam PropertyT The property type
 * @tparam componentIndex The component index of animatable property
 * @param eventThreadServices The event thread services
 * @param propertyValue The new property value given
 * @param cachedValue The local cached value of the property
 * @param animatableProperty The animatable property to set on the update-thread
 */
template<typename PropertyT, uint32_t componentIndex = static_cast<uint32_t>(Property::INVALID_COMPONENT_INDEX)>
void SetValue(EventThreadServices& eventThreadServices, const SceneGraph::PropertyOwner& propertyOwner, const Property::Value& propertyValue, PropertyT& cachedValue, const SceneGraph::AnimatableProperty<PropertyT>& animatableProperty)
{
  if constexpr(componentIndex == 0 && (std::is_same_v<PropertyT, Dali::Vector2> || std::is_same_v<PropertyT, Dali::Vector3> || std::is_same_v<PropertyT, Dali::Vector4>))
  {
    if(propertyValue.Get(cachedValue.x))
    {
      SetXComponentMessage<PropertyT>(eventThreadServices, propertyOwner, animatableProperty, cachedValue.x);
    }
  }
  else if constexpr(componentIndex == 1 && (std::is_same_v<PropertyT, Dali::Vector2> || std::is_same_v<PropertyT, Dali::Vector3> || std::is_same_v<PropertyT, Dali::Vector4>))
  {
    if(propertyValue.Get(cachedValue.y))
    {
      SetYComponentMessage<PropertyT>(eventThreadServices, propertyOwner, animatableProperty, cachedValue.y);
    }
  }
  else if constexpr(componentIndex == 2 && (std::is_same_v<PropertyT, Dali::Vector3> || std::is_same_v<PropertyT, Dali::Vector4>))
  {
    if(propertyValue.Get(cachedValue.z))
    {
      SetZComponentMessage<PropertyT>(eventThreadServices, propertyOwner, animatableProperty, cachedValue.z);
    }
  }
  else if constexpr(componentIndex == 3 && (std::is_same_v<PropertyT, Dali::Vector4>))
  {
    if(propertyValue.Get(cachedValue.w))
    {
      SetWComponentMessage<PropertyT>(eventThreadServices, propertyOwner, animatableProperty, cachedValue.w);
    }
  }
  else
  {
    static_assert(componentIndex == static_cast<uint32_t>(Property::INVALID_COMPONENT_INDEX));
    if(propertyValue.Get(cachedValue))
    {
      BakeMessage<PropertyT>(eventThreadServices, propertyOwner, animatableProperty, cachedValue);
    }
  }
}

inline constexpr bool IsAnimatableVisualPropertyIndex(Property::Index index)
{
  return (index >= Dali::VisualRenderer::Property::TRANSFORM_OFFSET) &&
         (index <= Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT);
}

inline constexpr bool IsVisualPropertyIndex(Property::Index index)
{
  return IsAnimatableVisualPropertyIndex(index) ||
         ((index >= Dali::VisualRenderer::Property::TRANSFORM_ORIGIN) &&
          (index <= Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY));
}

VisualRenderer::VisualPropertyCache gDummyVisualPropertyCache; // dummy cache for get default variables.

} // unnamed namespace

VisualRendererPtr VisualRenderer::New()
{
  // create scene object first so it's guaranteed to exist for the event side
  auto                               sceneObjectKey = SceneGraph::Renderer::NewKey();
  OwnerKeyType<SceneGraph::Renderer> transferKeyOwnership(sceneObjectKey);

  sceneObjectKey->SetDummyVisualProperties();

  // pass the pointer to base for message passing
  VisualRendererPtr rendererPtr(new VisualRenderer(sceneObjectKey.Get()));

  // transfer scene object ownership to update manager
  EventThreadServices&       eventThreadServices = rendererPtr->GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager       = eventThreadServices.GetUpdateManager();
  AddRendererMessage(updateManager, transferKeyOwnership);

  eventThreadServices.RegisterObject(rendererPtr.Get());
  return rendererPtr;
}

VisualRenderer::VisualRenderer(const SceneGraph::Renderer* sceneObject)
: Renderer(sceneObject),
  mPropertyCache(&gDummyVisualPropertyCache),
  mVisualProperties(nullptr),
  mVisualPropertiesCreated(false)
{
}

VisualRenderer::~VisualRenderer()
{
  // The scene object will be deleted by ~Renderer
  if(mVisualPropertiesCreated)
  {
    delete mPropertyCache;
  }
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
    if(IsVisualPropertyIndex(index))
    {
      EnsureVisualPropertiesAndCache();
    }

    switch(index)
    {
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffset, mVisualProperties->mTransformOffset);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X:
      {
        SetValue<Vector2, 0>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffset, mVisualProperties->mTransformOffset);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y:
      {
        SetValue<Vector2, 1>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffset, mVisualProperties->mTransformOffset);
        break;
      }

      case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformSize, mVisualProperties->mTransformSize);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH:
      {
        SetValue<Vector2, 0>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformSize, mVisualProperties->mTransformSize);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT:
      {
        SetValue<Vector2, 1>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformSize, mVisualProperties->mTransformSize);
        break;
      }

      case Dali::VisualRenderer::Property::EXTRA_SIZE:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mExtraSize, mVisualProperties->mExtraSize);
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH:
      {
        SetValue<Vector2, 0>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mExtraSize, mVisualProperties->mExtraSize);
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT:
      {
        SetValue<Vector2, 1>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mExtraSize, mVisualProperties->mExtraSize);
        break;
      }

      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOrigin, mVisualProperties->mTransformOrigin);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X:
      {
        SetValue<Vector2, 0>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOrigin, mVisualProperties->mTransformOrigin);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_Y:
      {
        SetValue<Vector2, 1>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOrigin, mVisualProperties->mTransformOrigin);
        break;
      }

      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformPivot, mVisualProperties->mTransformPivot);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X:
      {
        SetValue<Vector2, 0>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformPivot, mVisualProperties->mTransformPivot);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_Y:
      {
        SetValue<Vector2, 1>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformPivot, mVisualProperties->mTransformPivot);
        break;
      }

      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffsetSizeMode, mVisualProperties->mTransformOffsetSizeMode);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY:
      {
        SetValue<Vector4, 0>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffsetSizeMode, mVisualProperties->mTransformOffsetSizeMode);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY:
      {
        SetValue<Vector4, 1>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffsetSizeMode, mVisualProperties->mTransformOffsetSizeMode);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY:
      {
        SetValue<Vector4, 2>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffsetSizeMode, mVisualProperties->mTransformOffsetSizeMode);
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY:
      {
        SetValue<Vector4, 3>(GetEventThreadServices(), *mUpdateObject, propertyValue, mPropertyCache->mTransformOffsetSizeMode, mVisualProperties->mTransformOffsetSizeMode);
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
        value = mPropertyCache->mTransformOffset;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X:
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X);
        value                         = mPropertyCache->mTransformOffset[componentIndex];
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
      {
        value = mPropertyCache->mTransformSize;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH:
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH);
        value                         = mPropertyCache->mTransformSize[componentIndex];
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE:
      {
        value = mPropertyCache->mExtraSize;
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH:
      case Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH);
        value                         = mPropertyCache->mExtraSize[componentIndex];
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
      {
        value = mPropertyCache->mTransformOrigin;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X:
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_Y:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X);
        value                         = mPropertyCache->mTransformOrigin[componentIndex];
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT:
      {
        value = mPropertyCache->mTransformPivot;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X:
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_Y:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X);
        value                         = mPropertyCache->mTransformPivot[componentIndex];
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
      {
        value = mPropertyCache->mTransformOffsetSizeMode;
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY:
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY:
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY:
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY);
        value                         = mPropertyCache->mTransformOffsetSizeMode[componentIndex];
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
    switch(index)
    {
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
      {
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformOffset.Get();
        }
        else
        {
          value = mPropertyCache->mTransformOffset;
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X:
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X);
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformOffset.Get()[componentIndex];
        }
        else
        {
          value = mPropertyCache->mTransformOffset[componentIndex];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
      {
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformSize.Get();
        }
        else
        {
          value = mPropertyCache->mTransformSize;
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH:
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH);
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformSize.Get()[componentIndex];
        }
        else
        {
          value = mPropertyCache->mTransformSize[componentIndex];
        }
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE:
      {
        if(mVisualProperties)
        {
          value = mVisualProperties->mExtraSize.Get();
        }
        else
        {
          value = mPropertyCache->mExtraSize;
        }
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH:
      case Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH);
        if(mVisualProperties)
        {
          value = mVisualProperties->mExtraSize.Get()[componentIndex];
        }
        else
        {
          value = mPropertyCache->mExtraSize[componentIndex];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
      {
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformOrigin.Get();
        }
        else
        {
          value = mPropertyCache->mTransformOrigin;
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X:
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_Y:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X);
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformOrigin.Get()[componentIndex];
        }
        else
        {
          value = mPropertyCache->mTransformOrigin[componentIndex];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT:
      {
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformPivot.Get();
        }
        else
        {
          value = mPropertyCache->mTransformPivot;
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X:
      case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_Y:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X);
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformPivot.Get()[componentIndex];
        }
        else
        {
          value = mPropertyCache->mTransformPivot[componentIndex];
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
      {
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformOffsetSizeMode.Get();
        }
        else
        {
          value = mPropertyCache->mTransformOffsetSizeMode;
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY:
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY:
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY:
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY:
      {
        const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY);
        if(mVisualProperties)
        {
          value = mVisualProperties->mTransformOffsetSizeMode.Get()[componentIndex];
        }
        else
        {
          value = mPropertyCache->mTransformOffsetSizeMode[componentIndex];
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
    if(IsAnimatableVisualPropertyIndex(index))
    {
      EnsureVisualPropertiesAndCache();
    }

    switch(animationType)
    {
      case Animation::TO:
      case Animation::BETWEEN:
      {
        switch(index)
        {
          case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
          {
            value.Get(mPropertyCache->mTransformOffset);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X:
          case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y:
          {
            const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X);
            value.Get(mPropertyCache->mTransformOffset[componentIndex]);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
          {
            value.Get(mPropertyCache->mTransformSize);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH:
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT:
          {
            const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH);
            value.Get(mPropertyCache->mTransformSize[componentIndex]);
            break;
          }
          case Dali::VisualRenderer::Property::EXTRA_SIZE:
          {
            value.Get(mPropertyCache->mExtraSize);
            break;
          }
          case Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH:
          case Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT:
          {
            const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH);
            value.Get(mPropertyCache->mExtraSize[componentIndex]);
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
            AdjustValue<Vector2>(mPropertyCache->mTransformOffset, value);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X:
          case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y:
          {
            const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X);
            AdjustValue<float>(mPropertyCache->mTransformOffset[componentIndex], value);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
          {
            AdjustValue<Vector2>(mPropertyCache->mTransformSize, value);
            break;
          }
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH:
          case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT:
          {
            const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH);
            AdjustValue<float>(mPropertyCache->mTransformSize[componentIndex], value);
            break;
          }
          case Dali::VisualRenderer::Property::EXTRA_SIZE:
          {
            AdjustValue<Vector2>(mPropertyCache->mExtraSize, value);
            break;
          }
          case Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH:
          case Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT:
          {
            const uint32_t componentIndex = static_cast<uint32_t>(index) - static_cast<uint32_t>(Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH);
            AdjustValue<float>(mPropertyCache->mExtraSize[componentIndex], value);
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

  if(IsAnimatableVisualPropertyIndex(index))
  {
    EnsureVisualPropertiesAndCache();
  }

  switch(index)
  {
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X:
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y:
    {
      property = &mVisualProperties->mTransformOffset;
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH:
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT:
    {
      property = &mVisualProperties->mTransformSize;
      break;
    }
    case Dali::VisualRenderer::Property::EXTRA_SIZE:
    case Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH:
    case Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT:
    {
      property = &mVisualProperties->mExtraSize;
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
  if(IsAnimatableVisualPropertyIndex(index))
  {
    EnsureVisualPropertiesAndCache();
  }

  switch(index)
  {
    case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
    case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X:
    case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_Y:
    {
      return &mVisualProperties->mTransformOrigin;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_PIVOT:
    case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X:
    case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_Y:
    {
      return &mVisualProperties->mTransformPivot;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY:
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY:
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY:
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY:
    {
      return &mVisualProperties->mTransformOffsetSizeMode;
    }
    default:
    {
      return GetSceneObjectAnimatableProperty(index);
    }
  }
  return nullptr;
}

int32_t VisualRenderer::GetPropertyComponentIndex(Property::Index index) const
{
  int32_t componentIndex = Property::INVALID_COMPONENT_INDEX;

  switch(index)
  {
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X:
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH:
    case Dali::VisualRenderer::Property::EXTRA_SIZE_WIDTH:
    case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_X:
    case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_X:
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY:
    {
      componentIndex = 0;
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y:
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT:
    case Dali::VisualRenderer::Property::EXTRA_SIZE_HEIGHT:
    case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN_Y:
    case Dali::VisualRenderer::Property::TRANSFORM_PIVOT_Y:
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY:
    {
      componentIndex = 1;
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY:
    {
      componentIndex = 2;
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY:
    {
      componentIndex = 2;
      break;
    }

    default:
    {
      // Do nothing
      break;
    }
  }

  if(Property::INVALID_COMPONENT_INDEX == componentIndex)
  {
    // ask base
    componentIndex = Renderer::GetPropertyComponentIndex(index);
  }

  return componentIndex;
}

void VisualRenderer::OnShaderChanged()
{
  if(DALI_UNLIKELY(!mVisualPropertiesCreated && mShader && mShader->GetConnectedUniformBlockCount() != DEFAULT_VISUAL_UNIFORM_BLOCK_COUNT))
  {
    // If we don't use shared uniform block, we should register uniforms.
    EnsureVisualPropertiesAndCache();
  }
}

void VisualRenderer::EnsureVisualPropertiesAndCache() const
{
  if(DALI_UNLIKELY(!mVisualPropertiesCreated))
  {
    EventThreadServices& eventThreadServices = const_cast<EventThreadServices&>(GetEventThreadServices());
    // Mark as we will not use shared uniform block.
    // TODO : Currently, we ignore whole shared uniform blocks. We should specify the name of UBO in future.
    EnableSharedUniformBlockMessage(eventThreadServices, GetRendererSceneObject(), false);

    if(DALI_LIKELY(mVisualProperties == nullptr))
    {
      mVisualProperties = new SceneGraph::VisualRenderer::VisualProperties(const_cast<SceneGraph::Renderer&>(GetVisualRendererSceneObject()));
      OwnerPointer<SceneGraph::VisualRenderer::VisualProperties> transferOwnership(mVisualProperties);
      SetVisualPropertiesMessage(eventThreadServices, GetRendererSceneObject(), transferOwnership);
    }

    if(DALI_LIKELY(mPropertyCache == &gDummyVisualPropertyCache))
    {
      mPropertyCache = new VisualPropertyCache();
    }

    mVisualPropertiesCreated = true;

    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_OFFSET, ConstString("offset"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_SIZE, ConstString("size"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, ConstString("offsetSizeMode"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_ORIGIN, ConstString("origin"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_PIVOT, ConstString("pivot"));
    AddUniformMapping(Dali::VisualRenderer::Property::EXTRA_SIZE, ConstString("extraSize"));
  }
}

} // namespace Internal

} // namespace Dali
