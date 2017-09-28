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
#include <dali/internal/update/manager/update-manager.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/threading/mutex.h>

#include <dali/integration-api/core.h>
#include <dali/integration-api/render-controller.h>
#include <dali/internal/common/shader-data.h>
#include <dali/integration-api/debug.h>

#include <dali/internal/common/core-impl.h>
#include <dali/internal/common/message.h>

#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/event/common/property-notifier.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/animation/animation-playlist.h>

#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller-impl.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/manager/render-task-processor.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/manager/update-algorithms.h>
#include <dali/internal/update/manager/update-manager-debug.h>
#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/queue/update-message-queue.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>

#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

// Un-comment to enable node tree debug logging
//#define NODE_TREE_LOGGING 1

#if ( defined( DEBUG_ENABLED ) && defined( NODE_TREE_LOGGING ) )
#define SNAPSHOT_NODE_LOGGING \
const int FRAME_COUNT_TRIGGER = 16;\
if( mImpl->frameCounter >= FRAME_COUNT_TRIGGER )\
  {\
    if ( NULL != mImpl->root )\
    {\
      mImpl->frameCounter = 0;\
      PrintNodeTree( *mImpl->root, mSceneGraphBuffers.GetUpdateBufferIndex(), "" );\
    }\
  }\
mImpl->frameCounter++;
#else
#define SNAPSHOT_NODE_LOGGING
#endif

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gRenderTaskLogFilter;
#endif


using namespace Dali::Integration;
using Dali::Internal::Update::MessageQueue;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{
/**
 * Helper to reset animate-able objects to base values
 * @param container to iterate over
 * @param updateBufferIndex to use
 */
template< class T >
inline void ResetToBaseValues( OwnerContainer<T*>& container, BufferIndex updateBufferIndex )
{
  // Reset animatable properties to base values
  // use reference to avoid extra copies of the iterator
  for( auto&& iter : container )
  {
    iter->ResetToBaseValues( updateBufferIndex );
  }
}

/**
 * Helper to Erase an object from OwnerContainer using discard queue
 * @param container to remove from
 * @param object to remove
 * @param discardQueue to put the object to
 * @param updateBufferIndex to use
 */
template < class T >
inline void EraseUsingDiscardQueue( OwnerContainer<T*>& container, T* object, DiscardQueue& discardQueue, BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( object && "NULL object not allowed" );

  // need to use the reference version of auto as we need the pointer to the pointer for the Release call below
  for( auto&& iter : container )
  {
    if ( iter == object )
    {
      // Transfer ownership to the discard queue, this keeps the object alive, until the render-thread has finished with it
      discardQueue.Add( updateBufferIndex, container.Release( &iter ) ); // take the address of the reference to a pointer (iter)
      return; // return as we only ever remove one object. Iterators to container are now invalidated as well so cannot continue
    }
  }
}

/**
 * Descends into node's hierarchy and sorts the children of each child according to their depth-index.
 * @param[in] node The node whose hierarchy to descend
 */
void SortSiblingNodesRecursively( Node& node )
{
  NodeContainer& container = node.GetChildren();
  std::sort( container.Begin(), container.End(),
             []( Node* a, Node* b ) { return a->GetDepthIndex() < b->GetDepthIndex(); } );

  // Descend tree and sort as well
  for( auto&& iter : container )
  {
    SortSiblingNodesRecursively( *iter );
  }
}

} // unnamed namespace

/**
 * Structure to contain UpdateManager internal data
 */
struct UpdateManager::Impl
{
  Impl( NotificationManager& notificationManager,
        CompleteNotificationInterface& animationPlaylist,
        PropertyNotifier& propertyNotifier,
        DiscardQueue& discardQueue,
        RenderController& renderController,
        RenderManager& renderManager,
        RenderQueue& renderQueue,
        SceneGraphBuffers& sceneGraphBuffers,
        RenderTaskProcessor& renderTaskProcessor )
  : renderMessageDispatcher( renderManager, renderQueue, sceneGraphBuffers ),
    notificationManager( notificationManager ),
    transformManager(),
    animationPlaylist( animationPlaylist ),
    propertyNotifier( propertyNotifier ),
    shaderSaver( NULL ),
    discardQueue( discardQueue ),
    renderController( renderController ),
    sceneController( NULL ),
    renderManager( renderManager ),
    renderQueue( renderQueue ),
    renderInstructions( renderManager.GetRenderInstructionContainer() ),
    renderTaskProcessor( renderTaskProcessor ),
    backgroundColor( Dali::Stage::DEFAULT_BACKGROUND_COLOR ),
    taskList( renderMessageDispatcher ),
    systemLevelTaskList( renderMessageDispatcher ),
    root( NULL ),
    systemLevelRoot( NULL ),
    renderers(),
    textureSets(),
    shaders(),
    panGestureProcessor( NULL ),
    messageQueue( renderController, sceneGraphBuffers ),
    keepRenderingSeconds( 0.0f ),
    nodeDirtyFlags( TransformFlag ), // set to TransformFlag to ensure full update the first time through Update()
    frameCounter( 0 ),
    animationFinishedDuringUpdate( false ),
    previousUpdateScene( false ),
    renderTaskWaiting( false ),
    renderersAdded( false )
  {
    sceneController = new SceneControllerImpl( renderMessageDispatcher, renderQueue, discardQueue );

    // create first 'dummy' node
    nodes.PushBack(0u);
  }

