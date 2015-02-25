/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/property-index-ranges.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/actors/layer-list.h>

using Dali::Internal::SceneGraph::UpdateManager;

namespace Dali
{

const Property::Index Layer::CLIPPING_ENABLE = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT;
const Property::Index Layer::CLIPPING_BOX    = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 1;

namespace Internal
{

namespace
{

// Actions

const char* const ACTION_RAISE =           "raise";
const char* const ACTION_LOWER =           "lower";
const char* const ACTION_RAISE_TO_TOP =    "raise-to-top";
const char* const ACTION_LOWER_TO_BOTTOM = "lower-to-bottom";

BaseHandle Create()
{
  return Dali::Layer::New();
}

TypeRegistration mType( typeid( Dali::Layer ), typeid( Dali::Actor ), Create );

TypeAction a1( mType, ACTION_RAISE, &Layer::DoAction );
TypeAction a2( mType, ACTION_LOWER, &Layer::DoAction );
TypeAction a3( mType, ACTION_RAISE_TO_TOP, &Layer::DoAction );
TypeAction a4( mType, ACTION_LOWER_TO_BOTTOM, &Layer::DoAction );

const PropertyDetails DEFAULT_PROPERTY_DETAILS[] =
{
 // Name                     Type              writable animatable constraint-input
 { "clipping-enable",     Property::BOOLEAN,    true,    false,   true  },  // CLIPPING_ENABLE
 { "clipping-box",        Property::RECTANGLE,  true,    false,   true  },  // CLIPPING_BOX
};
const int DEFAULT_LAYER_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_DETAILS ) / sizeof( DEFAULT_PROPERTY_DETAILS[0] );

} // unnamed namespace

LayerPtr Layer::New()
{
  LayerPtr layer( new Layer( Actor::LAYER ) );

  // Second-phase construction
  layer->Initialize();

  return layer;
}

LayerPtr Layer::NewRoot( Stage& stage, LayerList& layerList, UpdateManager& manager, bool systemLevel )
{
  LayerPtr root( new Layer( Actor::ROOT_LAYER ) );

  // Second-phase construction
  SceneGraph::Layer* layer = static_cast<SceneGraph::Layer*>( root->CreateNode() );
  InstallRootMessage( manager, *layer, systemLevel ); // Transfer ownership to scene-graph

  // Keep a raw pointer to the layer node.
  root->mNode = layer;

  // stage must be set for the root layer
  root->mStage = &stage;

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
  mLayerList(NULL),
  mClippingBox(0,0,0,0),
  mSortFunction(Dali::Layer::ZValue),
  mIsClipping(false),
  mDepthTestDisabled(false),
  mTouchConsumed(false),
  mHoverConsumed(false)
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

void Layer::SetClipping(bool enabled)
{
  if (enabled != mIsClipping)
  {
    mIsClipping = enabled;

    // layerNode is being used in a separate thread; queue a message to set the value
    SetClippingMessage( mStage->GetUpdateInterface(), GetSceneLayerOnStage(), mIsClipping );
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
    clippingBox.y = mStage->GetSize().height - clippingBox.y - clippingBox.height;

    // layerNode is being used in a separate thread; queue a message to set the value
    SetClippingBoxMessage( mStage->GetUpdateInterface(), GetSceneLayerOnStage(), clippingBox );
  }
}

void Layer::SetDepthTestDisabled( bool disable )
{
  if( disable != mDepthTestDisabled )
  {
    mDepthTestDisabled = disable;

    // Send message .....
    // layerNode is being used in a separate thread; queue a message to set the value
    SetDepthTestDisabledMessage( mStage->GetUpdateInterface(), GetSceneLayerOnStage(), mDepthTestDisabled );
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
    SetSortFunctionMessage( mStage->GetUpdateInterface(), GetSceneLayerOnStage(), mSortFunction );
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
  return Actor::GetDefaultPropertyCount() + DEFAULT_LAYER_PROPERTY_COUNT;
}

void Layer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  Actor::GetDefaultPropertyIndices( indices ); // Actor class properties
  indices.reserve( indices.size() + DEFAULT_LAYER_PROPERTY_COUNT );

  int index = DEFAULT_ACTOR_PROPERTY_MAX_COUNT;
  for ( int i = 0; i < DEFAULT_LAYER_PROPERTY_COUNT; ++i, ++index )
  {
    indices.push_back( index );
  }
}

bool Layer::IsDefaultPropertyWritable( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::IsDefaultPropertyWritable(index);
  }
  else
  {
    return true; // all properties writable, no need to lookup the table
  }
}

bool Layer::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::IsDefaultPropertyAnimatable(index);
  }
  else
  {
    return false; // all properties non animateable, no need to lookup the table
  }
}

bool Layer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::IsDefaultPropertyAConstraintInput(index);
  }
  return true; // our properties can be used as an input to a constraint, no need to lookup the table
}

Property::Type Layer::GetDefaultPropertyType( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyType(index);
  }
  else
  {
    index -= DEFAULT_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_LAYER_PROPERTY_COUNT ) )
    {
      return DEFAULT_PROPERTY_DETAILS[index].type;
    }
    else
    {
      // index out-of-bounds
      return Property::NONE;
    }
  }
}

const char* Layer::GetDefaultPropertyName( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyName(index);
  }
  else
  {
    index -= DEFAULT_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_LAYER_PROPERTY_COUNT ) )
    {
      return DEFAULT_PROPERTY_DETAILS[index].name;
    }
    else
    {
      return NULL;
    }
  }
}

Property::Index Layer::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in current class' default properties
  for( int i = 0; i < DEFAULT_LAYER_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
    {
      index = i + DEFAULT_ACTOR_PROPERTY_MAX_COUNT;
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
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    Actor::SetDefaultProperty(index, propertyValue);
  }
  else
  {
    switch(index)
    {
      case Dali::Layer::CLIPPING_ENABLE:
      {
        SetClipping( propertyValue.Get<bool>() );
        break;
      }
      case Dali::Layer::CLIPPING_BOX:
      {
        Rect<int> clippingBox( propertyValue.Get<Rect<int> >() );
        SetClippingBox( clippingBox.x, clippingBox.y, clippingBox.width, clippingBox.height );
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value Layer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret;
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = Actor::GetDefaultProperty(index);
  }
  else
  {
    switch(index)
    {
      case Dali::Layer::CLIPPING_ENABLE:
      {
        ret = mIsClipping;
        break;
      }
      case Dali::Layer::CLIPPING_BOX:
      {
        ret = mClippingBox;
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)
  }

  return ret;
}

bool Layer::DoAction( BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes )
{
  bool done = false;
  Layer* layer = dynamic_cast<Layer*>( object );

  if( layer )
  {
    if( 0 == strcmp( actionName.c_str(), ACTION_RAISE ) )
    {
      layer->Raise();
      done = true;
    }
    else if( 0 == strcmp( actionName.c_str(), ACTION_LOWER ) )
    {
      layer->Lower();
      done = true;
    }
    else if( 0 == strcmp( actionName.c_str(), ACTION_RAISE_TO_TOP ) )
    {
      layer->RaiseToTop();
      done = true;
    }
    else if( 0 == strcmp( actionName.c_str(), ACTION_LOWER_TO_BOTTOM ) )
    {
      layer->LowerToBottom();
      done = true;
    }
  }

  return done;
}

} // namespace Internal

} // namespace Dali

