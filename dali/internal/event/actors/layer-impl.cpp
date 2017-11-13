/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/layer-impl.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>

using Dali::Internal::SceneGraph::UpdateManager;

namespace Dali
{

namespace
{

typedef Layer::Behavior Behavior;

DALI_ENUM_TO_STRING_TABLE_BEGIN( BEHAVIOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Layer, LAYER_2D )
DALI_ENUM_TO_STRING_WITH_SCOPE( Layer, LAYER_3D )
DALI_ENUM_TO_STRING_TABLE_END( BEHAVIOR )

} // namespace

namespace Internal
{

namespace
{

// Properties

//              Name                Type      writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "clippingEnable",    BOOLEAN,    true,    false,   true,   Dali::Layer::Property::CLIPPING_ENABLE )
DALI_PROPERTY( "clippingBox",       RECTANGLE,  true,    false,   true,   Dali::Layer::Property::CLIPPING_BOX    )
DALI_PROPERTY( "behavior",          STRING,     true,    false,   false,  Dali::Layer::Property::BEHAVIOR        )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX )

// Actions

const char* const ACTION_RAISE =           "raise";
const char* const ACTION_LOWER =           "lower";
const char* const ACTION_RAISE_TO_TOP =    "raiseToTop";
const char* const ACTION_LOWER_TO_BOTTOM = "lowerToBottom";

BaseHandle Create()
{
  return Dali::Layer::New();
}

TypeRegistration mType( typeid( Dali::Layer ), typeid( Dali::Actor ), Create );

TypeAction a1( mType, ACTION_RAISE, &Layer::DoAction );
TypeAction a2( mType, ACTION_LOWER, &Layer::DoAction );
TypeAction a3( mType, ACTION_RAISE_TO_TOP, &Layer::DoAction );
TypeAction a4( mType, ACTION_LOWER_TO_BOTTOM, &Layer::DoAction );

} // unnamed namespace


LayerPtr Layer::New()
{
  LayerPtr layer( new Layer( Actor::LAYER ) );

  // Second-phase construction
  layer->Initialize();

  return layer;
}

LayerPtr Layer::NewRoot( LayerList& layerList, UpdateManager& manager, bool systemLevel )
{
  LayerPtr root( new Layer( Actor::ROOT_LAYER ) );

  // Second-phase construction, keep a raw pointer to the layer node.
  SceneGraph::Layer* rootLayer = static_cast<SceneGraph::Layer*>( root->CreateNode() );
  root->mNode = rootLayer;
  OwnerPointer< SceneGraph::Layer > transferOwnership( rootLayer );
  InstallRootMessage( manager, transferOwnership, systemLevel );

  // root actor is immediately considered to be on-stage
  root->mIsOnStage = true;

  // The root actor will not emit a stage connection signal so set the signalled flag here as well
  root->mOnStageSignalled = true;

  // layer-list must be set for the root layer
  root->mLayerList = &layerList;
  layerList.RegisterLayer( *root );

  return root;
}

Layer::Layer( Actor::DerivedType type )
: Actor( type ),
  mLayerList( NULL ),
  mClippingBox( 0, 0, 0, 0 ),
  mSortFunction( Layer::ZValue ),
  mBehavior( Dali::Layer::LAYER_2D ),
  mIsClipping( false ),
  mDepthTestDisabled( true ),
  mTouchConsumed( false ),
  mHoverConsumed( false )
{
}

void Layer::OnInitialize()
{
}

Layer::~Layer()
{
}

unsigned int Layer::GetDepth() const
{
  return mLayerList ? mLayerList->GetDepth( this ) : 0u;
}

void Layer::Raise()
{
  if ( mLayerList )
  {
    mLayerList->RaiseLayer(*this);
  }
}

void Layer::Lower()
{
  if ( mLayerList )
  {
    mLayerList->LowerLayer(*this);
  }
}

void Layer::RaiseAbove( const Internal::Layer& target )
{
  // cannot raise above ourself, both have to be on stage
  if( ( this != &target ) && OnStage() && target.OnStage() )
  {
    // get parameters depth
    const unsigned int targetDepth = target.GetDepth();
    if( GetDepth() < targetDepth )
    {
      MoveAbove( target );
    }
  }
}

void Layer::LowerBelow( const Internal::Layer& target )
{
  // cannot lower below ourself, both have to be on stage
  if( ( this != &target ) && OnStage() && target.OnStage() )
  {
    // get parameters depth
    const unsigned int targetDepth = target.GetDepth();
    if( GetDepth() > targetDepth )
    {
      MoveBelow( target );
    }
  }
}

void Layer::RaiseToTop()
{
  if ( mLayerList )
  {
    mLayerList->RaiseLayerToTop(*this);
  }
}

void Layer::LowerToBottom()
{
  if ( mLayerList )
  {
    mLayerList->LowerLayerToBottom(*this);
  }
}

void Layer::MoveAbove( const Internal::Layer& target )
{
  // cannot raise above ourself, both have to be on stage
  if( ( this != &target ) && mLayerList && target.OnStage() )
  {
    mLayerList->MoveLayerAbove(*this, target );
  }
}

void Layer::MoveBelow( const Internal::Layer& target )
{
  // cannot lower below ourself, both have to be on stage
  if( ( this != &target ) && mLayerList && target.OnStage() )
  {
    mLayerList->MoveLayerBelow(*this, target );
  }
}

void Layer::SetBehavior( Dali::Layer::Behavior behavior )
{
  mBehavior = behavior;

  // Notify update side object.
  SetBehaviorMessage( GetEventThreadServices(), GetSceneLayerOnStage(), behavior );
  // By default, disable depth test for LAYER_2D, and enable for LAYER_3D.
  SetDepthTestDisabled( mBehavior == Dali::Layer::LAYER_2D );
}

void Layer::SetClipping(bool enabled)
{
  if (enabled != mIsClipping)
  {
    mIsClipping = enabled;

    // layerNode is being used in a separate thread; queue a message to set the value
    SetClippingMessage( GetEventThreadServices(), GetSceneLayerOnStage(), mIsClipping );
  }
}

void Layer::SetClippingBox(int x, int y, int width, int height)
{
  if( ( x != mClippingBox.x ) ||
      ( y != mClippingBox.y ) ||
      ( width != mClippingBox.width ) ||
      ( height != mClippingBox.height ) )
  {
    // Clipping box is not animatable; this is the most up-to-date value
    mClippingBox.Set(x, y, width, height);

    // Convert mClippingBox to GL based coordinates (from bottom-left)
    ClippingBox clippingBox( mClippingBox );

    StagePtr stage = Stage::GetCurrent();
    if( stage )
    {
      clippingBox.y = stage->GetSize().height - clippingBox.y - clippingBox.height;

      // layerNode is being used in a separate thread; queue a message to set the value
      SetClippingBoxMessage( GetEventThreadServices(), GetSceneLayerOnStage(), clippingBox );
    }
  }
}

void Layer::SetDepthTestDisabled( bool disable )
{
  if( disable != mDepthTestDisabled )
  {
    mDepthTestDisabled = disable;

    // Send message.
    // layerNode is being used in a separate thread; queue a message to set the value
    SetDepthTestDisabledMessage( GetEventThreadServices(), GetSceneLayerOnStage(), mDepthTestDisabled );
  }
}

bool Layer::IsDepthTestDisabled() const
{
  return mDepthTestDisabled;
}

void Layer::SetSortFunction(Dali::Layer::SortFunctionType function)
{
  if( function != mSortFunction )
  {
    mSortFunction = function;

    // layerNode is being used in a separate thread; queue a message to set the value
    SetSortFunctionMessage( GetEventThreadServices(), GetSceneLayerOnStage(), mSortFunction );
  }
}

void Layer::SetTouchConsumed( bool consume )
{
  mTouchConsumed = consume;
}

bool Layer::IsTouchConsumed() const
{
  return mTouchConsumed;
}

void Layer::SetHoverConsumed( bool consume )
{
  mHoverConsumed = consume;
}

bool Layer::IsHoverConsumed() const
{
  return mHoverConsumed;
}

SceneGraph::Node* Layer::CreateNode() const
{
  return SceneGraph::Layer::New();
}

void Layer::OnStageConnectionInternal()
{
  if ( !mIsRoot )
  {
    DALI_ASSERT_DEBUG( NULL == mLayerList );

    // Find the ordered layer-list
    // This is different for Layers added via Integration::GetSystemOverlay()
    for ( Actor* parent = mParent; parent != NULL; parent = parent->GetParent() )
    {
      if( parent->IsLayer() )
      {
        Layer* parentLayer = static_cast< Layer* >( parent ); // cheaper than dynamic_cast
        mLayerList = parentLayer->mLayerList;
      }
    }
  }

  DALI_ASSERT_DEBUG( NULL != mLayerList );
  mLayerList->RegisterLayer( *this );
}

void Layer::OnStageDisconnectionInternal()
{
  mLayerList->UnregisterLayer(*this);

  // mLayerList is only valid when on-stage
  mLayerList = NULL;
}

const SceneGraph::Layer& Layer::GetSceneLayerOnStage() const
{
  DALI_ASSERT_DEBUG( mNode != NULL );
  return dynamic_cast< const SceneGraph::Layer& >( *mNode );
}

unsigned int Layer::GetDefaultPropertyCount() const
{
  return Actor::GetDefaultPropertyCount() + DEFAULT_PROPERTY_COUNT;
}

void Layer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  Actor::GetDefaultPropertyIndices( indices ); // Actor class properties
  indices.Reserve( indices.Size() + DEFAULT_PROPERTY_COUNT );

