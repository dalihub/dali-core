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

inline constexpr bool IsCornerRadiusPropertyIndex(Property::Index index)
{
  return (index == Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS) ||
         (index == Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY);
}

inline constexpr bool IsCornerSquarenessPropertyIndex(Property::Index index)
{
  return IsCornerRadiusPropertyIndex(index) ||
         (index == Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS);
}

inline constexpr bool IsBorderlinePropertyIndex(Property::Index index)
{
  return (index == Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH) ||
         (index == Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR) ||
         (index == Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET);
}

inline constexpr bool IsBlurPropertyIndex(Property::Index index)
{
  return (index == Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS);
}

inline constexpr bool IsDecoratedVisualPropertyIndex(Property::Index index)
{
  return IsCornerSquarenessPropertyIndex(index) ||
         IsBorderlinePropertyIndex(index) ||
         IsBlurPropertyIndex(index);
}

inline constexpr uint8_t GetDecoratedVisualRendererUseType(Property::Index index)
{
  if(IsCornerRadiusPropertyIndex(index))
  {
    return DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_CORNER_RADIUS;
  }
  if(IsCornerSquarenessPropertyIndex(index))
  {
    return DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_CORNER_RADIUS | DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_CORNER_SQUARENESS;
  }
  if(IsBorderlinePropertyIndex(index))
  {
    return DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_BORDERLINE;
  }
  if(IsBlurPropertyIndex(index))
  {
    return DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_BLUR_RADIUS;
  }
  return 0u;
}

inline constexpr bool IsDecoratedVisualCornerRadiusPropertyRequred(uint8_t useFlag)
{
  constexpr uint8_t cornerRadiusFlag = DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_CORNER_RADIUS |
                                       DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_CORNER_SQUARENESS;
  return useFlag & cornerRadiusFlag;
}

inline constexpr bool IsDecoratedVisualBorderlinePropertyRequred(uint8_t useFlag)
{
  constexpr uint8_t borderlineFlag = DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_BORDERLINE |
                                     DecoratedVisualRenderer::DECORATED_VISUAL_RENDERER_USE_BLUR_RADIUS;
  return useFlag & borderlineFlag;
}

DecoratedVisualRenderer::DecoratedVisualCornerRadiusPropertyCache gDummyDecoratedVisualCornerRadiusPropertyCache; // dummy cache for get default variables.
DecoratedVisualRenderer::DecoratedVisualBorderlinePropertyCache   gDummyDecoratedVisualBorderlinePropertyCache;   // dummy cache for get default variables.

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
  mDecoratedCornerRadiusPropertyCache(&gDummyDecoratedVisualCornerRadiusPropertyCache),
  mDecoratedBorderlinePropertyCache(&gDummyDecoratedVisualBorderlinePropertyCache),
  mDecoratedVisualCornerRadiusProperties(nullptr),
  mDecoratedVisualBorderlineProperties(nullptr),
  mAddUniformFlag(0u),
  mDecoratedVisualCornerRadiusPropertiesCreated(false),
  mDecoratedVisualBorderlinePropertiesCreated(false)
{
}

DecoratedVisualRenderer::~DecoratedVisualRenderer()
{
  if(mDecoratedVisualCornerRadiusPropertiesCreated)
  {
    delete mDecoratedCornerRadiusPropertyCache;
  }
  if(mDecoratedVisualBorderlinePropertiesCreated)
  {
    delete mDecoratedBorderlinePropertyCache;
  }
  // The scene object will be deleted by ~VisualRenderer
}

