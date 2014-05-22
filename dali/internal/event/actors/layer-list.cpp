//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/event/actors/layer-list.h>

// EXTERNAL INCLUDES
#include <algorithm>  // for std::swap

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{

namespace Internal
{



namespace // unnamed namespace
{

typedef std::vector<Layer*> LayerContainer;
typedef LayerContainer::iterator LayerIter;
typedef LayerContainer::reverse_iterator ReverseLayerIter;

/**
 * A private helper template to return an iterator to the layer passed in.
 * @param[in] first position to start searching from
 * @param[in] last position to search to
 * @param[in] layer to search for
 * @return iterator to layer if found
 */
template<class InputIterator> InputIterator Find( InputIterator first, InputIterator last, const Layer& layer )
{
  for( ; first != last ; ++first )
  {
    if( *first == &layer )
    {
      break;
    }
 }
 return first;
}

} // unnamed namespace

LayerList* LayerList::New( Stage& stage, bool systemLevel )
{
  return new LayerList( stage, systemLevel );
}

LayerList::~LayerList()
{
}

unsigned int LayerList::GetLayerCount() const
{
  return mLayers.size();
}

Layer* LayerList::GetLayer( unsigned int depth ) const
{
  DALI_ASSERT_ALWAYS( depth < mLayers.size() );

  return mLayers[ depth ];
}

unsigned int LayerList::GetDepth( const Layer* layer ) const
{
  for( unsigned int count = 0; count < mLayers.size(); ++count )
  {
    if( layer == mLayers[ count ] )
    {
      return count;
    }
  }
  return 0;
}

void LayerList::RegisterLayer(Layer& layer)
{
  DALI_ASSERT_DEBUG(  mLayers.end() == Find( mLayers.begin(), mLayers.end(), layer) );
  mLayers.push_back(&layer);

  SetLayerDepths();
}

void LayerList::UnregisterLayer(Layer& layer)
{
  // Find the layer...
  LayerIter iter = Find( mLayers.begin(), mLayers.end(), layer);
  DALI_ASSERT_DEBUG(iter != mLayers.end());

  // ...and remove it
  mLayers.erase(iter);

  SetLayerDepths();
}

void LayerList::RaiseLayer(Layer& raiseLayer)
{
  LayerIter iter = Find( mLayers.begin(), mLayers.end(), raiseLayer);

  if (iter   != mLayers.end() &&
      iter+1 != mLayers.end())
  {
    LayerIter nextIter = iter+1;

    // Swap the pointers
    std::swap(*iter, *nextIter);

    SetLayerDepths();
  }
}

void LayerList::LowerLayer(Layer& lowerLayer)
{
  ReverseLayerIter iter =  Find( mLayers.rbegin(), mLayers.rend(), lowerLayer);

  if (iter   != mLayers.rend() &&
      iter+1 != mLayers.rend())
  {
    ReverseLayerIter nextIter = iter+1;

    // Swap the pointers
    std::swap(*iter, *nextIter);

    SetLayerDepths();
  }
}

void LayerList::RaiseLayerToTop( const Layer& layer )
{
  LayerIter iter = Find( mLayers.begin(), mLayers.end(), layer);

  if (iter   != mLayers.end() &&
      iter+1 != mLayers.end())
  {
    Layer* raised = *iter;

    copy(iter+1, mLayers.end(), iter);
    mLayers.back() = raised;

    SetLayerDepths();
  }
}

void LayerList::LowerLayerToBottom( const Layer& layer )
{
  ReverseLayerIter iter =  Find( mLayers.rbegin(), mLayers.rend(), layer);

  if (iter   != mLayers.rend() &&
      iter+1 != mLayers.rend())
  {
    Layer* lowered = *iter;

    copy(iter+1, mLayers.rend(), iter);
    mLayers.front() = lowered;

    SetLayerDepths();
  }
}

void LayerList::MoveLayerAbove( const Layer& layer, const Layer& target )
{
  // check if it already is
  if( layer.GetDepth() == ( target.GetDepth() + 1 ) )
  {
    return;
  }

  // find the layer to move
  LayerIter iter = Find( mLayers.begin(), mLayers.end(), layer);

  if( iter != mLayers.end() )
  {
    Layer* moved = *iter;
    mLayers.erase( iter );
    // find target
    LayerIter iterT = Find( mLayers.begin(), mLayers.end(), target);
    // if target is not found there's a programming error somewhere
    DALI_ASSERT_DEBUG( iterT != mLayers.end() );
    // iterT might be the last
    if( ( iterT+1 ) == mLayers.end() )
    {
      mLayers.push_back( moved );
    }
    else
    {
      mLayers.insert( iterT+1, moved );
    }

    SetLayerDepths();
  }
}

void LayerList::MoveLayerBelow( const Layer& layer, const Layer& target )
{
  // check if it already is in correct order
  if( layer.GetDepth() == ( target.GetDepth() - 1 ) )
  {
    return;
  }

  // find the layer to move
  LayerIter iter = Find( mLayers.begin(), mLayers.end(), layer);
  if( iter != mLayers.end() )
  {
    Layer* moved = *iter;
    mLayers.erase( iter );
    // find target
    LayerIter iterT = Find( mLayers.begin(), mLayers.end(), target);
    // if target is not found there's a programming error somewhere
    DALI_ASSERT_DEBUG( iterT != mLayers.end() );
    mLayers.insert( iterT, moved );

    SetLayerDepths();
  }
}

LayerList::LayerList( Stage& stage, bool systemLevel )
: mStage( stage ),
  mIsSystemLevel( systemLevel )
{
}

void LayerList::SetLayerDepths()
{
  // we've got a list of on-stage layers on actor side, need to get their stage
  // pointers so we can send them to the update manager
  std::vector< SceneGraph::Layer* > layers;
  layers.reserve( mLayers.size() );

  // Set the layers (possibly) new depth
  for (LayerIter iter = mLayers.begin(); iter != mLayers.end(); ++iter)
  {
    SceneGraph::Layer* layerPtr = const_cast< SceneGraph::Layer* >( &( (*iter)->GetSceneLayerOnStage() ) );
    layers.push_back( layerPtr );
  }

  // Layers are being used in a separate thread; queue a message to set order
  SetLayerDepthsMessage( mStage.GetUpdateManager(), layers, mIsSystemLevel );
}

} // namespace Internal

} // namespace Dali