  ~Impl()
  {
    // Disconnect render tasks from nodes, before destroying the nodes
    RenderTaskList::RenderTaskContainer& tasks = taskList.GetTasks();
    for ( auto&& iter : tasks )
    {
      iter->SetSourceNode( NULL );
    }
    // ..repeat for system level RenderTasks
    RenderTaskList::RenderTaskContainer& systemLevelTasks = systemLevelTaskList.GetTasks();
    for ( auto&& iter : systemLevelTasks )
    {
      iter->SetSourceNode( NULL );
    }

    // UpdateManager owns the Nodes. Although Nodes are pool allocated they contain heap allocated parts
    // like custom properties, which get released here
    Vector<Node*>::Iterator iter = nodes.Begin()+1;
    Vector<Node*>::Iterator endIter = nodes.End();
    for(;iter!=endIter;++iter)
    {
      (*iter)->OnDestroy();
      Node::Delete(*iter);
    }

    // If there is root, reset it, otherwise do nothing as rendering was never started
    if( root )
    {
      root->OnDestroy();

      Node::Delete( root );
      root = NULL;
    }

    if( systemLevelRoot )
    {
      systemLevelRoot->OnDestroy();

      Node::Delete( systemLevelRoot );
      systemLevelRoot = NULL;
    }

    delete sceneController;
  }

  SceneGraphBuffers                    sceneGraphBuffers;             ///< Used to keep track of which buffers are being written or read
  RenderMessageDispatcher              renderMessageDispatcher;       ///< Used for passing messages to the render-thread
  NotificationManager&                 notificationManager;           ///< Queues notification messages for the event-thread.
  TransformManager                     transformManager;              ///< Used to update the transformation matrices of the nodes
  CompleteNotificationInterface&       animationPlaylist;             ///< Holds handles to all the animations
  PropertyNotifier&                    propertyNotifier;              ///< Provides notification to applications when properties are modified.
  ShaderSaver*                         shaderSaver;                   ///< Saves shader binaries.
  DiscardQueue&                        discardQueue;                  ///< Nodes are added here when disconnected from the scene-graph.
  RenderController&                    renderController;              ///< render controller
  SceneControllerImpl*                 sceneController;               ///< scene controller
  RenderManager&                       renderManager;                 ///< This is responsible for rendering the results of each "update"
  RenderQueue&                         renderQueue;                   ///< Used to queue messages for the next render
  RenderInstructionContainer&          renderInstructions;            ///< Used to prepare the render instructions
  RenderTaskProcessor&                 renderTaskProcessor;           ///< Handles RenderTasks and RenderInstrucitons

  Vector4                              backgroundColor;               ///< The glClear color used at the beginning of each frame.

  RenderTaskList                       taskList;                      ///< The list of scene graph render-tasks
  RenderTaskList                       systemLevelTaskList;           ///< Separate render-tasks for system-level content

  Layer*                               root;                          ///< The root node (root is a layer)
  Layer*                               systemLevelRoot;               ///< A separate root-node for system-level content

  Vector<Node*>                        nodes;                         ///< A container of all instantiated nodes

  SortedLayerPointers                  sortedLayers;                  ///< A container of Layer pointers sorted by depth
  SortedLayerPointers                  systemLevelSortedLayers;       ///< A separate container of system-level Layers

  OwnerContainer< Camera* >            cameras;                       ///< A container of cameras
  OwnerContainer< PropertyOwner* >     customObjects;                 ///< A container of owned objects (with custom properties)

  AnimationContainer                   animations;                    ///< A container of owned animations
  PropertyNotificationContainer        propertyNotifications;         ///< A container of owner property notifications.

  OwnerContainer< Renderer* >          renderers;                     ///< A container of owned renderers
  OwnerContainer< TextureSet* >        textureSets;                   ///< A container of owned texture sets
  OwnerContainer< Shader* >            shaders;                       ///< A container of owned shaders
  OwnerPointer< PanGesture >           panGestureProcessor;           ///< Owned pan gesture processor; it lives for the lifecycle of UpdateManager

  MessageQueue                         messageQueue;                  ///< The messages queued from the event-thread
  std::vector<Internal::ShaderDataPtr> renderCompiledShaders;         ///< Shaders compiled on Render thread are inserted here for update thread to pass on to event thread.
  std::vector<Internal::ShaderDataPtr> updateCompiledShaders;         ///< Shaders to be sent from Update to Event
  Mutex                                compiledShaderMutex;           ///< lock to ensure no corruption on the renderCompiledShaders

  float                                keepRenderingSeconds;          ///< Set via Dali::Stage::KeepRendering
  int                                  nodeDirtyFlags;                ///< cumulative node dirty flags from previous frame
  int                                  frameCounter;                  ///< Frame counter used in debugging to choose which frame to debug and which to ignore.