  int index = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i, ++index )
  {
    indices.PushBack( index );
  }
}

bool Layer::IsDefaultPropertyWritable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyWritable( index );
  }

  return DEFAULT_PROPERTY_DETAILS[ index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX ].writable;
}

bool Layer::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAnimatable( index );
  }

  return DEFAULT_PROPERTY_DETAILS[ index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX ].animatable;
}

bool Layer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAConstraintInput( index );
  }

  return DEFAULT_PROPERTY_DETAILS[ index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX ].constraintInput;
}

Property::Type Layer::GetDefaultPropertyType( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::GetDefaultPropertyType( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;

  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out-of-bounds
  return Property::NONE;
}

const char* Layer::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::GetDefaultPropertyName( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  return NULL;
}

Property::Index Layer::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in current class' default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[i];
    if( 0 == name.compare( property->name ) ) // dont want to convert rhs to string
    {
      index = i + DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
      break;
    }
  }
  if( Property::INVALID_INDEX == index )
  {
    // If not found, check in base class
    index = Actor::GetDefaultPropertyIndex( name );
  }

  return index;
}

void Layer::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    Actor::SetDefaultProperty( index, propertyValue );
  }
  else
  {
    switch( index )
    {
      case Dali::Layer::Property::CLIPPING_ENABLE:
      {
        SetClipping( propertyValue.Get<bool>() );
        break;
      }
      case Dali::Layer::Property::CLIPPING_BOX:
      {
        Rect<int> clippingBox( propertyValue.Get<Rect<int> >() );
        SetClippingBox( clippingBox.x, clippingBox.y, clippingBox.width, clippingBox.height );
        break;
      }
      case Dali::Layer::Property::BEHAVIOR:
      {
        Behavior behavior(Dali::Layer::LAYER_2D);
        if( Scripting::GetEnumeration< Behavior >( propertyValue.Get< std::string >().c_str(), BEHAVIOR_TABLE, BEHAVIOR_TABLE_COUNT, behavior ) )
        {
          SetBehavior( behavior );
        }
        break;
      }
      default:
      {
        DALI_LOG_WARNING( "Unknown property (%d)\n", index );
        break;
      }
    } // switch(index)

  } // else
}