void DecoratedVisualRenderer::SetDefaultProperty(Property::Index        index,
                                                 const Property::Value& propertyValue)
{
  if(index < Dali::DecoratedVisualRenderer::Property::DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX)
  {
    VisualRenderer::SetDefaultProperty(index, propertyValue);
  }
  else
  {
    const uint8_t addFlags = GetDecoratedVisualRendererUseType(index);
    if(addFlags != 0u)
    {
      EnsureDecoratedVisualPropertiesAndCache(addFlags);
    }

    switch(index)
    {
      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mDecoratedCornerRadiusPropertyCache->mCornerRadius, mDecoratedVisualCornerRadiusProperties->mCornerRadius);
        break;
      }

      case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mDecoratedCornerRadiusPropertyCache->mCornerSquareness, mDecoratedVisualCornerRadiusProperties->mCornerSquareness);
        break;
      }

      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mDecoratedCornerRadiusPropertyCache->mCornerRadiusPolicy, mDecoratedVisualCornerRadiusProperties->mCornerRadiusPolicy);
        break;
      }

      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mDecoratedBorderlinePropertyCache->mBorderlineWidth, mDecoratedVisualBorderlineProperties->mBorderlineWidth);
        break;
      }

      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mDecoratedBorderlinePropertyCache->mBorderlineColor, mDecoratedVisualBorderlineProperties->mBorderlineColor);
        break;
      }

      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mDecoratedBorderlinePropertyCache->mBorderlineOffset, mDecoratedVisualBorderlineProperties->mBorderlineOffset);
        break;
      }

      case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
      {
        SetValue(GetEventThreadServices(), *mUpdateObject, propertyValue, mDecoratedBorderlinePropertyCache->mBlurRadius, mDecoratedVisualBorderlineProperties->mBlurRadius);
        break;
      }
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
        value = mDecoratedCornerRadiusPropertyCache->mCornerRadius;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
      {
        value = mDecoratedCornerRadiusPropertyCache->mCornerSquareness;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
      {
        value = mDecoratedCornerRadiusPropertyCache->mCornerRadiusPolicy;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
      {
        value = mDecoratedBorderlinePropertyCache->mBorderlineWidth;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
      {
        value = mDecoratedBorderlinePropertyCache->mBorderlineColor;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
      {
        value = mDecoratedBorderlinePropertyCache->mBorderlineOffset;
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
      {
        value = mDecoratedBorderlinePropertyCache->mBlurRadius;
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
        if(mDecoratedVisualCornerRadiusProperties)
        {
          value = mDecoratedVisualCornerRadiusProperties->mCornerRadius.Get(GetEventThreadServices().GetEventBufferIndex());
        }
        else
        {
          value = mDecoratedCornerRadiusPropertyCache->mCornerRadius;
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
      {
        if(mDecoratedVisualCornerRadiusProperties)
        {
          value = mDecoratedVisualCornerRadiusProperties->mCornerSquareness.Get(GetEventThreadServices().GetEventBufferIndex());
        }
        else
        {
          value = mDecoratedCornerRadiusPropertyCache->mCornerSquareness;
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
      {
        if(mDecoratedVisualCornerRadiusProperties)
        {
          value = mDecoratedVisualCornerRadiusProperties->mCornerRadiusPolicy.Get(GetEventThreadServices().GetEventBufferIndex());
        }
        else
        {
          value = mDecoratedCornerRadiusPropertyCache->mCornerRadiusPolicy;
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
      {
        if(mDecoratedVisualBorderlineProperties)
        {
          value = mDecoratedVisualBorderlineProperties->mBorderlineWidth.Get(GetEventThreadServices().GetEventBufferIndex());
        }
        else
        {
          value = mDecoratedBorderlinePropertyCache->mBorderlineWidth;
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
      {
        if(mDecoratedVisualBorderlineProperties)
        {
          value = mDecoratedVisualBorderlineProperties->mBorderlineColor.Get(GetEventThreadServices().GetEventBufferIndex());
        }
        else
        {
          value = mDecoratedBorderlinePropertyCache->mBorderlineColor;
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
      {
        if(mDecoratedVisualBorderlineProperties)
        {
          value = mDecoratedVisualBorderlineProperties->mBorderlineOffset.Get(GetEventThreadServices().GetEventBufferIndex());
        }
        else
        {
          value = mDecoratedBorderlinePropertyCache->mBorderlineOffset;
        }
        break;
      }
      case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
      {
        if(mDecoratedVisualBorderlineProperties)
        {
          value = mDecoratedVisualBorderlineProperties->mBlurRadius.Get(GetEventThreadServices().GetEventBufferIndex());
        }
        else
        {
          value = mDecoratedBorderlinePropertyCache->mBlurRadius;
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
    const uint8_t addFlags = GetDecoratedVisualRendererUseType(index);
    if(addFlags != 0u)
    {
      EnsureDecoratedVisualPropertiesAndCache(addFlags);
    }

    switch(animationType)
    {
      case Animation::TO:
      case Animation::BETWEEN:
      {
        switch(index)
        {
          case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
          {
            value.Get(mDecoratedCornerRadiusPropertyCache->mCornerRadius);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
          {
            value.Get(mDecoratedCornerRadiusPropertyCache->mCornerSquareness);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
          {
            value.Get(mDecoratedBorderlinePropertyCache->mBorderlineWidth);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
          {
            value.Get(mDecoratedBorderlinePropertyCache->mBorderlineColor);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
          {
            value.Get(mDecoratedBorderlinePropertyCache->mBorderlineOffset);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
          {
            value.Get(mDecoratedBorderlinePropertyCache->mBlurRadius);
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
            AdjustValue<Vector4>(mDecoratedCornerRadiusPropertyCache->mCornerRadius, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
          {
            AdjustValue<Vector4>(mDecoratedCornerRadiusPropertyCache->mCornerSquareness, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
          {
            AdjustValue<float>(mDecoratedBorderlinePropertyCache->mBorderlineWidth, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
          {
            AdjustValue<Vector4>(mDecoratedBorderlinePropertyCache->mBorderlineColor, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
          {
            AdjustValue<float>(mDecoratedBorderlinePropertyCache->mBorderlineOffset, value);
            break;
          }
          case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
          {
            AdjustValue<float>(mDecoratedBorderlinePropertyCache->mBlurRadius, value);
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

  const uint8_t addFlags = GetDecoratedVisualRendererUseType(index);
  if(addFlags != 0u)
  {
    EnsureDecoratedVisualPropertiesAndCache(addFlags);
  }

  switch(index)
  {
    case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS:
    {
      property = &mDecoratedVisualCornerRadiusProperties->mCornerRadius;
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::CORNER_SQUARENESS:
    {
      property = &mDecoratedVisualCornerRadiusProperties->mCornerSquareness;
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH:
    {
      property = &mDecoratedVisualBorderlineProperties->mBorderlineWidth;
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BORDERLINE_COLOR:
    {
      property = &mDecoratedVisualBorderlineProperties->mBorderlineColor;
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BORDERLINE_OFFSET:
    {
      property = &mDecoratedVisualBorderlineProperties->mBorderlineOffset;
      break;
    }
    case Dali::DecoratedVisualRenderer::Property::BLUR_RADIUS:
    {
      property = &mDecoratedVisualBorderlineProperties->mBlurRadius;
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

  const uint8_t addFlags = GetDecoratedVisualRendererUseType(index);
  if(addFlags != 0u)
  {
    EnsureDecoratedVisualPropertiesAndCache(addFlags);
  }

  switch(index)
  {
    case Dali::DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY:
    {
      return &mDecoratedVisualCornerRadiusProperties->mCornerRadiusPolicy;
    }
    default:
    {
      return GetSceneObjectAnimatableProperty(index);
    }
  }
  return nullptr;
}

void DecoratedVisualRenderer::EnsureDecoratedVisualPropertiesAndCache(uint8_t newAddFlag) const
{
  const uint8_t diffUniformFlag = (~mAddUniformFlag) & newAddFlag;
  if(diffUniformFlag)
  {
    if(DALI_UNLIKELY(IsDecoratedVisualCornerRadiusPropertyRequred(diffUniformFlag) && !mDecoratedVisualCornerRadiusPropertiesCreated))
    {
      if(DALI_LIKELY(mDecoratedVisualCornerRadiusProperties == nullptr))
      {
        EventThreadServices& eventThreadServices = const_cast<EventThreadServices&>(GetEventThreadServices());
        mDecoratedVisualCornerRadiusProperties   = new SceneGraph::VisualRenderer::DecoratedVisualCornerRadiusProperties();
        OwnerPointer<SceneGraph::VisualRenderer::DecoratedVisualCornerRadiusProperties> transferOwnership(mDecoratedVisualCornerRadiusProperties);
        SetDecoratedVisualCornerRadiusPropertiesMessage(eventThreadServices, GetRendererSceneObject(), transferOwnership);
      }

      if(DALI_LIKELY(mDecoratedCornerRadiusPropertyCache == &gDummyDecoratedVisualCornerRadiusPropertyCache))
      {
        mDecoratedCornerRadiusPropertyCache = new DecoratedVisualCornerRadiusPropertyCache();
      }

      mDecoratedVisualCornerRadiusPropertiesCreated = true;
    }

    if(DALI_UNLIKELY(IsDecoratedVisualBorderlinePropertyRequred(diffUniformFlag) && !mDecoratedVisualBorderlinePropertiesCreated))
    {
      if(DALI_LIKELY(mDecoratedVisualBorderlineProperties == nullptr))
      {
        EventThreadServices& eventThreadServices = const_cast<EventThreadServices&>(GetEventThreadServices());
        mDecoratedVisualBorderlineProperties     = new SceneGraph::VisualRenderer::DecoratedVisualBorderlineProperties(const_cast<SceneGraph::Renderer&>(GetVisualRendererSceneObject()));
        OwnerPointer<SceneGraph::VisualRenderer::DecoratedVisualBorderlineProperties> transferOwnership(mDecoratedVisualBorderlineProperties);
        SetDecoratedVisualBorderlinePropertiesMessage(eventThreadServices, GetRendererSceneObject(), transferOwnership);
      }

      if(DALI_LIKELY(mDecoratedBorderlinePropertyCache == &gDummyDecoratedVisualBorderlinePropertyCache))
      {
        mDecoratedBorderlinePropertyCache = new DecoratedVisualBorderlinePropertyCache();
      }

      mDecoratedVisualBorderlinePropertiesCreated = true;
    }

    // Note. Let we don't remove UniformMapping due to the performane issue.
    mAddUniformFlag |= newAddFlag;

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
  }
}

} // namespace Internal

} // namespace Dali