  bool                                 animationFinishedDuringUpdate; ///< Flag whether any animations finished during the Update()
  bool                                 previousUpdateScene;           ///< True if the scene was updated in the previous frame (otherwise it was optimized out)
  bool                                 renderTaskWaiting;             ///< A REFRESH_ONCE render task is waiting to be rendered
  bool                                 renderersAdded;                ///< Flag to keep track when renderers have been added to avoid unnecessary processing

private:

  Impl( const Impl& ); ///< Undefined
  Impl& operator=( const Impl& ); ///< Undefined
};

UpdateManager::UpdateManager( NotificationManager& notificationManager,
                              CompleteNotificationInterface& animationFinishedNotifier,
                              PropertyNotifier& propertyNotifier,
                              DiscardQueue& discardQueue,
                              RenderController& controller,
                              RenderManager& renderManager,
                              RenderQueue& renderQueue,
                              RenderTaskProcessor& renderTaskProcessor )
  : mImpl(NULL)
{
  mImpl = new Impl( notificationManager,
                    animationFinishedNotifier,
                    propertyNotifier,
                    discardQueue,
                    controller,
                    renderManager,
                    renderQueue,
                    mSceneGraphBuffers,
                    renderTaskProcessor );

}

UpdateManager::~UpdateManager()
{
  delete mImpl;
}

void UpdateManager::InstallRoot( OwnerPointer<Layer>& layer, bool systemLevel )
{
  DALI_ASSERT_DEBUG( layer->IsLayer() );
  DALI_ASSERT_DEBUG( layer->GetParent() == NULL);

  if ( !systemLevel )
  {
    DALI_ASSERT_DEBUG( mImpl->root == NULL && "Root Node already installed" );
    mImpl->root = layer.Release();
    mImpl->root->CreateTransform( &mImpl->transformManager );
    mImpl->root->SetRoot(true);
  }
  else
  {
    DALI_ASSERT_DEBUG( mImpl->systemLevelRoot == NULL && "System-level Root Node already installed" );
    mImpl->systemLevelRoot = layer.Release();
    mImpl->systemLevelRoot->CreateTransform( &mImpl->transformManager );
    mImpl->systemLevelRoot->SetRoot(true);
  }

}

void UpdateManager::AddNode( OwnerPointer<Node>& node )
{
  DALI_ASSERT_ALWAYS( NULL == node->GetParent() ); // Should not have a parent yet

  // Nodes must be sorted by pointer
  Node* rawNode = node.Release();
  Vector<Node*>::Iterator begin = mImpl->nodes.Begin();
  for( Vector<Node*>::Iterator iter = mImpl->nodes.End()-1; iter >= begin; --iter )
  {
    if( rawNode > (*iter) )
    {
      mImpl->nodes.Insert((iter+1), rawNode );
      rawNode->CreateTransform( &mImpl->transformManager );
      return;
    }
  }
}

void UpdateManager::ConnectNode( Node* parent, Node* node )
{
  DALI_ASSERT_ALWAYS( NULL != parent );
  DALI_ASSERT_ALWAYS( NULL != node );
  DALI_ASSERT_ALWAYS( NULL == node->GetParent() ); // Should not have a parent yet

  parent->ConnectChild( node );
}

void UpdateManager::DisconnectNode( Node* node )
{
  Node* parent = node->GetParent();
  DALI_ASSERT_ALWAYS( NULL != parent );
  parent->SetDirtyFlag( ChildDeletedFlag ); // make parent dirty so that render items dont get reused

  parent->DisconnectChild( mSceneGraphBuffers.GetUpdateBufferIndex(), *node );
}

void UpdateManager::DestroyNode( Node* node )
{
  DALI_ASSERT_ALWAYS( NULL != node );
  DALI_ASSERT_ALWAYS( NULL == node->GetParent() ); // Should have been disconnected

  Vector<Node*>::Iterator iter = mImpl->nodes.Begin()+1;
  Vector<Node*>::Iterator endIter = mImpl->nodes.End();
  for(;iter!=endIter;++iter)
  {
    if((*iter) == node)
    {
      mImpl->nodes.Erase(iter);
      break;
    }
  }

  mImpl->discardQueue.Add( mSceneGraphBuffers.GetUpdateBufferIndex(), node );

  // Notify the Node about impending destruction
  node->OnDestroy();
}

void UpdateManager::AddCamera( OwnerPointer< Camera >& camera )
{
  mImpl->cameras.PushBack( camera.Release() ); // takes ownership
}

void UpdateManager::RemoveCamera( const Camera* camera )
{
  // Find the camera and destroy it
  EraseUsingDiscardQueue( mImpl->cameras, const_cast<Camera*>( camera ), mImpl->discardQueue, mSceneGraphBuffers.GetUpdateBufferIndex() );
}

void UpdateManager::AddObject( OwnerPointer<PropertyOwner>& object )
{
  mImpl->customObjects.PushBack( object.Release() );
}

void UpdateManager::RemoveObject( PropertyOwner* object )
{
  mImpl->customObjects.EraseObject( object );
}

void UpdateManager::AddAnimation( OwnerPointer< SceneGraph::Animation >& animation )
{
  mImpl->animations.PushBack( animation.Release() );
}

