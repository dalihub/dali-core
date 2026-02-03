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
#include <dali/internal/event/rendering/decorated-visual-renderer-impl.h>

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/common/owner-key-type.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/animatable-property-messages.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-renderer-messages.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/rendering/scene-graph-visual-renderer.h>
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
DALI_PROPERTY("cornerRadius", VECTOR4, true, true, true, Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS)
DALI_PROPERTY("cornerRadiusPolicy", FLOAT, true, false, true, Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY)
DALI_PROPERTY("borderlineWidth", FLOAT, true, true, true, Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH)
DALI_PROPERTY("borderlineColor", VECTOR4, true, true, true, Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR)
DALI_PROPERTY("borderlineOffset", FLOAT, true, true, true, Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET)
DALI_PROPERTY("blurRadius", FLOAT, true, true, true, Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS)
DALI_PROPERTY("cornerSquareness", VECTOR4, true, true, true, Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS)
DALI_PROPERTY_TABLE_END(Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS, DecoratedVisualRendererDefaultProperties)

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType(typeid(Dali::DecoratedVisualRenderer), typeid(Dali::VisualRenderer), Create, DecoratedVisualRendererDefaultProperties);

/**
 * Sets both the cached value of a property and sends a message to set the animatable property in the Update thread.
 * @tparam T The property type
 * @param eventThreadServices The event thread services
 * @param propertyValue The new property value given
 * @param cachedValue The local cached value of the property
 * @param animatableProperty The animatable property to set on the update-thread
 */
template<typename T>
void SetValue(EventThreadServices& eventThreadServices, const SceneGraph::PropertyOwner& propertyOwner, const Property::Value& propertyValue, T& cachedValue, const SceneGraph::AnimatableProperty<T>& animatableProperty)
{
  if(propertyValue.Get(cachedValue))
  {
    BakeMessage<T>(eventThreadServices, propertyOwner, animatableProperty, cachedValue);
  }
}

/**
 * Sets the cached value of a property only.
 * @tparam T The property type
 * @param[in] propertyValue The new property value given
 * @param[in,out] cachedValue The local cached value of the property
 * @param[in,out] updated True if cache value updated
 */
template<typename T>
void SetCacheValue(const Property::Value& propertyValue, T& cachedValue, bool& updated)
{
  if(propertyValue.Get(cachedValue))
  {
    updated = true;
  }
}

} // unnamed namespace

DecoratedVisualRendererPtr DecoratedVisualRenderer::New()
{
  // create scene object first so it's guaranteed to exist for the event side
  auto                               sceneObjectKey = SceneGraph::Renderer::NewKey();
  OwnerKeyType<SceneGraph::Renderer> transferKeyOwnership(sceneObjectKey);

  sceneObjectKey->SetDummyVisualProperties();
  sceneObjectKey->SetDummyDecoratedVisualProperties();

  // pass the pointer to base for message passing
  DecoratedVisualRendererPtr rendererPtr(new DecoratedVisualRenderer(sceneObjectKey.Get()));

  EventThreadServices&       eventThreadServices = rendererPtr->GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager       = eventThreadServices.GetUpdateManager();
  AddRendererMessage(updateManager, transferKeyOwnership);

  eventThreadServices.RegisterObject(rendererPtr.Get());
  return rendererPtr;
}

DecoratedVisualRenderer::DecoratedVisualRenderer(const SceneGraph::Renderer* sceneObject)
: VisualRenderer(sceneObject),
  mDecoratedPropertyCache(),
  mDecoratedVisualProperties(nullptr),
  mAddUniformFlag(0u),
  mPropertyCacheChanged(false)
{
}

DecoratedVisualRenderer::~DecoratedVisualRenderer() = default; // The scene object will be deleted by ~VisualRenderer