Property::Value Layer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret;
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    ret = Actor::GetDefaultProperty( index );
  }
  else
  {
    switch( index )
    {
      case Dali::Layer::Property::CLIPPING_ENABLE:
      {
        ret = mIsClipping;
        break;
      }
      case Dali::Layer::Property::CLIPPING_BOX:
      {
        ret = mClippingBox;
        break;
      }
      case Dali::Layer::Property::BEHAVIOR:
      {
        ret = Scripting::GetLinearEnumerationName< Behavior >( GetBehavior(), BEHAVIOR_TABLE, BEHAVIOR_TABLE_COUNT );
        break;
      }
      default:
      {
        DALI_LOG_WARNING( "Unknown property (%d)\n", index );
        break;
      }
    } // switch(index)
  }

  return ret;
}

Property::Value Layer::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  Property::Value ret;
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    ret = Actor::GetDefaultPropertyCurrentValue( index );
  }
  else
  {
    ret = GetDefaultProperty( index ); // Layer only has event-side properties
  }

  return ret;
}

bool Layer::DoAction( BaseObject* object, const std::string& actionName, const Property::Map& /*attributes*/ )
{
  bool done = false;
  Layer* layer = dynamic_cast<Layer*>( object );

  if( layer )
  {
    if( 0 == actionName.compare( ACTION_RAISE ) )
    {
      layer->Raise();
      done = true;
    }
    else if( 0 == actionName.compare( ACTION_LOWER ) )
    {
      layer->Lower();
      done = true;
    }
    else if( 0 == actionName.compare( ACTION_RAISE_TO_TOP ) )
    {
      layer->RaiseToTop();
      done = true;
    }
    else if( 0 == actionName.compare( ACTION_LOWER_TO_BOTTOM ) )
    {
      layer->LowerToBottom();
      done = true;
    }
  }

  return done;
}

} // namespace Internal

} // namespace Dali