void UpdateManager::StopAnimation( Animation* animation )
{
  DALI_ASSERT_DEBUG( animation && "NULL animation called to stop" );

  bool animationFinished = animation->Stop( mSceneGraphBuffers.GetUpdateBufferIndex() );

  mImpl->animationFinishedDuringUpdate = mImpl->animationFinishedDuringUpdate || animationFinished;
}

void UpdateManager::RemoveAnimation( Animation* animation )
{
  DALI_ASSERT_DEBUG( animation && "NULL animation called to remove" );

  animation->OnDestroy( mSceneGraphBuffers.GetUpdateBufferIndex() );

  DALI_ASSERT_DEBUG( animation->GetState() == Animation::Destroyed );
}

bool UpdateManager::IsAnimationRunning() const
{
  // Find any animation that isn't stopped or paused
  for ( auto&& iter : mImpl->animations )
  {
    const Animation::State state = iter->GetState();

    if (state != Animation::Stopped &&
        state != Animation::Paused)
    {
      return true; // stop iteration as soon as first one is found
    }
  }

  return false;
}

void UpdateManager::AddPropertyNotification( OwnerPointer< PropertyNotification >& propertyNotification )
{
  mImpl->propertyNotifications.PushBack( propertyNotification.Release() );
}

void UpdateManager::RemovePropertyNotification( PropertyNotification* propertyNotification )
{
  mImpl->propertyNotifications.EraseObject( propertyNotification );
}

void UpdateManager::PropertyNotificationSetNotify( PropertyNotification* propertyNotification, PropertyNotification::NotifyMode notifyMode )
{
  DALI_ASSERT_DEBUG( propertyNotification && "propertyNotification scene graph object missing" );
  propertyNotification->SetNotifyMode( notifyMode );
}

void UpdateManager::AddShader( OwnerPointer< Shader >& shader )
{
  mImpl->shaders.PushBack( shader.Release() );
}

void UpdateManager::RemoveShader( Shader* shader )
{
  // Find the shader and destroy it
  EraseUsingDiscardQueue( mImpl->shaders, shader, mImpl->discardQueue, mSceneGraphBuffers.GetUpdateBufferIndex() );
}

void UpdateManager::SetShaderProgram( Shader* shader,
                                      Internal::ShaderDataPtr shaderData, bool modifiesGeometry )
{
  if( shaderData )
  {

    typedef MessageValue3< Shader, Internal::ShaderDataPtr, ProgramCache*, bool> DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( shader, &Shader::SetProgram, shaderData, mImpl->renderManager.GetProgramCache(), modifiesGeometry );
  }
}

void UpdateManager::SaveBinary( Internal::ShaderDataPtr shaderData )
{
  DALI_ASSERT_DEBUG( shaderData && "No NULL shader data pointers please." );
  DALI_ASSERT_DEBUG( shaderData->GetBufferSize() > 0 && "Shader binary empty so nothing to save." );
  {
    // lock as update might be sending previously compiled shaders to event thread
    Mutex::ScopedLock lock( mImpl->compiledShaderMutex );
    mImpl->renderCompiledShaders.push_back( shaderData );
  }
}

void UpdateManager::SetShaderSaver( ShaderSaver& upstream )
{
  mImpl->shaderSaver = &upstream;
}

void UpdateManager::AddRenderer( OwnerPointer< Renderer >& renderer )
{
  renderer->ConnectToSceneGraph( *mImpl->sceneController, mSceneGraphBuffers.GetUpdateBufferIndex() );
  mImpl->renderers.PushBack( renderer.Release() );
  mImpl->renderersAdded = true;
}

void UpdateManager::RemoveRenderer( Renderer* renderer )
{
  // Find the renderer and destroy it
  EraseUsingDiscardQueue( mImpl->renderers, renderer, mImpl->discardQueue, mSceneGraphBuffers.GetUpdateBufferIndex() );
  // Need to remove the render object as well
  renderer->DisconnectFromSceneGraph( *mImpl->sceneController, mSceneGraphBuffers.GetUpdateBufferIndex() );
}

void UpdateManager::SetPanGestureProcessor( PanGesture* panGestureProcessor )
{
  DALI_ASSERT_DEBUG( NULL != panGestureProcessor );

  mImpl->panGestureProcessor = panGestureProcessor;
}

void UpdateManager::AddTextureSet( OwnerPointer< TextureSet >& textureSet )
{
  mImpl->textureSets.PushBack( textureSet.Release() );
}

void UpdateManager::RemoveTextureSet( TextureSet* textureSet )
{
  mImpl->textureSets.EraseObject( textureSet );
}

RenderTaskList* UpdateManager::GetRenderTaskList( bool systemLevel )
{
  if ( !systemLevel )
  {
    // copy the list, this is only likely to happen once in application life cycle
    return &(mImpl->taskList);
  }
  else
  {
    // copy the list, this is only likely to happen once in application life cycle
    return &(mImpl->systemLevelTaskList);
  }
}

unsigned int* UpdateManager::ReserveMessageSlot( std::size_t size, bool updateScene )
{
  return mImpl->messageQueue.ReserveMessageSlot( size, updateScene );
}

void UpdateManager::EventProcessingStarted()
{
  mImpl->messageQueue.EventProcessingStarted();
}

bool UpdateManager::FlushQueue()
{
  return mImpl->messageQueue.FlushQueue();
}

