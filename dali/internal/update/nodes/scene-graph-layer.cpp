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
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/event/actors/layer-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

SceneGraph::Layer* Layer::New()
{
  return new Layer();
}

Layer::Layer()
: mSortFunction( Internal::Layer::ZValue ),
  mClippingBox( 0,0,0,0 ),
  mLastCamera( NULL ),
  mBehavior( Dali::Layer::LAYER_2D ),
  mIsClipping( false ),
  mDepthTestDisabled( false ),
  mIsDefaultSortFunction( true )
{
  // layer starts off dirty
  mAllChildTransformsClean[ 0 ] = false;
  mAllChildTransformsClean[ 1 ] = false;
}

Layer::~Layer()
{
}

void Layer::SetSortFunction( Dali::Layer::SortFunctionType function )
{
  if( mSortFunction != function )
  {
    // is a custom sort function used
    if( function != Internal::Layer::ZValue )
    {
      mIsDefaultSortFunction = false;
    }
    else
    {
      mIsDefaultSortFunction = true;
    }

    // changing the sort function makes the layer dirty
    mAllChildTransformsClean[ 0 ] = false;
    mAllChildTransformsClean[ 1 ] = false;
    mSortFunction = function;
  }
}

void Layer::SetClipping(bool enabled)
{
  mIsClipping = enabled;
}

void Layer::SetClippingBox(const Dali::ClippingBox& box)
{
  mClippingBox.Set(box.x, box.y, box.width, box.height);
}

void Layer::SetBehavior( Dali::Layer::Behavior behavior )
{
  mBehavior = behavior;
}

void Layer::SetDepthTestDisabled( bool disable )
{
  mDepthTestDisabled = disable;
}

bool Layer::IsDepthTestDisabled() const
{
  return ( mBehavior == Dali::Layer::LAYER_2D ) || mDepthTestDisabled;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
