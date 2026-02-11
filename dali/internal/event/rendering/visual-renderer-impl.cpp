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
DALI_PROPERTY("visualMixColor", VECTOR3, true, false, true, Dali::VisualRenderer::Property::VISUAL_MIX_COLOR)
DALI_PROPERTY("visualPreMultipliedAlpha", FLOAT, true, false, false, Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA)
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

/**
 * Sets both the cached value of a property and sends a message to set the animatable property in the Update thread.
 * @tparam T The property type
 * @param[in] eventThreadServices The event thread services
 * @param[in] propertyValue The new property value given
 * @param[in,out] cachedValue The local cached value of the property
 * @param[in] animatableProperty The animatable property to set on the update-thread
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
  mPropertyCache(),
  mVisualProperties(nullptr),
  mUniformMapped(false),
  mPropertyCacheChanged(false)
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
    bool sceneGraphVisualPropertyUpdated = false;
    if(DALI_LIKELY(mVisualProperties))
    {
      EventThreadServices& eventThreadServices = GetEventThreadServices();
      switch(index)
      {
        case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mPropertyCache.mTransformOffset, mVisualProperties->mTransformOffset);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mPropertyCache.mTransformSize, mVisualProperties->mTransformSize);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mPropertyCache.mTransformOrigin, mVisualProperties->mTransformOrigin);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mPropertyCache.mTransformAnchorPoint, mVisualProperties->mTransformAnchorPoint);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mPropertyCache.mTransformOffsetSizeMode, mVisualProperties->mTransformOffsetSizeMode);
          sceneGraphVisualPropertyUpdated = true;
          break;
        }

        case Dali::VisualRenderer::Property::EXTRA_SIZE:
        {
          SetValue(eventThreadServices, *mUpdateObject, propertyValue, mPropertyCache.mExtraSize, mVisualProperties->mExtraSize);
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
        case Dali::VisualRenderer::Property::TRANSFORM_OFFSET:
        {
          SetCacheValue(propertyValue, mPropertyCache.mTransformOffset, propertyCacheChanged);
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
        {
          SetCacheValue(propertyValue, mPropertyCache.mTransformSize, propertyCacheChanged);
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
        {
          SetCacheValue(propertyValue, mPropertyCache.mTransformOrigin, propertyCacheChanged);
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
        {
          SetCacheValue(propertyValue, mPropertyCache.mTransformAnchorPoint, propertyCacheChanged);
          break;
        }

        case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
        {
          SetCacheValue(propertyValue, mPropertyCache.mTransformOffsetSizeMode, propertyCacheChanged);
          break;
        }

        case Dali::VisualRenderer::Property::EXTRA_SIZE:
        {
          SetCacheValue(propertyValue, mPropertyCache.mExtraSize, propertyCacheChanged);
          break;
        }

        case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
        {
          Vector3 mixColorVec3;
          if(propertyValue.Get(mixColorVec3))
          {
            float opacity = Renderer::GetDefaultProperty(Dali::Renderer::Property::OPACITY).Get<float>();
            Renderer::SetDefaultProperty(Dali::Renderer::Property::MIX_COLOR, Vector4(mixColorVec3.r, mixColorVec3.g, mixColorVec3.b, opacity));
          }
          break;
        }

        case Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA:
        {
          float preMultipliedAlpha = 0.0f;
          if(propertyValue.Get(preMultipliedAlpha))
          {
            Renderer::SetDefaultProperty(Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, !Dali::EqualsZero(preMultipliedAlpha));
          }
          break;
        }
      }

      mPropertyCacheChanged |= propertyCacheChanged;
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
        Vector4 mixColor     = Renderer::GetDefaultProperty(Dali::Renderer::Property::MIX_COLOR).Get<Vector4>();
        Vector3 mixColorVec3 = Vector3(mixColor.r, mixColor.g, mixColor.b);
        value                = mixColorVec3;
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA:
      {
        bool blendPreMultipliedAlpha = Renderer::GetDefaultProperty(Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA).Get<bool>();
        value                        = blendPreMultipliedAlpha ? 1.0f : 0.0f;
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
        if(DALI_LIKELY(mVisualProperties))
        {
          value = mVisualProperties->mTransformOffset.Get();
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
      {
        if(DALI_LIKELY(mVisualProperties))
        {
          value = mVisualProperties->mTransformSize.Get();
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ORIGIN:
      {
        if(DALI_LIKELY(mVisualProperties))
        {
          value = mVisualProperties->mTransformOrigin.Get();
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
      {
        if(DALI_LIKELY(mVisualProperties))
        {
          value = mVisualProperties->mTransformAnchorPoint.Get();
        }
        break;
      }
      case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
      {
        if(DALI_LIKELY(mVisualProperties))
        {
          value = mVisualProperties->mTransformOffsetSizeMode.Get();
        }
        break;
      }
      case Dali::VisualRenderer::Property::EXTRA_SIZE:
      {
        if(DALI_LIKELY(mVisualProperties))
        {
          value = mVisualProperties->mExtraSize.Get();
        }
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
      {
        Vector4 mixColor     = Renderer::GetDefaultPropertyCurrentValue(Dali::Renderer::Property::MIX_COLOR).Get<Vector4>();
        Vector3 mixColorVec3 = Vector3(mixColor.r, mixColor.g, mixColor.b);
        value                = mixColorVec3;
        break;
      }
      case Dali::VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA:
      {
        bool blendPreMultipliedAlpha = Renderer::GetDefaultPropertyCurrentValue(Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA).Get<bool>();
        value                        = blendPreMultipliedAlpha ? 1.0f : 0.0f;
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
      if(DALI_LIKELY(mVisualProperties))
      {
        property = &mVisualProperties->mTransformOffset;
      }
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_SIZE:
    {
      if(DALI_LIKELY(mVisualProperties))
      {
        property = &mVisualProperties->mTransformSize;
      }
      break;
    }
    case Dali::VisualRenderer::Property::EXTRA_SIZE:
    {
      if(DALI_LIKELY(mVisualProperties))
      {
        property = &mVisualProperties->mExtraSize;
      }
      break;
    }
    case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
    {
      // We should use Dali::Renderer::Property::MIX_COLOR, instead of it.
      property = Renderer::GetSceneObjectAnimatableProperty(Dali::Renderer::Property::MIX_COLOR);
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
      if(DALI_LIKELY(mVisualProperties))
      {
        return &mVisualProperties->mTransformOrigin;
      }
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT:
    {
      if(DALI_LIKELY(mVisualProperties))
      {
        return &mVisualProperties->mTransformAnchorPoint;
      }
      break;
    }
    case Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE:
    {
      if(DALI_LIKELY(mVisualProperties))
      {
        return &mVisualProperties->mTransformOffsetSizeMode;
      }
      break;
    }
    case Dali::VisualRenderer::Property::VISUAL_MIX_COLOR:
    {
      // We should use Dali::Renderer::Property::MIX_COLOR, instead of it.
      return Renderer::GetSceneObjectInputProperty(Dali::Renderer::Property::MIX_COLOR);
    }
    default:
    {
      return GetSceneObjectAnimatableProperty(index);
    }
  }
  return nullptr;
}

void VisualRenderer::RegisterVisualTransformUniform()
{
  if(!mUniformMapped)
  {
    // Mark as we will not use shared uniform block.
    // TODO : Currently, we ignore whole shared uniform blocks. We should specify the name of UBO in future.
    EnableSharedUniformBlockMessage(GetEventThreadServices(), GetRendererSceneObject(), false);

    if(mVisualProperties == nullptr)
    {
      mVisualProperties = new SceneGraph::VisualRenderer::VisualProperties(const_cast<SceneGraph::Renderer&>(GetVisualRendererSceneObject()));
      OwnerPointer<SceneGraph::VisualRenderer::VisualProperties> transferOwnership(mVisualProperties);
      SetVisualPropertiesMessage(GetEventThreadServices(), GetRendererSceneObject(), transferOwnership);

      if(DALI_UNLIKELY(mPropertyCacheChanged))
      {
        BakeMessage<Vector2>(GetEventThreadServices(), *mUpdateObject, mVisualProperties->mTransformOffset, mPropertyCache.mTransformOffset);
        BakeMessage<Vector2>(GetEventThreadServices(), *mUpdateObject, mVisualProperties->mTransformSize, mPropertyCache.mTransformSize);
        BakeMessage<Vector2>(GetEventThreadServices(), *mUpdateObject, mVisualProperties->mTransformOrigin, mPropertyCache.mTransformOrigin);
        BakeMessage<Vector2>(GetEventThreadServices(), *mUpdateObject, mVisualProperties->mTransformAnchorPoint, mPropertyCache.mTransformAnchorPoint);
        BakeMessage<Vector4>(GetEventThreadServices(), *mUpdateObject, mVisualProperties->mTransformOffsetSizeMode, mPropertyCache.mTransformOffsetSizeMode);
        BakeMessage<Vector2>(GetEventThreadServices(), *mUpdateObject, mVisualProperties->mExtraSize, mPropertyCache.mExtraSize);
      }
    }

    mUniformMapped = true;
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_OFFSET, ConstString("offset"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_SIZE, ConstString("size"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, ConstString("offsetSizeMode"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_ORIGIN, ConstString("origin"));
    AddUniformMapping(Dali::VisualRenderer::Property::TRANSFORM_ANCHOR_POINT, ConstString("anchorPoint"));
    AddUniformMapping(Dali::VisualRenderer::Property::EXTRA_SIZE, ConstString("extraSize"));
  }
}

} // namespace Internal

} // namespace Dali