void UpdateManager::ResetProperties( BufferIndex bufferIndex )
{
  // Clear the "animations finished" flag; This should be set if any (previously playing) animation is stopped
  mImpl->animationFinishedDuringUpdate = false;

  // Animated properties have to be reset to their original value each frame

  // Reset root properties
  if ( mImpl->root )
  {
    mImpl->root->ResetToBaseValues( bufferIndex );
  }
  if ( mImpl->systemLevelRoot )
  {
    mImpl->systemLevelRoot->ResetToBaseValues( bufferIndex );
  }

  // Reset all the nodes
  Vector<Node*>::Iterator iter = mImpl->nodes.Begin()+1;
  Vector<Node*>::Iterator endIter = mImpl->nodes.End();
  for( ;iter != endIter; ++iter )
  {
    (*iter)->ResetToBaseValues( bufferIndex );
  }

  // Reset system-level render-task list properties to base values
  ResetToBaseValues( mImpl->systemLevelTaskList.GetTasks(), bufferIndex );

  // Reset render-task list properties to base values.
  ResetToBaseValues( mImpl->taskList.GetTasks(), bufferIndex );

  // Reset custom object properties to base values
  ResetToBaseValues( mImpl->customObjects, bufferIndex );

  // Reset animatable renderer properties to base values
  ResetToBaseValues( mImpl->renderers, bufferIndex );

  // Reset animatable shader properties to base values
  ResetToBaseValues( mImpl->shaders, bufferIndex );
}

bool UpdateManager::ProcessGestures( BufferIndex bufferIndex, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds )
{
  bool gestureUpdated( false );

  if( mImpl->panGestureProcessor )
  {
    // gesture processor only supports default properties
    mImpl->panGestureProcessor->ResetDefaultProperties( bufferIndex ); // Needs to be done every time as gesture data is written directly to an update-buffer rather than via a message
    gestureUpdated |= mImpl->panGestureProcessor->UpdateProperties( lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds );
  }

  return gestureUpdated;
}

void UpdateManager::Animate( BufferIndex bufferIndex, float elapsedSeconds )
{
  AnimationContainer &animations = mImpl->animations;
  AnimationIter iter = animations.Begin();
  bool animationLooped = false;

  while ( iter != animations.End() )
  {
    Animation* animation = *iter;
    bool finished = false;
    bool looped = false;
    bool progressMarkerReached = false;
    animation->Update( bufferIndex, elapsedSeconds, looped, finished, progressMarkerReached );

    if ( progressMarkerReached )
    {
      mImpl->notificationManager.QueueMessage( Internal::NotifyProgressReachedMessage( mImpl->animationPlaylist, animation ) );
    }

    mImpl->animationFinishedDuringUpdate = mImpl->animationFinishedDuringUpdate || finished;
    animationLooped = animationLooped || looped;

    // Remove animations that had been destroyed but were still waiting for an update
    if (animation->GetState() == Animation::Destroyed)
    {
      iter = animations.Erase(iter);
    }
    else
    {
      ++iter;
    }
  }

  // queue the notification on finished or looped (to update loop count)
  if ( mImpl->animationFinishedDuringUpdate || animationLooped )
  {
    // The application should be notified by NotificationManager, in another thread
    mImpl->notificationManager.QueueCompleteNotification( &mImpl->animationPlaylist );
  }
}

void UpdateManager::ConstrainCustomObjects( BufferIndex bufferIndex )
{
  //Constrain custom objects (in construction order)
  for ( auto&& object : mImpl->customObjects )
  {
    ConstrainPropertyOwner( *object, bufferIndex );
  }
}

void UpdateManager::ConstrainRenderTasks( BufferIndex bufferIndex )
{
  // Constrain system-level render-tasks
  const RenderTaskList::RenderTaskContainer& systemLevelTasks = mImpl->systemLevelTaskList.GetTasks();
  for ( auto&& task : systemLevelTasks )
  {
    ConstrainPropertyOwner( *task, bufferIndex );
  }

  // Constrain render-tasks
  const RenderTaskList::RenderTaskContainer& tasks = mImpl->taskList.GetTasks();
  for ( auto&& task : tasks )
  {
    ConstrainPropertyOwner( *task, bufferIndex );
  }
}

void UpdateManager::ConstrainShaders( BufferIndex bufferIndex )
{
  // constrain shaders... (in construction order)
  for ( auto&& shader : mImpl->shaders )
  {
    ConstrainPropertyOwner( *shader, bufferIndex );
  }
}

void UpdateManager::ProcessPropertyNotifications( BufferIndex bufferIndex )
{
  for( auto&& notification : mImpl->propertyNotifications )
  {
    bool valid = notification->Check( bufferIndex );
    if(valid)
    {
      mImpl->notificationManager.QueueMessage( PropertyChangedMessage( mImpl->propertyNotifier, notification, notification->GetValidity() ) );
    }
  }
}

