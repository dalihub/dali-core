#ifndef DALI_INTERNAL_SCENE_GRAPH_OBJECT_OWNER_CONTAINER
#define DALI_INTERNAL_SCENE_GRAPH_OBJECT_OWNER_CONTAINER

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/internal/common/owner-container.h>
#include <dali/internal/update/common/discard-queue.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class UpdateManager;

/**
 * ObjectOwnerContainer is an object which owns SceneGraph Objects.
 * It is responsible for ensuring they are placed on a discard queue
 * when removed from the container.
 */
template< class Type >
class ObjectOwnerContainer
{
public:
  typedef typename Internal::OwnerContainer< Type* > ObjectContainer;
  typedef typename Internal::OwnerContainer< Type* >::Iterator Iterator;

  /**
   * @brief Constructor - createw a new object container
   *
   * Object container own update side objects
   *
   * @param[in] sceneGraphBuffers Helper to get the correct buffer index
   * @param[in] discardQueue Queue to discard objects that might still be in use in the render thread.
   **/
  ObjectOwnerContainer( SceneGraphBuffers& sceneGraphBuffers, DiscardQueue& discardQueue )
  : mSceneGraphBuffers( sceneGraphBuffers ),
    mDiscardQueue( discardQueue )
  {
  }

  /**
   * @brief Add an object to the owner
   *
   * @param[in] object Pointer to the object that will be owned
   **/
  void Add( Type* pointer )
  {
    DALI_ASSERT_DEBUG( pointer && "Pointer should not be null" );

    mObjectContainer.PushBack( pointer );
  }

  /**
   * @brief Remove an object from the owner.
   *
   * The object is put on the discard queue.
   *
   * @param[in] object Pointer to the object to be removed
   **/
  void Remove( Type* pointer )
  {
    DALI_ASSERT_DEBUG( pointer && "Pointer should not be null" );

    // Find the object
    Iterator match = std::find( mObjectContainer.Begin(), mObjectContainer.End(), pointer );
    DALI_ASSERT_DEBUG( match != mObjectContainer.End() && "Should always find a match" );

    mDiscardQueue.Add( mSceneGraphBuffers.GetUpdateBufferIndex(), mObjectContainer.Release( match ) );
  }

  /**
   * @brief Method to call ResetToBaseValues on all the objects owned.
   *
   * @param[in] bufferIndex Buffer index for double buffered values.
   **/
  void ResetToBaseValues( BufferIndex bufferIndex )
  {
    for ( Iterator iter = mObjectContainer.Begin(); iter != mObjectContainer.End(); ++iter)
    {
      Type* object = (*iter);
      object->ResetToBaseValues( bufferIndex );
    }
  }

  /**
   * @brief Method to call ConstrainObjects on all the objects owned.
   *
   * @param[in] bufferIndex Buffer index for double buffered values.
   **/
  unsigned int ConstrainObjects( BufferIndex bufferIndex )
  {
    unsigned int numberOfConstrainedObjects = 0;

    for ( Iterator iter = mObjectContainer.Begin(); iter != mObjectContainer.End(); ++iter)
    {
      Type* object = (*iter);
      numberOfConstrainedObjects += ConstrainPropertyOwner( *object, bufferIndex );
    }
    return numberOfConstrainedObjects;
  }

private:
  ObjectContainer mObjectContainer;       ///< Container for the objects owned
  SceneGraphBuffers& mSceneGraphBuffers;  ///< Reference to a SceneGraphBuffers to get the indexBuffer
  DiscardQueue& mDiscardQueue;            ///< Discard queue used for removed objects
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali


#endif // DALI_INTERNAL_SCENE_GRAPH_OBJECT_OWNER_CONTAINER
