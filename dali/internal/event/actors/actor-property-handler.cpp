/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/actor-property-handler.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>

#include <dali/devel-api/actors/actor-devel.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/actor-relayouter.h>
#include <dali/internal/event/actors/actor-sizer.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/nodes/node-messages.h>

using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::Node;
using Dali::Internal::SceneGraph::PropertyBase;

namespace Dali
{
namespace Internal
{
namespace // unnamed namespace
{
struct AnchorValue
{
  const char*    name;
  const Vector3& value;
};

DALI_ENUM_TO_STRING_TABLE_BEGIN_WITH_TYPE(AnchorValue, ANCHOR_CONSTANT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, TOP_LEFT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, TOP_CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, TOP_RIGHT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, CENTER_LEFT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, CENTER_RIGHT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, BOTTOM_LEFT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, BOTTOM_CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(AnchorPoint, BOTTOM_RIGHT)
DALI_ENUM_TO_STRING_TABLE_END(ANCHOR_CONSTANT)

DALI_ENUM_TO_STRING_TABLE_BEGIN(COLOR_MODE)
  DALI_ENUM_TO_STRING(USE_OWN_COLOR)
  DALI_ENUM_TO_STRING(USE_PARENT_COLOR)
  DALI_ENUM_TO_STRING(USE_OWN_MULTIPLY_PARENT_COLOR)
  DALI_ENUM_TO_STRING(USE_OWN_MULTIPLY_PARENT_ALPHA)
DALI_ENUM_TO_STRING_TABLE_END(COLOR_MODE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(DRAW_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DrawMode, NORMAL)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DrawMode, OVERLAY_2D)
DALI_ENUM_TO_STRING_TABLE_END(DRAW_MODE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(RESIZE_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, FIXED)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, USE_NATURAL_SIZE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, FILL_TO_PARENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, SIZE_RELATIVE_TO_PARENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, SIZE_FIXED_OFFSET_FROM_PARENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, FIT_TO_CHILDREN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, DIMENSION_DEPENDENCY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ResizePolicy, USE_ASSIGNED_SIZE)
DALI_ENUM_TO_STRING_TABLE_END(RESIZE_POLICY)

DALI_ENUM_TO_STRING_TABLE_BEGIN(SIZE_SCALE_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(SizeScalePolicy, USE_SIZE_SET)
  DALI_ENUM_TO_STRING_WITH_SCOPE(SizeScalePolicy, FIT_WITH_ASPECT_RATIO)
  DALI_ENUM_TO_STRING_WITH_SCOPE(SizeScalePolicy, FILL_WITH_ASPECT_RATIO)
DALI_ENUM_TO_STRING_TABLE_END(SIZE_SCALE_POLICY)

DALI_ENUM_TO_STRING_TABLE_BEGIN(CLIPPING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ClippingMode, DISABLED)
  DALI_ENUM_TO_STRING_WITH_SCOPE(ClippingMode, CLIP_CHILDREN)
DALI_ENUM_TO_STRING_TABLE_END(CLIPPING_MODE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(LAYOUT_DIRECTION)
  DALI_ENUM_TO_STRING_WITH_SCOPE(LayoutDirection, LEFT_TO_RIGHT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(LayoutDirection, RIGHT_TO_LEFT)
DALI_ENUM_TO_STRING_TABLE_END(LAYOUT_DIRECTION)

DALI_ENUM_TO_STRING_TABLE_BEGIN(CHILDREN_DEPTH_INDEX_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelActor::ChildrenDepthIndexPolicy, INCREASE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelActor::ChildrenDepthIndexPolicy, EQUAL)
DALI_ENUM_TO_STRING_TABLE_END(CHILDREN_DEPTH_INDEX_POLICY)

bool GetAnchorPointParentOriginConstant(const std::string& value, Vector3& anchor)
{
  for(uint32_t i = 0; i < ANCHOR_CONSTANT_TABLE_COUNT; ++i)
  {
    uint32_t sizeIgnored = 0;
    if(CompareTokens(value.c_str(), ANCHOR_CONSTANT_TABLE[i].name, sizeIgnored))
    {
      anchor = ANCHOR_CONSTANT_TABLE[i].value;
      return true;
    }
  }
  return false;
}

bool GetVector3Value(const Property::Value& property, Vector3& vector3)
{
  if(property.Get(vector3))
  {
    return true;
  }
  else
  {
    std::string stringConstant;
    if(property.Get(stringConstant))
    {
      return GetAnchorPointParentOriginConstant(stringConstant, vector3);
    }
    else
    {
      float value = 0.0f;
      if(property.Get(value))
      {
        vector3.x = vector3.y = vector3.z = value;
        return true;
      }
    }
  }
  return false;
}

void DetermineVector3ValueAndSet(const Property::Value& property, Actor& actor, void (Actor::*member)(const Vector3&))
{
  Vector3 value;
  if(GetVector3Value(property, value))
  {
    (actor.*member)(value);
  }
}

template<typename ParameterType>
void CheckValidAndSet(const Property::Value& property, Actor& actor, void (Actor::*member)(ParameterType))
{
  ParameterType value;
  if(property.Get(value))
  {
    (actor.*member)(value);
  }
}

template<typename ParameterType>
void CheckValidAndSet(const Property::Value& property, Actor& actor, void (Actor::*member)(const ParameterType&))
{
  ParameterType value;
  if(property.Get(value))
  {
    (actor.*member)(value);
  }
}

template<typename ParameterType, typename Function>
void CheckValidAndSet(const Property::Value& property, Function function)
{
  ParameterType value;
  if(property.Get(value))
  {
    function(value);
  }
}
} // unnamed namespace

void Actor::PropertyHandler::SetDefaultProperty(Internal::Actor& actor, Property::Index index, const Property::Value& property)
{
  switch(index)
  {
    case Dali::Actor::Property::PARENT_ORIGIN:
    {
      DetermineVector3ValueAndSet(property, actor, &Actor::SetParentOrigin);
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_X:
    {
      const Vector3& current = actor.GetCurrentParentOrigin();
      actor.SetParentOrigin(Vector3(property.Get<float>(), current.y, current.z));
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Y:
    {
      const Vector3& current = actor.GetCurrentParentOrigin();
      actor.SetParentOrigin(Vector3(current.x, property.Get<float>(), current.z));
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Z:
    {
      const Vector3& current = actor.GetCurrentParentOrigin();
      actor.SetParentOrigin(Vector3(current.x, current.y, property.Get<float>()));
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT:
    {
      DetermineVector3ValueAndSet(property, actor, &Actor::SetAnchorPoint);
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_X:
    {
      const Vector3& current = actor.GetCurrentAnchorPoint();
      actor.SetAnchorPoint(Vector3(property.Get<float>(), current.y, current.z));
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Y:
    {
      const Vector3& current = actor.GetCurrentAnchorPoint();
      actor.SetAnchorPoint(Vector3(current.x, property.Get<float>(), current.z));
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Z:
    {
      const Vector3& current = actor.GetCurrentAnchorPoint();
      actor.SetAnchorPoint(Vector3(current.x, current.y, property.Get<float>()));
      break;
    }

    case Dali::Actor::Property::SIZE:
    {
      DetermineVector3ValueAndSet(property, actor, &Actor::SetSize);
      break;
    }

    case Dali::Actor::Property::SIZE_WIDTH:
    {
      actor.SetWidth(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      actor.SetHeight(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::SIZE_DEPTH:
    {
      actor.SetDepth(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::POSITION:
    {
      DetermineVector3ValueAndSet(property, actor, &Actor::SetPosition);
      break;
    }

    case Dali::Actor::Property::POSITION_X:
    {
      actor.SetX(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::POSITION_Y:
    {
      actor.SetY(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::POSITION_Z:
    {
      actor.SetZ(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::ORIENTATION:
    {
      actor.SetOrientation(property.Get<Quaternion>());
      break;
    }

    case Dali::Actor::Property::SCALE:
    {
      DetermineVector3ValueAndSet(property, actor, &Actor::SetScale);
      break;
    }

    case Dali::Actor::Property::SCALE_X:
    {
      actor.SetScaleX(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::SCALE_Y:
    {
      actor.SetScaleY(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::SCALE_Z:
    {
      actor.SetScaleZ(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::VISIBLE:
    {
      actor.SetVisible(property.Get<bool>());
      break;
    }

    case Dali::Actor::Property::COLOR:
    {
      CheckValidAndSet<Vector4>(property,
                                [&property, &actor](Vector4& color)
                                {
                                  color.a = (property.GetType() == Property::VECTOR4) ? color.a : 1.0f;
                                  actor.SetColor(color);
                                });
      break;
    }

    case Dali::Actor::Property::COLOR_RED:
    {
      actor.SetColorRed(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::COLOR_GREEN:
    {
      actor.SetColorGreen(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::COLOR_BLUE:
    {
      actor.SetColorBlue(property.Get<float>());
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
    case Dali::Actor::Property::OPACITY:
    {
      CheckValidAndSet(property, actor, &Actor::SetOpacity);
      break;
    }

    case Dali::Actor::Property::NAME:
    {
      actor.SetName(property.Get<std::string>());
      break;
    }

    case Dali::Actor::Property::SENSITIVE:
    {
      actor.SetSensitive(property.Get<bool>());
      break;
    }

    case Dali::Actor::Property::LEAVE_REQUIRED:
    {
      actor.SetLeaveRequired(property.Get<bool>());
      break;
    }

    case Dali::Actor::Property::INHERIT_POSITION:
    {
      actor.SetInheritPosition(property.Get<bool>());
      break;
    }

    case Dali::Actor::Property::INHERIT_ORIENTATION:
    {
      actor.SetInheritOrientation(property.Get<bool>());
      break;
    }

    case Dali::Actor::Property::INHERIT_SCALE:
    {
      actor.SetInheritScale(property.Get<bool>());
      break;
    }

    case Dali::Actor::Property::COLOR_MODE:
    {
      ColorMode mode = actor.mColorMode;
      if(Scripting::GetEnumerationProperty<ColorMode>(property, COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT, mode))
      {
        actor.SetColorMode(mode);
      }
      break;
    }

    case Dali::Actor::Property::DRAW_MODE:
    {
      DrawMode::Type mode = actor.mDrawMode;
      if(Scripting::GetEnumerationProperty<DrawMode::Type>(property, DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT, mode))
      {
        actor.SetDrawMode(mode);
      }
      break;
    }

    case Dali::Actor::Property::SIZE_MODE_FACTOR:
    {
      actor.SetSizeModeFactor(property.Get<Vector3>());
      break;
    }

    case Dali::Actor::Property::WIDTH_RESIZE_POLICY:
    {
      ResizePolicy::Type type = actor.mSizer.GetResizePolicy(Dimension::WIDTH);
      if(Scripting::GetEnumerationProperty<ResizePolicy::Type>(property, RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT, type))
      {
        actor.mSizer.SetResizePolicy(type, Dimension::WIDTH);
      }
      break;
    }

    case Dali::Actor::Property::HEIGHT_RESIZE_POLICY:
    {
      ResizePolicy::Type type = actor.mSizer.GetResizePolicy(Dimension::HEIGHT);
      if(Scripting::GetEnumerationProperty<ResizePolicy::Type>(property, RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT, type))
      {
        actor.mSizer.SetResizePolicy(type, Dimension::HEIGHT);
      }
      break;
    }

    case Dali::Actor::Property::SIZE_SCALE_POLICY:
    {
      SizeScalePolicy::Type type = actor.mSizer.GetSizeScalePolicy();
      if(Scripting::GetEnumerationProperty<SizeScalePolicy::Type>(property, SIZE_SCALE_POLICY_TABLE, SIZE_SCALE_POLICY_TABLE_COUNT, type))
      {
        actor.mSizer.SetSizeScalePolicy(type);
      }
      break;
    }

    case Dali::Actor::Property::WIDTH_FOR_HEIGHT:
    {
      if(property.Get<bool>())
      {
        actor.mSizer.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::WIDTH);
      }
      break;
    }

    case Dali::Actor::Property::HEIGHT_FOR_WIDTH:
    {
      if(property.Get<bool>())
      {
        actor.mSizer.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
      }
      break;
    }

    case Dali::Actor::Property::PADDING:
    {
      Vector4 padding = property.Get<Vector4>();
      actor.SetPadding(Vector2(padding.x, padding.y), Dimension::WIDTH);
      actor.SetPadding(Vector2(padding.z, padding.w), Dimension::HEIGHT);
      break;
    }

    case Dali::Actor::Property::MINIMUM_SIZE:
    {
      Vector2 size = property.Get<Vector2>();
      actor.SetMinimumSize(size.x, Dimension::WIDTH);
      actor.SetMinimumSize(size.y, Dimension::HEIGHT);
      break;
    }

    case Dali::Actor::Property::MAXIMUM_SIZE:
    {
      Vector2 size = property.Get<Vector2>();
      actor.SetMaximumSize(size.x, Dimension::WIDTH);
      actor.SetMaximumSize(size.y, Dimension::HEIGHT);
      break;
    }

    case Dali::DevelActor::Property::SIBLING_ORDER:
    {
      int value;

      if(property.Get(value))
      {
        Actor* parent = actor.GetParent();
        if(parent)
        {
          parent->SetSiblingOrderOfChild(actor, value);
        }
      }
      break;
    }

    case Dali::Actor::Property::CLIPPING_MODE:
    {
      ClippingMode::Type convertedValue = actor.mClippingMode;
      if(Scripting::GetEnumerationProperty<ClippingMode::Type>(property, CLIPPING_MODE_TABLE, CLIPPING_MODE_TABLE_COUNT, convertedValue))
      {
        actor.mClippingMode = convertedValue;
        SetClippingModeMessage(actor.GetEventThreadServices(), actor.GetNode(), actor.mClippingMode);
      }
      break;
    }

    case Dali::Actor::Property::POSITION_USES_ANCHOR_POINT:
    {
      bool value = false;
      if(property.Get(value) && value != actor.mPositionUsesAnchorPoint)
      {
        actor.mPositionUsesAnchorPoint = value;
        SetPositionUsesAnchorPointMessage(actor.GetEventThreadServices(), actor.GetNode(), actor.mPositionUsesAnchorPoint);
      }
      break;
    }

    case Dali::Actor::Property::LAYOUT_DIRECTION:
    {
      Dali::LayoutDirection::Type direction = actor.mLayoutDirection;
      actor.mInheritLayoutDirection         = false;

      if(Scripting::GetEnumerationProperty<LayoutDirection::Type>(property, LAYOUT_DIRECTION_TABLE, LAYOUT_DIRECTION_TABLE_COUNT, direction))
      {
        actor.mParentImpl.InheritLayoutDirectionRecursively(direction, true);
      }
      break;
    }

    case Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION:
    {
      CheckValidAndSet(property, actor, &Actor::SetInheritLayoutDirection);
      break;
    }

    case Dali::Actor::Property::KEYBOARD_FOCUSABLE:
    {
      CheckValidAndSet(property, actor, &Actor::SetKeyboardFocusable);
      break;
    }

    case Dali::Actor::Property::UPDATE_AREA_HINT:
    {
      CheckValidAndSet(property, actor, &Actor::SetUpdateAreaHint);
      break;
    }

    case Dali::DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START:
    {
      CheckValidAndSet<bool>(property, [&actor](bool value)
                             { actor.mCaptureAllTouchAfterStart = value; });
      break;
    }

    case Dali::DevelActor::Property::TOUCH_AREA_OFFSET:
    {
      CheckValidAndSet(property, actor, &Actor::SetTouchAreaOffset);
      break;
    }

    case Dali::DevelActor::Property::BLEND_EQUATION:
    {
      CheckValidAndSet(property, actor, &Actor::SetBlendEquation);
      break;
    }

    case Dali::DevelActor::Property::TOUCH_FOCUSABLE:
    {
      CheckValidAndSet(property, actor, &Actor::SetTouchFocusable);
      break;
    }

    case Dali::DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN:
    {
      CheckValidAndSet(property, actor, &Actor::SetKeyboardFocusableChildren);
      break;
    }

    case Dali::DevelActor::Property::USER_INTERACTION_ENABLED:
    {
      CheckValidAndSet(property, actor, &Actor::SetUserInteractionEnabled);
      break;
    }

    case Dali::DevelActor::Property::ALLOW_ONLY_OWN_TOUCH:
    {
      CheckValidAndSet<bool>(property, [&actor](bool value)
                             { actor.mAllowOnlyOwnTouch = value; });
      break;
    }

    case Dali::DevelActor::Property::USE_TEXTURE_UPDATE_AREA:
    {
      CheckValidAndSet<bool>(property,
                             [&actor](bool value)
                             {
                               actor.mUseTextureUpdateArea = value;
                               UseTextureUpdateAreaMessage(actor.GetEventThreadServices(), actor.GetNode(), value);
                             });
      break;
    }

    case Dali::DevelActor::Property::DISPATCH_TOUCH_MOTION:
    {
      CheckValidAndSet<bool>(property, [&actor](bool value)
                             { actor.mDispatchTouchMotion = value; });
      break;
    }

    case Dali::DevelActor::Property::DISPATCH_HOVER_MOTION:
    {
      CheckValidAndSet<bool>(property, [&actor](bool value)
                             { actor.mDispatchHoverMotion = value; });
      break;
    }

    case Dali::DevelActor::Property::CHILDREN_DEPTH_INDEX_POLICY:
    {
      Dali::DevelActor::ChildrenDepthIndexPolicy::Type depthIndexPolicy = actor.mChildrenDepthIndexPolicy;
      if(Scripting::GetEnumerationProperty<DevelActor::ChildrenDepthIndexPolicy::Type>(property, CHILDREN_DEPTH_INDEX_POLICY_TABLE, CHILDREN_DEPTH_INDEX_POLICY_TABLE_COUNT, depthIndexPolicy))
      {
        actor.SetChildrenDepthIndexPolicy(depthIndexPolicy);
      }
      break;
    }

    default:
    {
      // this can happen in the case of a non-animatable default property so just do nothing
      break;
    }
  }
}

void Actor::PropertyHandler::SetSceneGraphProperty(
  Property::Index         index,
  const PropertyMetadata& entry,
  const Property::Value&  value,
  EventThreadServices&    eventThreadServices,
  const SceneGraph::Node& node)
{
  switch(entry.GetType())
  {
    case Property::BOOLEAN:
    {
      const AnimatableProperty<bool>* property = dynamic_cast<const AnimatableProperty<bool>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<bool>::Send(eventThreadServices, &node, property, &AnimatableProperty<bool>::Bake, value.Get<bool>());

      break;
    }

    case Property::INTEGER:
    {
      const AnimatableProperty<int>* property = dynamic_cast<const AnimatableProperty<int>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<int>::Send(eventThreadServices, &node, property, &AnimatableProperty<int>::Bake, value.Get<int>());

      break;
    }

    case Property::FLOAT:
    {
      const AnimatableProperty<float>* property = dynamic_cast<const AnimatableProperty<float>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<float>::Send(eventThreadServices, &node, property, &AnimatableProperty<float>::Bake, value.Get<float>());

      break;
    }

    case Property::VECTOR2:
    {
      const AnimatableProperty<Vector2>* property = dynamic_cast<const AnimatableProperty<Vector2>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SceneGraph::NodePropertyComponentMessage<Vector2>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector2>::BakeX, value.Get<float>());
      }
      else if(entry.componentIndex == 1)
      {
        SceneGraph::NodePropertyComponentMessage<Vector2>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector2>::BakeY, value.Get<float>());
      }
      else
      {
        SceneGraph::NodePropertyMessage<Vector2>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector2>::Bake, value.Get<Vector2>());
      }

      break;
    }

    case Property::VECTOR3:
    {
      const AnimatableProperty<Vector3>* property = dynamic_cast<const AnimatableProperty<Vector3>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SceneGraph::NodePropertyComponentMessage<Vector3>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector3>::BakeX, value.Get<float>());
      }
      else if(entry.componentIndex == 1)
      {
        SceneGraph::NodePropertyComponentMessage<Vector3>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector3>::BakeY, value.Get<float>());
      }
      else if(entry.componentIndex == 2)
      {
        SceneGraph::NodePropertyComponentMessage<Vector3>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector3>::BakeZ, value.Get<float>());
      }
      else
      {
        SceneGraph::NodePropertyMessage<Vector3>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector3>::Bake, value.Get<Vector3>());
      }

      break;
    }

    case Property::VECTOR4:
    {
      const AnimatableProperty<Vector4>* property = dynamic_cast<const AnimatableProperty<Vector4>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector4>::BakeX, value.Get<float>());
      }
      else if(entry.componentIndex == 1)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector4>::BakeY, value.Get<float>());
      }
      else if(entry.componentIndex == 2)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector4>::BakeZ, value.Get<float>());
      }
      else if(entry.componentIndex == 3)
      {
        SceneGraph::NodePropertyComponentMessage<Vector4>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector4>::BakeW, value.Get<float>());
      }
      else
      {
        SceneGraph::NodePropertyMessage<Vector4>::Send(eventThreadServices, &node, property, &AnimatableProperty<Vector4>::Bake, value.Get<Vector4>());
      }

      break;
    }

    case Property::ROTATION:
    {
      const AnimatableProperty<Quaternion>* property = dynamic_cast<const AnimatableProperty<Quaternion>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Quaternion>::Send(eventThreadServices, &node, property, &AnimatableProperty<Quaternion>::Bake, value.Get<Quaternion>());

      break;
    }

    case Property::MATRIX:
    {
      const AnimatableProperty<Matrix>* property = dynamic_cast<const AnimatableProperty<Matrix>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Matrix>::Send(eventThreadServices, &node, property, &AnimatableProperty<Matrix>::Bake, value.Get<Matrix>());

      break;
    }

    case Property::MATRIX3:
    {
      const AnimatableProperty<Matrix3>* property = dynamic_cast<const AnimatableProperty<Matrix3>*>(entry.GetSceneGraphProperty());
      DALI_ASSERT_DEBUG(NULL != property);

      // property is being used in a separate thread; queue a message to set the property
      SceneGraph::NodePropertyMessage<Matrix3>::Send(eventThreadServices, &node, property, &AnimatableProperty<Matrix3>::Bake, value.Get<Matrix3>());

      break;
    }

    default:
    {
      // nothing to do for other types
    }
  } // entry.GetType
}

void Actor::PropertyHandler::OnNotifyDefaultPropertyAnimation(Internal::Actor& actor, Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType)
{
  switch(animationType)
  {
    case Animation::TO:
    case Animation::BETWEEN:
    {
      switch(index)
      {
        case Dali::Actor::Property::SIZE:
        {
          Vector3 targetSize;
          if(value.Get(targetSize))
          {
            actor.mSizer.OnAnimateSize(animation, targetSize, false);
          }
          break;
        }

        case Dali::Actor::Property::SIZE_WIDTH:
        {
          float width;
          if(value.Get(width))
          {
            actor.mSizer.OnAnimateWidth(animation, width, false);
          }
          break;
        }

        case Dali::Actor::Property::SIZE_HEIGHT:
        {
          float height;
          if(value.Get(height))
          {
            actor.mSizer.OnAnimateHeight(animation, height, false);
          }
          break;
        }

        case Dali::Actor::Property::SIZE_DEPTH:
        {
          float depth;
          if(value.Get(depth))
          {
            actor.mSizer.OnAnimateDepth(animation, depth, false);
          }
          break;
        }

        case Dali::Actor::Property::POSITION:
        {
          value.Get(actor.mTargetPosition);
          break;
        }

        case Dali::Actor::Property::POSITION_X:
        {
          value.Get(actor.mTargetPosition.x);
          break;
        }

        case Dali::Actor::Property::POSITION_Y:
        {
          value.Get(actor.mTargetPosition.y);
          break;
        }

        case Dali::Actor::Property::POSITION_Z:
        {
          value.Get(actor.mTargetPosition.z);
          break;
        }

        case Dali::Actor::Property::ORIENTATION:
        {
          value.Get(actor.mTargetOrientation);
          break;
        }

        case Dali::Actor::Property::SCALE:
        {
          value.Get(actor.mTargetScale);
          break;
        }

        case Dali::Actor::Property::SCALE_X:
        {
          value.Get(actor.mTargetScale.x);
          break;
        }

        case Dali::Actor::Property::SCALE_Y:
        {
          value.Get(actor.mTargetScale.y);
          break;
        }

        case Dali::Actor::Property::SCALE_Z:
        {
          value.Get(actor.mTargetScale.z);
          break;
        }

        case Dali::Actor::Property::VISIBLE:
        {
          actor.SetVisibleInternal(value.Get<bool>(), SendMessage::FALSE);
          break;
        }

        case Dali::Actor::Property::COLOR:
        {
          value.Get(actor.mTargetColor);
          break;
        }

        case Dali::Actor::Property::COLOR_RED:
        {
          value.Get(actor.mTargetColor.r);
          break;
        }

        case Dali::Actor::Property::COLOR_GREEN:
        {
          value.Get(actor.mTargetColor.g);
          break;
        }

        case Dali::Actor::Property::COLOR_BLUE:
        {
          value.Get(actor.mTargetColor.b);
          break;
        }

        case Dali::Actor::Property::COLOR_ALPHA:
        case Dali::Actor::Property::OPACITY:
        {
          value.Get(actor.mTargetColor.a);
          break;
        }

        default:
        {
          // Not an animatable property. Do nothing.
          break;
        }
      }
      break;
    }

    case Animation::BY:
    {
      switch(index)
      {
        case Dali::Actor::Property::SIZE:
        {
          Vector3 targetSize;
          if(value.Get(targetSize))
          {
            actor.mSizer.OnAnimateSize(animation, targetSize, true);
          }
          break;
        }

        case Dali::Actor::Property::SIZE_WIDTH:
        {
          float width;
          if(value.Get(width))
          {
            actor.mSizer.OnAnimateWidth(animation, width, true);
          }
          break;
        }

        case Dali::Actor::Property::SIZE_HEIGHT:
        {
          float height;
          if(value.Get(height))
          {
            actor.mSizer.OnAnimateHeight(animation, height, true);
          }
          break;
        }

        case Dali::Actor::Property::SIZE_DEPTH:
        {
          float depth;
          if(value.Get(depth))
          {
            actor.mSizer.OnAnimateDepth(animation, depth, true);
          }
          break;
        }

        case Dali::Actor::Property::POSITION:
        {
          AdjustValue<Vector3>(actor.mTargetPosition, value);
          break;
        }

        case Dali::Actor::Property::POSITION_X:
        {
          AdjustValue<float>(actor.mTargetPosition.x, value);
          break;
        }

        case Dali::Actor::Property::POSITION_Y:
        {
          AdjustValue<float>(actor.mTargetPosition.y, value);
          break;
        }

        case Dali::Actor::Property::POSITION_Z:
        {
          AdjustValue<float>(actor.mTargetPosition.z, value);
          break;
        }

        case Dali::Actor::Property::ORIENTATION:
        {
          Quaternion relativeValue;
          if(value.Get(relativeValue))
          {
            actor.mTargetOrientation *= relativeValue;
          }
          break;
        }

        case Dali::Actor::Property::SCALE:
        {
          AdjustValue<Vector3>(actor.mTargetScale, value);
          break;
        }

        case Dali::Actor::Property::SCALE_X:
        {
          AdjustValue<float>(actor.mTargetScale.x, value);
          break;
        }

        case Dali::Actor::Property::SCALE_Y:
        {
          AdjustValue<float>(actor.mTargetScale.y, value);
          break;
        }

        case Dali::Actor::Property::SCALE_Z:
        {
          AdjustValue<float>(actor.mTargetScale.z, value);
          break;
        }

        case Dali::Actor::Property::VISIBLE:
        {
          bool relativeValue = false;
          if(value.Get(relativeValue))
          {
            bool visible = actor.mVisible || relativeValue;
            actor.SetVisibleInternal(visible, SendMessage::FALSE);
          }
          break;
        }

        case Dali::Actor::Property::COLOR:
        {
          AdjustValue<Vector4>(actor.mTargetColor, value);
          break;
        }

        case Dali::Actor::Property::COLOR_RED:
        {
          AdjustValue<float>(actor.mTargetColor.r, value);
          break;
        }

        case Dali::Actor::Property::COLOR_GREEN:
        {
          AdjustValue<float>(actor.mTargetColor.g, value);
          break;
        }

        case Dali::Actor::Property::COLOR_BLUE:
        {
          AdjustValue<float>(actor.mTargetColor.b, value);
          break;
        }

        case Dali::Actor::Property::COLOR_ALPHA:
        case Dali::Actor::Property::OPACITY:
        {
          AdjustValue<float>(actor.mTargetColor.a, value);
          break;
        }

        default:
        {
          // Not an animatable property. Do nothing.
          break;
        }
      }
      break;
    }
  }
}

const PropertyBase* Actor::PropertyHandler::GetSceneObjectAnimatableProperty(Property::Index index, const SceneGraph::Node& node)
{
  const PropertyBase* property(nullptr);

  switch(index)
  {
    case Dali::Actor::Property::SIZE:        // FALLTHROUGH
    case Dali::Actor::Property::SIZE_WIDTH:  // FALLTHROUGH
    case Dali::Actor::Property::SIZE_HEIGHT: // FALLTHROUGH
    case Dali::Actor::Property::SIZE_DEPTH:
    {
      property = &node.mSize;
      break;
    }
    case Dali::Actor::Property::POSITION:   // FALLTHROUGH
    case Dali::Actor::Property::POSITION_X: // FALLTHROUGH
    case Dali::Actor::Property::POSITION_Y: // FALLTHROUGH
    case Dali::Actor::Property::POSITION_Z:
    {
      property = &node.mPosition;
      break;
    }
    case Dali::Actor::Property::ORIENTATION:
    {
      property = &node.mOrientation;
      break;
    }
    case Dali::Actor::Property::SCALE:   // FALLTHROUGH
    case Dali::Actor::Property::SCALE_X: // FALLTHROUGH
    case Dali::Actor::Property::SCALE_Y: // FALLTHROUGH
    case Dali::Actor::Property::SCALE_Z:
    {
      property = &node.mScale;
      break;
    }
    case Dali::Actor::Property::VISIBLE:
    {
      property = &node.mVisible;
      break;
    }
    case Dali::Actor::Property::COLOR:       // FALLTHROUGH
    case Dali::Actor::Property::COLOR_RED:   // FALLTHROUGH
    case Dali::Actor::Property::COLOR_GREEN: // FALLTHROUGH
    case Dali::Actor::Property::COLOR_BLUE:  // FALLTHROUGH
    case Dali::Actor::Property::COLOR_ALPHA: // FALLTHROUGH
    case Dali::Actor::Property::OPACITY:
    {
      property = &node.mColor;
      break;
    }
    default:
    {
      break;
    }
  }

  return property;
}

const PropertyInputImpl* Actor::PropertyHandler::GetSceneObjectInputProperty(Property::Index index, const SceneGraph::Node& node)
{
  const PropertyInputImpl* property(nullptr);

  switch(index)
  {
    case Dali::Actor::Property::PARENT_ORIGIN:   // FALLTHROUGH
    case Dali::Actor::Property::PARENT_ORIGIN_X: // FALLTHROUGH
    case Dali::Actor::Property::PARENT_ORIGIN_Y: // FALLTHROUGH
    case Dali::Actor::Property::PARENT_ORIGIN_Z:
    {
      property = &node.mParentOrigin;
      break;
    }
    case Dali::Actor::Property::ANCHOR_POINT:   // FALLTHROUGH
    case Dali::Actor::Property::ANCHOR_POINT_X: // FALLTHROUGH
    case Dali::Actor::Property::ANCHOR_POINT_Y: // FALLTHROUGH
    case Dali::Actor::Property::ANCHOR_POINT_Z:
    {
      property = &node.mAnchorPoint;
      break;
    }
    case Dali::Actor::Property::WORLD_POSITION:   // FALLTHROUGH
    case Dali::Actor::Property::WORLD_POSITION_X: // FALLTHROUGH
    case Dali::Actor::Property::WORLD_POSITION_Y: // FALLTHROUGH
    case Dali::Actor::Property::WORLD_POSITION_Z:
    {
      property = &node.mWorldPosition;
      break;
    }
    case Dali::Actor::Property::WORLD_ORIENTATION:
    {
      property = &node.mWorldOrientation;
      break;
    }
    case Dali::Actor::Property::WORLD_SCALE:
    {
      property = &node.mWorldScale;
      break;
    }
    case Dali::Actor::Property::WORLD_COLOR:
    {
      property = &node.mWorldColor;
      break;
    }
    case Dali::Actor::Property::WORLD_MATRIX:
    {
      property = &node.mWorldMatrix;
      break;
    }
    case Dali::Actor::Property::CULLED:
    {
      property = &node.mCulled;
      break;
    }
    default:
    {
      break;
    }
  }

  return property;
}

int32_t Actor::PropertyHandler::GetPropertyComponentIndex(Property::Index index)
{
  int32_t componentIndex = Property::INVALID_COMPONENT_INDEX;

  switch(index)
  {
    case Dali::Actor::Property::PARENT_ORIGIN_X:
    case Dali::Actor::Property::ANCHOR_POINT_X:
    case Dali::Actor::Property::SIZE_WIDTH:
    case Dali::Actor::Property::POSITION_X:
    case Dali::Actor::Property::WORLD_POSITION_X:
    case Dali::Actor::Property::SCALE_X:
    case Dali::Actor::Property::COLOR_RED:
    {
      componentIndex = 0;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Y:
    case Dali::Actor::Property::ANCHOR_POINT_Y:
    case Dali::Actor::Property::SIZE_HEIGHT:
    case Dali::Actor::Property::POSITION_Y:
    case Dali::Actor::Property::WORLD_POSITION_Y:
    case Dali::Actor::Property::SCALE_Y:
    case Dali::Actor::Property::COLOR_GREEN:
    {
      componentIndex = 1;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Z:
    case Dali::Actor::Property::ANCHOR_POINT_Z:
    case Dali::Actor::Property::SIZE_DEPTH:
    case Dali::Actor::Property::POSITION_Z:
    case Dali::Actor::Property::WORLD_POSITION_Z:
    case Dali::Actor::Property::SCALE_Z:
    case Dali::Actor::Property::COLOR_BLUE:
    {
      componentIndex = 2;
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
    case Dali::Actor::Property::OPACITY:
    {
      componentIndex = 3;
      break;
    }

    default:
    {
      // Do nothing
      break;
    }
  }

  return componentIndex;
}

bool Actor::PropertyHandler::GetCachedPropertyValue(const Internal::Actor& actor, Property::Index index, Property::Value& value)
{
  bool valueSet = true;

  switch(index)
  {
    case Dali::Actor::Property::PARENT_ORIGIN:
    {
      value = actor.GetCurrentParentOrigin();
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_X:
    {
      value = actor.GetCurrentParentOrigin().x;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Y:
    {
      value = actor.GetCurrentParentOrigin().y;
      break;
    }

    case Dali::Actor::Property::PARENT_ORIGIN_Z:
    {
      value = actor.GetCurrentParentOrigin().z;
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT:
    {
      value = actor.GetCurrentAnchorPoint();
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_X:
    {
      value = actor.GetCurrentAnchorPoint().x;
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Y:
    {
      value = actor.GetCurrentAnchorPoint().y;
      break;
    }

    case Dali::Actor::Property::ANCHOR_POINT_Z:
    {
      value = actor.GetCurrentAnchorPoint().z;
      break;
    }

    case Dali::Actor::Property::SIZE:
    {
      value = actor.GetTargetSize();
      break;
    }

    case Dali::Actor::Property::SIZE_WIDTH:
    {
      value = actor.GetTargetSize().width;
      break;
    }

    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      value = actor.GetTargetSize().height;
      break;
    }

    case Dali::Actor::Property::SIZE_DEPTH:
    {
      value = actor.GetTargetSize().depth;
      break;
    }

    case Dali::Actor::Property::POSITION:
    {
      value = actor.GetTargetPosition();
      break;
    }

    case Dali::Actor::Property::POSITION_X:
    {
      value = actor.GetTargetPosition().x;
      break;
    }

    case Dali::Actor::Property::POSITION_Y:
    {
      value = actor.GetTargetPosition().y;
      break;
    }

    case Dali::Actor::Property::POSITION_Z:
    {
      value = actor.GetTargetPosition().z;
      break;
    }

    case Dali::Actor::Property::ORIENTATION:
    {
      value = actor.mTargetOrientation;
      break;
    }

    case Dali::Actor::Property::SCALE:
    {
      value = actor.mTargetScale;
      break;
    }

    case Dali::Actor::Property::SCALE_X:
    {
      value = actor.mTargetScale.x;
      break;
    }

    case Dali::Actor::Property::SCALE_Y:
    {
      value = actor.mTargetScale.y;
      break;
    }

    case Dali::Actor::Property::SCALE_Z:
    {
      value = actor.mTargetScale.z;
      break;
    }

    case Dali::Actor::Property::VISIBLE:
    {
      value = actor.mVisible;
      break;
    }

    case Dali::Actor::Property::COLOR:
    {
      value = actor.mTargetColor;
      break;
    }

    case Dali::Actor::Property::COLOR_RED:
    {
      value = actor.mTargetColor.r;
      break;
    }

    case Dali::Actor::Property::COLOR_GREEN:
    {
      value = actor.mTargetColor.g;
      break;
    }

    case Dali::Actor::Property::COLOR_BLUE:
    {
      value = actor.mTargetColor.b;
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
    case Dali::Actor::Property::OPACITY:
    {
      value = actor.mTargetColor.a;
      break;
    }

    case Dali::Actor::Property::NAME:
    {
      value = std::string(actor.GetName());
      break;
    }

    case Dali::Actor::Property::SENSITIVE:
    {
      value = actor.IsSensitive();
      break;
    }

    case Dali::Actor::Property::LEAVE_REQUIRED:
    {
      value = actor.GetLeaveRequired();
      break;
    }

    case Dali::Actor::Property::INHERIT_POSITION:
    {
      value = actor.IsPositionInherited();
      break;
    }

    case Dali::Actor::Property::INHERIT_ORIENTATION:
    {
      value = actor.IsOrientationInherited();
      break;
    }

    case Dali::Actor::Property::INHERIT_SCALE:
    {
      value = actor.IsScaleInherited();
      break;
    }

    case Dali::Actor::Property::COLOR_MODE:
    {
      value = actor.GetColorMode();
      break;
    }

    case Dali::Actor::Property::DRAW_MODE:
    {
      value = actor.GetDrawMode();
      break;
    }

    case Dali::Actor::Property::SIZE_MODE_FACTOR:
    {
      value = actor.GetSizeModeFactor();
      break;
    }

    case Dali::Actor::Property::WIDTH_RESIZE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName<ResizePolicy::Type>(actor.mSizer.GetResizePolicy(Dimension::WIDTH), RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT);
      break;
    }

    case Dali::Actor::Property::HEIGHT_RESIZE_POLICY:
    {
      value = Scripting::GetLinearEnumerationName<ResizePolicy::Type>(actor.mSizer.GetResizePolicy(Dimension::HEIGHT), RESIZE_POLICY_TABLE, RESIZE_POLICY_TABLE_COUNT);
      break;
    }

    case Dali::Actor::Property::SIZE_SCALE_POLICY:
    {
      value = actor.mSizer.GetSizeScalePolicy();
      break;
    }

    case Dali::Actor::Property::WIDTH_FOR_HEIGHT:
    {
      value = (actor.mSizer.GetResizePolicy(Dimension::WIDTH) == ResizePolicy::DIMENSION_DEPENDENCY) && (actor.mSizer.GetDimensionDependency(Dimension::WIDTH) == Dimension::HEIGHT);
      break;
    }

    case Dali::Actor::Property::HEIGHT_FOR_WIDTH:
    {
      value = (actor.mSizer.GetResizePolicy(Dimension::HEIGHT) == ResizePolicy::DIMENSION_DEPENDENCY) && (actor.mSizer.GetDimensionDependency(Dimension::HEIGHT) == Dimension::WIDTH);
      break;
    }

    case Dali::Actor::Property::PADDING:
    {
      Vector2 widthPadding  = actor.GetPadding(Dimension::WIDTH);
      Vector2 heightPadding = actor.GetPadding(Dimension::HEIGHT);
      value                 = Vector4(widthPadding.x, widthPadding.y, heightPadding.x, heightPadding.y);
      break;
    }

    case Dali::Actor::Property::MINIMUM_SIZE:
    {
      value = Vector2(actor.GetMinimumSize(Dimension::WIDTH), actor.GetMinimumSize(Dimension::HEIGHT));
      break;
    }

    case Dali::Actor::Property::MAXIMUM_SIZE:
    {
      value = Vector2(actor.GetMaximumSize(Dimension::WIDTH), actor.GetMaximumSize(Dimension::HEIGHT));
      break;
    }

    case Dali::Actor::Property::CLIPPING_MODE:
    {
      value = actor.mClippingMode;
      break;
    }

    case Dali::DevelActor::Property::SIBLING_ORDER:
    {
      Actor* parent = actor.GetParent();
      if(parent)
      {
        value = static_cast<int>(parent->GetSiblingOrderOfChild(actor));
      }
      else
      {
        value = 0;
      }
      break;
    }

    case Dali::Actor::Property::SCREEN_POSITION:
    {
      value = actor.GetCurrentScreenPosition();
      break;
    }

    case Dali::Actor::Property::POSITION_USES_ANCHOR_POINT:
    {
      value = actor.mPositionUsesAnchorPoint;
      break;
    }

    case Dali::Actor::Property::LAYOUT_DIRECTION:
    {
      value = actor.mLayoutDirection;
      break;
    }

    case Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION:
    {
      value = actor.IsLayoutDirectionInherited();
      break;
    }

    case Dali::Actor::Property::ID:
    {
      value = static_cast<int>(actor.GetId());
      break;
    }

    case Dali::Actor::Property::HIERARCHY_DEPTH:
    {
      value = actor.GetHierarchyDepth();
      break;
    }

    case Dali::Actor::Property::IS_ROOT:
    {
      value = actor.IsRoot();
      break;
    }

    case Dali::Actor::Property::IS_LAYER:
    {
      value = actor.IsLayer();
      break;
    }

    case Dali::Actor::Property::CONNECTED_TO_SCENE:
    {
      value = actor.OnScene();
      break;
    }

    case Dali::Actor::Property::KEYBOARD_FOCUSABLE:
    {
      value = actor.IsKeyboardFocusable();
      break;
    }

    case Dali::DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START:
    {
      value = actor.mCaptureAllTouchAfterStart;
      break;
    }

    case Dali::DevelActor::Property::TOUCH_AREA_OFFSET:
    {
      value = actor.GetTouchAreaOffset();
      break;
    }

    case Dali::DevelActor::Property::BLEND_EQUATION:
    {
      value = actor.GetBlendEquation();
      break;
    }

    case Dali::DevelActor::Property::TOUCH_FOCUSABLE:
    {
      value = actor.IsTouchFocusable();
      break;
    }

    case Dali::DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN:
    {
      value = actor.AreChildrenKeyBoardFocusable();
      break;
    }

    case Dali::DevelActor::Property::USER_INTERACTION_ENABLED:
    {
      value = actor.IsUserInteractionEnabled();
      break;
    }

    case Dali::DevelActor::Property::ALLOW_ONLY_OWN_TOUCH:
    {
      value = actor.mAllowOnlyOwnTouch;
      break;
    }

    case Dali::DevelActor::Property::USE_TEXTURE_UPDATE_AREA:
    {
      value = actor.mUseTextureUpdateArea;
      break;
    }

    case Dali::DevelActor::Property::DISPATCH_TOUCH_MOTION:
    {
      value = actor.mDispatchTouchMotion;
      break;
    }

    case Dali::DevelActor::Property::DISPATCH_HOVER_MOTION:
    {
      value = actor.mDispatchHoverMotion;
      break;
    }

    case Dali::DevelActor::Property::CHILDREN_DEPTH_INDEX_POLICY:
    {
      value = actor.mChildrenDepthIndexPolicy;
      break;
    }

    default:
    {
      // Must be a scene-graph only property
      valueSet = false;
      break;
    }
  }

  return valueSet;
}

bool Actor::PropertyHandler::GetCurrentPropertyValue(const Internal::Actor& actor, Property::Index index, Property::Value& value)
{
  bool valueSet = true;

  switch(index)
  {
    case Dali::Actor::Property::SIZE:
    {
      value = actor.GetCurrentSize();
      break;
    }

    case Dali::Actor::Property::SIZE_WIDTH:
    {
      value = actor.GetCurrentSize().width;
      break;
    }

    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      value = actor.GetCurrentSize().height;
      break;
    }

    case Dali::Actor::Property::SIZE_DEPTH:
    {
      value = actor.GetCurrentSize().depth;
      break;
    }

    case Dali::Actor::Property::POSITION:
    {
      value = actor.GetCurrentPosition();
      break;
    }

    case Dali::Actor::Property::POSITION_X:
    {
      value = actor.GetCurrentPosition().x;
      break;
    }

    case Dali::Actor::Property::POSITION_Y:
    {
      value = actor.GetCurrentPosition().y;
      break;
    }

    case Dali::Actor::Property::POSITION_Z:
    {
      value = actor.GetCurrentPosition().z;
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION:
    {
      value = actor.GetCurrentWorldPosition();
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION_X:
    {
      value = actor.GetCurrentWorldPosition().x;
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION_Y:
    {
      value = actor.GetCurrentWorldPosition().y;
      break;
    }

    case Dali::Actor::Property::WORLD_POSITION_Z:
    {
      value = actor.GetCurrentWorldPosition().z;
      break;
    }

    case Dali::Actor::Property::ORIENTATION:
    {
      value = actor.GetCurrentOrientation();
      break;
    }

    case Dali::Actor::Property::WORLD_ORIENTATION:
    {
      value = actor.GetCurrentWorldOrientation();
      break;
    }

    case Dali::Actor::Property::SCALE:
    {
      value = actor.GetCurrentScale();
      break;
    }

    case Dali::Actor::Property::SCALE_X:
    {
      value = actor.GetCurrentScale().x;
      break;
    }

    case Dali::Actor::Property::SCALE_Y:
    {
      value = actor.GetCurrentScale().y;
      break;
    }

    case Dali::Actor::Property::SCALE_Z:
    {
      value = actor.GetCurrentScale().z;
      break;
    }

    case Dali::Actor::Property::WORLD_SCALE:
    {
      value = actor.GetCurrentWorldScale();
      break;
    }

    case Dali::Actor::Property::COLOR:
    {
      value = actor.GetCurrentColor();
      break;
    }

    case Dali::Actor::Property::COLOR_RED:
    {
      value = actor.GetCurrentColor().r;
      break;
    }

    case Dali::Actor::Property::COLOR_GREEN:
    {
      value = actor.GetCurrentColor().g;
      break;
    }

    case Dali::Actor::Property::COLOR_BLUE:
    {
      value = actor.GetCurrentColor().b;
      break;
    }

    case Dali::Actor::Property::COLOR_ALPHA:
    case Dali::Actor::Property::OPACITY:
    {
      value = actor.GetCurrentColor().a;
      break;
    }

    case Dali::Actor::Property::WORLD_COLOR:
    {
      value = actor.GetCurrentWorldColor();
      break;
    }

    case Dali::Actor::Property::WORLD_MATRIX:
    {
      value = actor.GetCurrentWorldMatrix();
      break;
    }

    case Dali::Actor::Property::VISIBLE:
    {
      value = actor.IsVisible();
      break;
    }

    case Dali::Actor::Property::CULLED:
    {
      value = actor.GetNode().IsCulled(actor.GetEventThreadServices().GetEventBufferIndex());
      break;
    }

    case Dali::Actor::Property::UPDATE_AREA_HINT:
    {
      // node is being used in a separate thread, the value from the previous update is the same, set by user
      value = Vector4(actor.GetNode().GetUpdateAreaHint());
      break;
    }

    case Dali::DevelActor::Property::USE_TEXTURE_UPDATE_AREA:
    {
      // node is being used in a separate thread, the value from the previous update is the same, set by user
      value = actor.GetNode().IsTextureUpdateAreaUsed();
      break;
    }
    default:
    {
      // Must be an event-side only property
      valueSet = false;
      break;
    }
  }

  return valueSet;
}

} // namespace Internal

} // namespace Dali