void UpdateManager::ForwardCompiledShadersToEventThread()
{
  DALI_ASSERT_DEBUG( (mImpl->shaderSaver != 0) && "shaderSaver should be wired-up during startup." );
  if( mImpl->shaderSaver )
  {
    // lock and swap the queues
    {
      // render might be attempting to send us more binaries at the same time
      Mutex::ScopedLock lock( mImpl->compiledShaderMutex );
      mImpl->renderCompiledShaders.swap( mImpl->updateCompiledShaders );
    }

    if( mImpl->updateCompiledShaders.size() > 0 )
    {
      ShaderSaver& factory = *mImpl->shaderSaver;
      for( auto&& shader : mImpl->updateCompiledShaders )
      {
        mImpl->notificationManager.QueueMessage( ShaderCompiledMessage( factory, shader ) );
      }
      // we don't need them in update anymore
      mImpl->updateCompiledShaders.clear();
    }
  }
}

void UpdateManager::UpdateRenderers( BufferIndex bufferIndex )
{
  const unsigned int rendererCount = mImpl->renderers.Count();
  for( unsigned int i = 0; i < rendererCount; ++i )
  {
    //Apply constraints
    ConstrainPropertyOwner( *mImpl->renderers[i], bufferIndex );

    mImpl->renderers[i]->PrepareRender( bufferIndex );
  }
}

void UpdateManager::UpdateNodes( BufferIndex bufferIndex )
{
  mImpl->nodeDirtyFlags = NothingFlag;

  if ( !mImpl->root )
  {
    return;
  }

  // Prepare resources, update shaders, for each node
  // And add the renderers to the sorted layers. Start from root, which is also a layer
  mImpl->nodeDirtyFlags = UpdateNodeTree( *( mImpl->root ),
                                          bufferIndex,
                                          mImpl->renderQueue );

  if ( mImpl->systemLevelRoot )
  {
    mImpl->nodeDirtyFlags |= UpdateNodeTree( *( mImpl->systemLevelRoot ),
                                             bufferIndex,
                                             mImpl->renderQueue );
  }
}

unsigned int UpdateManager::Update( float elapsedSeconds,
                                    unsigned int lastVSyncTimeMilliseconds,
                                    unsigned int nextVSyncTimeMilliseconds )
{
  const BufferIndex bufferIndex = mSceneGraphBuffers.GetUpdateBufferIndex();

  //Clear nodes/resources which were previously discarded
  mImpl->discardQueue.Clear( bufferIndex );

  //Process Touches & Gestures
  const bool gestureUpdated = ProcessGestures( bufferIndex, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds );

  bool updateScene = // The scene-graph requires an update if..
      (mImpl->nodeDirtyFlags & RenderableUpdateFlags) ||    // ..nodes were dirty in previous frame OR
      IsAnimationRunning()                            ||    // ..at least one animation is running OR
      mImpl->messageQueue.IsSceneUpdateRequired()     ||    // ..a message that modifies the scene graph node tree is queued OR
      gestureUpdated;                                       // ..a gesture property was updated


  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // values if the scene was updated in the previous frame.
  if( updateScene || mImpl->previousUpdateScene )
  {
    //Reset properties from the previous update
    ResetProperties( bufferIndex );
    mImpl->transformManager.ResetToBaseValue();
  }

  // Process the queued scene messages. Note, MessageQueue::FlushQueue may be called
  // between calling IsSceneUpdateRequired() above and here, so updateScene should
  // be set again
  updateScene |= mImpl->messageQueue.ProcessMessages( bufferIndex );

  //Forward compiled shader programs to event thread for saving
  ForwardCompiledShadersToEventThread();

  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // renderer lists if the scene was updated in the previous frame.
  // We should not start skipping update steps or reusing lists until there has been two frames where nothing changes
  if( updateScene || mImpl->previousUpdateScene )
  {
    //Animate
    Animate( bufferIndex, elapsedSeconds );

    //Constraint custom objects
    ConstrainCustomObjects( bufferIndex );

    //Clear the lists of renderers from the previous update
    for( size_t i(0); i<mImpl->sortedLayers.size(); ++i )
    {
      mImpl->sortedLayers[i]->ClearRenderables();
    }

    for( size_t i(0); i<mImpl->systemLevelSortedLayers.size(); ++i )
    {
      mImpl->systemLevelSortedLayers[i]->ClearRenderables();
    }

    //Update node hierarchy, apply constraints and perform sorting / culling.
    //This will populate each Layer with a list of renderers which are ready.
    UpdateNodes( bufferIndex );

    //Apply constraints to RenderTasks, shaders
    ConstrainRenderTasks( bufferIndex );
    ConstrainShaders( bufferIndex );

    //Update renderers and apply constraints
    UpdateRenderers( bufferIndex );

    //Update the trnasformations of all the nodes
    mImpl->transformManager.Update();

    //Process Property Notifications
    ProcessPropertyNotifications( bufferIndex );

    //Update cameras
    for( auto&& cameraIterator : mImpl->cameras )
    {
      cameraIterator->Update( bufferIndex );
    }

    //Process the RenderTasks if renderers exist. This creates the instructions for rendering the next frame.
    //reset the update buffer index and make sure there is enough room in the instruction container
    if( mImpl->renderersAdded )
    {
      mImpl->renderInstructions.ResetAndReserve( bufferIndex,
                                                 mImpl->taskList.GetTasks().Count() + mImpl->systemLevelTaskList.GetTasks().Count() );

      if ( NULL != mImpl->root )
      {
        mImpl->renderTaskProcessor.Process( bufferIndex,
                                          mImpl->taskList,
                                          *mImpl->root,
                                          mImpl->sortedLayers,
                                          mImpl->renderInstructions );

        // Process the system-level RenderTasks last
        if ( NULL != mImpl->systemLevelRoot )
        {
          mImpl->renderTaskProcessor.Process( bufferIndex,
                                            mImpl->systemLevelTaskList,
                                            *mImpl->systemLevelRoot,
                                            mImpl->systemLevelSortedLayers,
                                            mImpl->renderInstructions );
        }
      }
    }
  }

  // check the countdown and notify (note, at the moment this is only done for normal tasks, not for systemlevel tasks)
  bool doRenderOnceNotify = false;
  mImpl->renderTaskWaiting = false;
  for ( auto&& renderTask : mImpl->taskList.GetTasks() )
  {
    renderTask->UpdateState();

    if( renderTask->IsWaitingToRender() &&
        renderTask->ReadyToRender( bufferIndex ) /*avoid updating forever when source actor is off-stage*/ )
    {
      mImpl->renderTaskWaiting = true; // keep update/render threads alive
    }

    if( renderTask->HasRendered() )
    {
      doRenderOnceNotify = true;
    }
  }

  if( doRenderOnceNotify )
  {
    DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "Notify a render task has finished\n");
    mImpl->notificationManager.QueueCompleteNotification( mImpl->taskList.GetCompleteNotificationInterface() );
  }

  // Macro is undefined in release build.
  SNAPSHOT_NODE_LOGGING;

  // A ResetProperties() may be required in the next frame
  mImpl->previousUpdateScene = updateScene;

  // Check whether further updates are required
  unsigned int keepUpdating = KeepUpdatingCheck( elapsedSeconds );

  // tell the update manager that we're done so the queue can be given to event thread
  mImpl->notificationManager.UpdateCompleted();

  // The update has finished; swap the double-buffering indices
  mSceneGraphBuffers.Swap();

  return keepUpdating;
}