void DecoratedVisualRenderer::SetDefaultProperty(Property::Index        index,
                                                 const Property::Value& propertyValue)
{
  if(index < Dali::DecoratedVisualRenderer::Property::DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    VisualRenderer::SetDefaultProperty(index, propertyValue);
  }
  else
  {
    bool sceneGraphVisualPropertyUpdated = false;
    if(DALI_LIKELY(mDecoratedVisualProperties))
    {
      EventThreadServices& eventThreadServices = GetEventThreadServices();
      switch(index)
      {
        case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mDecoratedPropertyCache.mCornerRadius, mDecoratedVisualProperties->mCornerRadius);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mDecoratedPropertyCache.mCornerSquareness, mDecoratedVisualProperties->mCornerSquareness);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mDecoratedPropertyCache.mCornerRadiusPolicy, mDecoratedVisualProperties->mCornerRadiusPolicy);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mDecoratedPropertyCache.mBorderlineWidth, mDecoratedVisualProperties->mBorderlineWidth);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mDecoratedPropertyCache.mBorderlineColor, mDecoratedVisualProperties->mBorderlineColor);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mDecoratedPropertyCache.mBorderlineOffset, mDecoratedVisualProperties->mBorderlineOffset);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mDecoratedPropertyCache.mBlurRadius, mDecoratedVisualProperties->mBlurRadius);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }
      }
    }
    if(!sceneGraphVisualPropertyUpdated)
    {
      bool propertyCacheChanged = false;
      switch(index)
      {
        case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
        {
          SetCacheValue(propertyValue, mDecoratedPropertyCache.mCornerRadius, propertyCacheChanged);
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
        {
          SetCacheValue(propertyValue, mDecoratedPropertyCache.mCornerSquareness, propertyCacheChanged);
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
        {
          SetCacheValue(propertyValue, mDecoratedPropertyCache.mCornerRadiusPolicy, propertyCacheChanged);
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
        {
          SetCacheValue(propertyValue, mDecoratedPropertyCache.mBorderlineWidth, propertyCacheChanged);
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
        {
          SetCacheValue(propertyValue, mDecoratedPropertyCache.mBorderlineColor, propertyCacheChanged);
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
        {
          SetCacheValue(propertyValue, mDecoratedPropertyCache.mBorderlineOffset, propertyCacheChanged);
          break;
        }

        case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
        {
          SetCacheValue(propertyValue, mDecoratedPropertyCache.mBlurRadius, propertyCacheChanged);
          break;
        }
      }
      mPropertyCacheChanged |= propertyCacheChanged;
    }
  }
}

Property::Value DecoratedVisualRenderer::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  if(index < Dali::DecoratedVisualRenderer::Property::DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    value = VisualRenderer::GetDefaultProperty(index);
  }
  else
  {
    switch(index)
    {
      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
      {
        value = mDecoratedPropertyCache.mCornerRadius;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
      {
        value = mDecoratedPropertyCache.mCornerSquareness;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
      {
        value = mDecoratedPropertyCache.mCornerRadiusPolicy;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
      {
        value = mDecoratedPropertyCache.mBorderlineWidth;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
      {
        value = mDecoratedPropertyCache.mBorderlineColor;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
      {
        value = mDecoratedPropertyCache.mBorderlineOffset;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
      {
        value = mDecoratedPropertyCache.mBlurRadius;
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

Property::Value DecoratedVisualRenderer::GetDefaultPropertyCurrentValue(Property::Index index) const
{
  Property::Value value;

  if(index < Dali::DecoratedVisualRenderer::Property::DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    value = VisualRenderer::GetDefaultPropertyCurrentValue(index);
  }
  else
  {
    switch(index)
    {
      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
      {
        if(DALI_LIKELY(mDecoratedVisualProperties))
        {
          value = mDecoratedVisualProperties->mCornerRadius[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
      {
        if(DALI_LIKELY(mDecoratedVisualProperties))
        {
          value = mDecoratedVisualProperties->mCornerSquareness[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
      {
        if(DALI_LIKELY(mDecoratedVisualProperties))
        {
          value = mDecoratedVisualProperties->mCornerRadiusPolicy[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
      {
        if(DALI_LIKELY(mDecoratedVisualProperties))
        {
          value = mDecoratedVisualProperties->mBorderlineWidth[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
      {
        if(DALI_LIKELY(mDecoratedVisualProperties))
        {
          value = mDecoratedVisualProperties->mBorderlineColor[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
      {
        if(DALI_LIKELY(mDecoratedVisualProperties))
        {
          value = mDecoratedVisualProperties->mBorderlineOffset[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
      {
        if(DALI_LIKELY(mDecoratedVisualProperties))
        {
          value = mDecoratedVisualProperties->mBlurRadius[GetEventThreadServices().GetEventBufferIndex()];
        }
        break;
      }
    }
  }
  return value;
}

void DecoratedVisualRenderer::OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType)
{
  if(index < Dali::DecoratedVisualRenderer::Property::DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    VisualRenderer::OnNotifyDefaultPropertyAnimation(animation, index, value, animationType);
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
          case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
          {
            value.Get(mDecoratedPropertyCache.mCornerRadius);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
          {
            value.Get(mDecoratedPropertyCache.mCornerSquareness);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
          {
            value.Get(mDecoratedPropertyCache.mBorderlineWidth);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
          {
            value.Get(mDecoratedPropertyCache.mBorderlineColor);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
          {
            value.Get(mDecoratedPropertyCache.mBorderlineOffset);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
          {
            value.Get(mDecoratedPropertyCache.mBlurRadius);
            break;
          }
        }
        break;
      }

      case Animation::BY:
      {
        switch(index)
        {
          case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
          {
            AdjustValue<Vector4>(mDecoratedPropertyCache.mCornerRadius, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
          {
            AdjustValue<Vector4>(mDecoratedPropertyCache.mCornerSquareness, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
          {
            AdjustValue<float>(mDecoratedPropertyCache.mBorderlineWidth, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
          {
            AdjustValue<Vector4>(mDecoratedPropertyCache.mBorderlineColor, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
          {
            AdjustValue<float>(mDecoratedPropertyCache.mBorderlineOffset, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
          {
            AdjustValue<float>(mDecoratedPropertyCache.mBlurRadius, value);
            break;
          }
        }
        break;
      }
    }
  }
}

const SceneGraph::PropertyBase* DecoratedVisualRenderer::GetSceneObjectAnimatableProperty(Property::Index index) const
{
  const SceneGraph::PropertyBase* property = nullptr;

  switch(index)
  {
    case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
    {
      if(DALI_LIKELY(mDecoratedVisualProperties))
      {
        property = &mDecoratedVisualProperties->mCornerRadius;
      }
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
    {
      if(DALI_LIKELY(mDecoratedVisualProperties))
      {
        property = &mDecoratedVisualProperties->mCornerSquareness;
      }
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
    {
      if(DALI_LIKELY(mDecoratedVisualProperties))
      {
        property = &mDecoratedVisualProperties->mBorderlineWidth;
      }
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
    {
      if(DALI_LIKELY(mDecoratedVisualProperties))
      {
        property = &mDecoratedVisualProperties->mBorderlineColor;
      }
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
    {
      if(DALI_LIKELY(mDecoratedVisualProperties))
      {
        property = &mDecoratedVisualProperties->mBorderlineOffset;
      }
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
    {
      if(DALI_LIKELY(mDecoratedVisualProperties))
      {
        property = &mDecoratedVisualProperties->mBlurRadius;
      }
      break;
    }
  }

  if(!property)
  {
    // not our property, ask base
    property = VisualRenderer::GetSceneObjectAnimatableProperty(index);
  }

  return property;
}

const PropertyInputImpl* DecoratedVisualRenderer::GetSceneObjectInputProperty(Property::Index index) const
{
  if(index < Dali::DecoratedVisualRenderer::Property::DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    return VisualRenderer::GetSceneObjectInputProperty(index);
  }
  switch(index)
  {
    case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
    {
      if(DALI_LIKELY(mDecoratedVisualProperties))
      {
        return &mDecoratedVisualProperties->mCornerRadiusPolicy;
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

void DecoratedVisualRenderer::RegisterCornerRadiusUniform()
{
  AddUniformFlag(DECORATED_VISUAL_RENDERER_USE_CORNER_RADIUS);
}

void DecoratedVisualRenderer::RegisterCornerSquarenessUniform()
{
  AddUniformFlag(DECORATED_VISUAL_RENDERER_USE_CORNER_RADIUS | DECORATED_VISUAL_RENDERER_USE_CORNER_SQUARENESS);
}

void DecoratedVisualRenderer::RegisterBorderlineUniform()
{
  AddUniformFlag(DECORATED_VISUAL_RENDERER_USE_BORDERLINE);
}

void DecoratedVisualRenderer::RegisterBlurRadiusUniform()
{
  AddUniformFlag(DECORATED_VISUAL_RENDERER_USE_BLUR_RADIUS);
}

void DecoratedVisualRenderer::AddUniformFlag(uint8_t newAddFlag)
{
  const uint8_t diffUniformFlag = (~mAddUniformFlag) & newAddFlag;
  if(diffUniformFlag)
  {
    if(mDecoratedVisualProperties == nullptr)
    {
      mDecoratedVisualProperties = new SceneGraph::VisualRenderer::DecoratedVisualProperties(const_cast<SceneGraph::Renderer&>(GetVisualRendererSceneObject()));
      OwnerPointer<SceneGraph::VisualRenderer::DecoratedVisualProperties> transferOwnership(mDecoratedVisualProperties);
      SetDecoratedVisualPropertiesMessage(GetEventThreadServices(), GetRendererSceneObject(), transferOwnership);

      if(DALI_UNLIKELY(mPropertyCacheChanged))
      {
        BakeMessage<Vector4>(GetEventThreadServices(), *mUpdateObject, mDecoratedVisualProperties->mCornerRadius, mDecoratedPropertyCache.mCornerRadius);
        BakeMessage<Vector4>(GetEventThreadServices(), *mUpdateObject, mDecoratedVisualProperties->mCornerSquareness, mDecoratedPropertyCache.mCornerSquareness);
        BakeMessage<float>(GetEventThreadServices(), *mUpdateObject, mDecoratedVisualProperties->mCornerRadiusPolicy, mDecoratedPropertyCache.mCornerRadiusPolicy);
        BakeMessage<float>(GetEventThreadServices(), *mUpdateObject, mDecoratedVisualProperties->mBorderlineWidth, mDecoratedPropertyCache.mBorderlineWidth);
        BakeMessage<Vector4>(GetEventThreadServices(), *mUpdateObject, mDecoratedVisualProperties->mBorderlineColor, mDecoratedPropertyCache.mBorderlineColor);
        BakeMessage<float>(GetEventThreadServices(), *mUpdateObject, mDecoratedVisualProperties->mBorderlineOffset, mDecoratedPropertyCache.mBorderlineOffset);
        BakeMessage<float>(GetEventThreadServices(), *mUpdateObject, mDecoratedVisualProperties->mBlurRadius, mDecoratedPropertyCache.mBlurRadius);
      }
    }

    if(diffUniformFlag & DECORATED_VISUAL_RENDERER_USE_CORNER_RADIUS)
    {
      AddUniformMapping(Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS, ConstString("cornerRadius"));
      AddUniformMapping(Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, ConstString("cornerRadiusPolicy"));
    }
    if(diffUniformFlag & DECORATED_VISUAL_RENDERER_USE_CORNER_SQUARENESS)
    {
      AddUniformMapping(Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS, ConstString("cornerSquareness"));
    }
    if(diffUniformFlag & DECORATED_VISUAL_RENDERER_USE_BORDERLINE)
    {
      AddUniformMapping(Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, ConstString("borderlineWidth"));
      AddUniformMapping(Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR, ConstString("borderlineColor"));
      AddUniformMapping(Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, ConstString("borderlineOffset"));
    }
    if(diffUniformFlag & DECORATED_VISUAL_RENDERER_USE_BLUR_RADIUS)
    {
      AddUniformMapping(Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS, ConstString("blurRadius"));
    }

    // Note. Let we don't remove UniformMapping due to the performane issue.
    mAddUniformFlag |= newAddFlag;
  }
}

} // namespace Internal

} // namespace Dali