unsigned int UpdateManager::KeepUpdatingCheck( float elapsedSeconds ) const
{
  // Update the duration set via Stage::KeepRendering()
  if ( mImpl->keepRenderingSeconds > 0.0f )
  {
    mImpl->keepRenderingSeconds -= elapsedSeconds;
  }

  unsigned int keepUpdatingRequest = KeepUpdating::NOT_REQUESTED;

  // If Stage::KeepRendering() has been called, then continue until the duration has elapsed.
  // Keep updating until no messages are received and no animations are running.
  // If an animation has just finished, update at least once more for Discard end-actions.
  // No need to check for renderQueue as there is always a render after update and if that
  // render needs another update it will tell the adaptor to call update again

  if ( mImpl->keepRenderingSeconds > 0.0f )
  {
    keepUpdatingRequest |= KeepUpdating::STAGE_KEEP_RENDERING;
  }

  if ( IsAnimationRunning() ||
       mImpl->animationFinishedDuringUpdate )
  {
    keepUpdatingRequest |= KeepUpdating::ANIMATIONS_RUNNING;
  }

  if ( mImpl->renderTaskWaiting )
  {
    keepUpdatingRequest |= KeepUpdating::RENDER_TASK_SYNC;
  }

  return keepUpdatingRequest;
}

void UpdateManager::SetBackgroundColor( const Vector4& color )
{
  typedef MessageValue1< RenderManager, Vector4 > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager, &RenderManager::SetBackgroundColor, color );
}

void UpdateManager::SetDefaultSurfaceRect( const Rect<int>& rect )
{
  typedef MessageValue1< RenderManager, Rect<int> > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetDefaultSurfaceRect, rect );
}

void UpdateManager::KeepRendering( float durationSeconds )
{
  mImpl->keepRenderingSeconds = std::max( mImpl->keepRenderingSeconds, durationSeconds );
}

void UpdateManager::SetLayerDepths( const SortedLayerPointers& layers, bool systemLevel )
{
  if ( !systemLevel )
  {
    // just copy the vector of pointers
    mImpl->sortedLayers = layers;
  }
  else
  {
    mImpl->systemLevelSortedLayers = layers;
  }
}

void UpdateManager::SetDepthIndices( OwnerPointer< NodeDepths >& nodeDepths )
{
  // note,this vector is already in depth order. It could be used as-is to
  // remove sorting in update algorithm. However, it lacks layer boundary markers.
  for( auto&& iter : nodeDepths->nodeDepths )
  {
    iter.node->SetDepthIndex( iter.sortedDepth );
  }

  // Go through node hierarchy and rearrange siblings according to depth-index
  SortSiblingNodesRecursively( *( mImpl->root ) );
}

void UpdateManager::AddSampler( OwnerPointer< Render::Sampler >& sampler )
{
  // Message has ownership of Sampler while in transit from update to render
  typedef MessageValue1< RenderManager, OwnerPointer< Render::Sampler > > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::AddSampler, sampler );
}

void UpdateManager::RemoveSampler( Render::Sampler* sampler )
{
  typedef MessageValue1< RenderManager, Render::Sampler* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::RemoveSampler, sampler );
}

void UpdateManager::SetFilterMode( Render::Sampler* sampler, unsigned int minFilterMode, unsigned int magFilterMode )
{
  typedef MessageValue3< RenderManager, Render::Sampler*, unsigned int, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetFilterMode, sampler, minFilterMode, magFilterMode );
}

void UpdateManager::SetWrapMode( Render::Sampler* sampler, unsigned int rWrapMode, unsigned int sWrapMode, unsigned int tWrapMode )
{
  typedef MessageValue4< RenderManager, Render::Sampler*, unsigned int, unsigned int, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetWrapMode, sampler, rWrapMode, sWrapMode, tWrapMode );
}

void UpdateManager::AddPropertyBuffer( OwnerPointer< Render::PropertyBuffer >& propertyBuffer )
{
  // Message has ownership of format while in transit from update -> render
  typedef MessageValue1< RenderManager, OwnerPointer< Render::PropertyBuffer > > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::AddPropertyBuffer, propertyBuffer );
}

void UpdateManager::RemovePropertyBuffer( Render::PropertyBuffer* propertyBuffer )
{
  typedef MessageValue1< RenderManager, Render::PropertyBuffer* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::RemovePropertyBuffer, propertyBuffer );
}

void UpdateManager::SetPropertyBufferFormat( Render::PropertyBuffer* propertyBuffer, OwnerPointer< Render::PropertyBuffer::Format>& format )
{
  // Message has ownership of format while in transit from update -> render
  typedef MessageValue2< RenderManager, Render::PropertyBuffer*, OwnerPointer< Render::PropertyBuffer::Format > > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetPropertyBufferFormat, propertyBuffer, format );
}

void UpdateManager::SetPropertyBufferData( Render::PropertyBuffer* propertyBuffer, OwnerPointer< Vector<char> >& data, size_t size )
{
  // Message has ownership of format while in transit from update -> render
  typedef MessageValue3< RenderManager, Render::PropertyBuffer*, OwnerPointer< Dali::Vector<char> >, size_t > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager, &RenderManager::SetPropertyBufferData, propertyBuffer, data, size );
}

void UpdateManager::AddGeometry( OwnerPointer< Render::Geometry >& geometry )
{
  // Message has ownership of format while in transit from update -> render
  typedef MessageValue1< RenderManager, OwnerPointer< Render::Geometry > > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::AddGeometry, geometry );
}

void UpdateManager::RemoveGeometry( Render::Geometry* geometry )
{
  typedef MessageValue1< RenderManager, Render::Geometry* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::RemoveGeometry, geometry );
}

void UpdateManager::SetGeometryType( Render::Geometry* geometry, unsigned int geometryType )
{
  typedef MessageValue2< RenderManager, Render::Geometry*, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetGeometryType, geometry, geometryType );
}

void UpdateManager::SetIndexBuffer( Render::Geometry* geometry, Dali::Vector<unsigned short>& indices )
{
  typedef IndexBufferMessage< RenderManager > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager, geometry, indices );
}

void UpdateManager::RemoveVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer )
{
  typedef MessageValue2< RenderManager, Render::Geometry*, Render::PropertyBuffer* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::RemoveVertexBuffer, geometry, propertyBuffer );
}

void UpdateManager::AttachVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer )
{
  typedef MessageValue2< RenderManager, Render::Geometry*, Render::PropertyBuffer* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::AttachVertexBuffer, geometry, propertyBuffer );
}

void UpdateManager::AddTexture( OwnerPointer< Render::Texture >& texture )
{
  // Message has ownership of Texture while in transit from update -> render
  typedef MessageValue1< RenderManager, OwnerPointer< Render::Texture > > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager, &RenderManager::AddTexture, texture );
}

void UpdateManager::RemoveTexture( Render::Texture* texture)
{
  typedef MessageValue1< RenderManager, Render::Texture* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::RemoveTexture, texture );
}

void UpdateManager::UploadTexture( Render::Texture* texture, PixelDataPtr pixelData, const Texture::UploadParams& params )
{
  typedef MessageValue3< RenderManager, Render::Texture*, PixelDataPtr, Texture::UploadParams > DerivedType;

  // Reserve some memory inside the message queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager, &RenderManager::UploadTexture, texture, pixelData, params );
}

void UpdateManager::GenerateMipmaps( Render::Texture* texture )
{
  typedef MessageValue1< RenderManager, Render::Texture* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::GenerateMipmaps, texture );
}

void UpdateManager::AddFrameBuffer( Render::FrameBuffer* frameBuffer )
{
  typedef MessageValue1< RenderManager, Render::FrameBuffer* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::AddFrameBuffer, frameBuffer );
}

void UpdateManager::RemoveFrameBuffer( Render::FrameBuffer* frameBuffer)
{
  typedef MessageValue1< RenderManager, Render::FrameBuffer* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::RemoveFrameBuffer, frameBuffer );
}

void UpdateManager::AttachColorTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, unsigned int mipmapLevel, unsigned int layer )
{
  typedef MessageValue4< RenderManager, Render::FrameBuffer*, Render::Texture*, unsigned int, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::AttachColorTextureToFrameBuffer, frameBuffer, texture, mipmapLevel, layer );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
