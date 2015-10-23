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
 *
 */

// CLASS HEADER
#include <dali/internal/update/manager/update-manager.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/common/set-wrapper.h>
#include <dali/devel-api/common/mutex.h>
#include <dali/devel-api/common/owner-container.h>

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

#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller-impl.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/manager/object-owner-container.h>
#include <dali/internal/update/manager/prepare-render-algorithms.h>
#include <dali/internal/update/manager/process-render-tasks.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/manager/update-algorithms.h>
#include <dali/internal/update/manager/update-manager-debug.h>
#include <dali/internal/update/node-attachments/scene-graph-camera-attachment.h>
#include <dali/internal/update/node-attachments/scene-graph-image-attachment.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/queue/update-message-queue.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/rendering/scene-graph-material.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/touch/touch-resampler.h>

#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-sampler.h>

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

void DestroyNodeSet( std::set<Node*>& nodeSet )
{
  for( std::set<Node*>::iterator iter = nodeSet.begin(); iter != nodeSet.end(); ++iter )
  {
    Node* node( *iter );

    // Call Node::OnDestroy as each node is destroyed
    node->OnDestroy();

    delete node;
  }
  nodeSet.clear();
}

} //namespace

typedef OwnerContainer< Shader* >              ShaderContainer;
typedef ShaderContainer::Iterator              ShaderIter;
typedef ShaderContainer::ConstIterator         ShaderConstIter;

typedef std::vector<Internal::ShaderDataPtr>   ShaderDataBinaryQueue;

typedef OwnerContainer<PanGesture*>            GestureContainer;
typedef GestureContainer::Iterator             GestureIter;
typedef GestureContainer::ConstIterator        GestureConstIter;


/**
 * Structure to contain UpdateManager internal data
 */
struct UpdateManager::Impl
{
  Impl( NotificationManager& notificationManager,
        GlSyncAbstraction& glSyncAbstraction,
        CompleteNotificationInterface& animationFinishedNotifier,
        PropertyNotifier& propertyNotifier,
        ResourceManager& resourceManager,
        DiscardQueue& discardQueue,
        RenderController& renderController,
        RenderManager& renderManager,
        RenderQueue& renderQueue,
        TextureCache& textureCache,
        TouchResampler& touchResampler,
        SceneGraphBuffers& sceneGraphBuffers )
  :
    renderMessageDispatcher( renderManager, renderQueue, sceneGraphBuffers ),
    notificationManager( notificationManager ),
    animationFinishedNotifier( animationFinishedNotifier ),
    propertyNotifier( propertyNotifier ),
    shaderSaver( NULL ),
    resourceManager( resourceManager ),
    discardQueue( discardQueue ),
    renderController( renderController ),
    sceneController( NULL ),
    renderManager( renderManager ),
    renderQueue( renderQueue ),
    renderInstructions( renderManager.GetRenderInstructionContainer() ),
    completeStatusManager( glSyncAbstraction, renderMessageDispatcher, resourceManager ),
    touchResampler( touchResampler ),
    backgroundColor( Dali::Stage::DEFAULT_BACKGROUND_COLOR ),
    taskList ( completeStatusManager ),
    systemLevelTaskList ( completeStatusManager ),
    root( NULL ),
    systemLevelRoot( NULL ),
    renderers( sceneGraphBuffers, discardQueue ),
    geometries( sceneGraphBuffers, discardQueue ),
    materials( sceneGraphBuffers, discardQueue ),
    messageQueue( renderController, sceneGraphBuffers ),
    keepRenderingSeconds( 0.0f ),
    animationFinishedDuringUpdate( false ),
    nodeDirtyFlags( TransformFlag ), // set to TransformFlag to ensure full update the first time through Update()
    previousUpdateScene( false ),
    frameCounter( 0 ),
    renderSortingHelper(),
    renderTaskWaiting( false )
  {
    sceneController = new SceneControllerImpl( renderMessageDispatcher, renderQueue, discardQueue, textureCache, completeStatusManager );

    renderers.SetSceneController( *sceneController );
    geometries.SetSceneController( *sceneController );
    materials.SetSceneController( *sceneController );
  }

  ~Impl()
  {
    // Disconnect render tasks from nodes, before destroying the nodes
    RenderTaskList::RenderTaskContainer& tasks = taskList.GetTasks();
    for (RenderTaskList::RenderTaskContainer::Iterator iter = tasks.Begin(); iter != tasks.End(); ++iter)
    {
      (*iter)->SetSourceNode( NULL );
    }
    // ..repeat for system level RenderTasks
    RenderTaskList::RenderTaskContainer& systemLevelTasks = systemLevelTaskList.GetTasks();
    for (RenderTaskList::RenderTaskContainer::Iterator iter = systemLevelTasks.Begin(); iter != systemLevelTasks.End(); ++iter)
    {
      (*iter)->SetSourceNode( NULL );
    }

    // UpdateManager owns the Nodes
    DestroyNodeSet( activeDisconnectedNodes );
    DestroyNodeSet( connectedNodes );
    DestroyNodeSet( disconnectedNodes );

    // If there is root, reset it, otherwise do nothing as rendering was never started
    if( root )
    {
      root->OnDestroy();

      delete root;
      root = NULL;
    }

    if( systemLevelRoot )
    {
      systemLevelRoot->OnDestroy();

      delete systemLevelRoot;
      systemLevelRoot = NULL;
    }

    sceneController->GetTextureCache().SetBufferIndices(NULL); // TODO - Remove
    delete sceneController;
  }

  SceneGraphBuffers                   sceneGraphBuffers;             ///< Used to keep track of which buffers are being written or read
  RenderMessageDispatcher             renderMessageDispatcher;       ///< Used for passing messages to the render-thread
  NotificationManager&                notificationManager;           ///< Queues notification messages for the event-thread.
  CompleteNotificationInterface&      animationFinishedNotifier;     ///< Provides notification to applications when animations are finished.
  PropertyNotifier&                   propertyNotifier;              ///< Provides notification to applications when properties are modified.
  ShaderSaver*                        shaderSaver;                   ///< Saves shader binaries.
  ResourceManager&                    resourceManager;               ///< resource manager
  DiscardQueue&                       discardQueue;                  ///< Nodes are added here when disconnected from the scene-graph.
  RenderController&                   renderController;              ///< render controller
  SceneControllerImpl*                sceneController;               ///< scene controller
  RenderManager&                      renderManager;                 ///< This is responsible for rendering the results of each "update"
  RenderQueue&                        renderQueue;                   ///< Used to queue messages for the next render
  RenderInstructionContainer&         renderInstructions;            ///< Used to prepare the render instructions
  CompleteStatusManager               completeStatusManager;         ///< Complete Status Manager
  TouchResampler&                     touchResampler;                ///< Used to resample touch events on every update.

  Vector4                             backgroundColor;               ///< The glClear color used at the beginning of each frame.

  RenderTaskList                      taskList;                      ///< The list of scene graph render-tasks
  RenderTaskList                      systemLevelTaskList;           ///< Separate render-tasks for system-level content

  Layer*                              root;                          ///< The root node (root is a layer)
  Layer*                              systemLevelRoot;               ///< A separate root-node for system-level content
  std::set< Node* >                   activeDisconnectedNodes;       ///< A container of new or modified nodes (without parent) owned by UpdateManager
  std::set< Node* >                   connectedNodes;                ///< A container of connected (with parent) nodes owned by UpdateManager
  std::set< Node* >                   disconnectedNodes;             ///< A container of inactive disconnected nodes (without parent) owned by UpdateManager

  SortedLayerPointers                 sortedLayers;                  ///< A container of Layer pointers sorted by depth
  SortedLayerPointers                 systemLevelSortedLayers;       ///< A separate container of system-level Layers

  OwnerContainer< PropertyOwner* >    customObjects;                 ///< A container of owned objects (with custom properties)

  AnimationContainer                  animations;                    ///< A container of owned animations
  PropertyNotificationContainer       propertyNotifications;         ///< A container of owner property notifications.

  ObjectOwnerContainer<Renderer>      renderers;
  ObjectOwnerContainer<Geometry>      geometries;                    ///< A container of geometries
  ObjectOwnerContainer<Material>      materials;                     ///< A container of materials

  ShaderContainer                     shaders;                       ///< A container of owned shaders

  MessageQueue                        messageQueue;                  ///< The messages queued from the event-thread
  ShaderDataBinaryQueue               renderCompiledShaders;         ///< Shaders compiled on Render thread are inserted here for update thread to pass on to event thread.
  ShaderDataBinaryQueue               updateCompiledShaders;         ///< Shaders to be sent from Update to Event
  Mutex                               compiledShaderMutex;           ///< lock to ensure no corruption on the renderCompiledShaders

  float                               keepRenderingSeconds;          ///< Set via Dali::Stage::KeepRendering
  bool                                animationFinishedDuringUpdate; ///< Flag whether any animations finished during the Update()

  int                                 nodeDirtyFlags;                ///< cumulative node dirty flags from previous frame
  bool                                previousUpdateScene;           ///< True if the scene was updated in the previous frame (otherwise it was optimized out)

  int                                 frameCounter;                  ///< Frame counter used in debugging to choose which frame to debug and which to ignore.
  RendererSortingHelper               renderSortingHelper;           ///< helper used to sort transparent renderers

  GestureContainer                    gestures;                      ///< A container of owned gesture detectors
  bool                                renderTaskWaiting;             ///< A REFRESH_ONCE render task is waiting to be rendered
};

UpdateManager::UpdateManager( NotificationManager& notificationManager,
                              GlSyncAbstraction& glSyncAbstraction,
                              CompleteNotificationInterface& animationFinishedNotifier,
                              PropertyNotifier& propertyNotifier,
                              ResourceManager& resourceManager,
                              DiscardQueue& discardQueue,
                              RenderController& controller,
                              RenderManager& renderManager,
                              RenderQueue& renderQueue,
                              TextureCache& textureCache,
                              TouchResampler& touchResampler )
  : mImpl(NULL)
{
  mImpl = new Impl( notificationManager,
                    glSyncAbstraction,
                    animationFinishedNotifier,
                    propertyNotifier,
                    resourceManager,
                    discardQueue,
                    controller,
                    renderManager,
                    renderQueue,
                    textureCache,
                    touchResampler,
                    mSceneGraphBuffers );

  textureCache.SetBufferIndices( &mSceneGraphBuffers );
}

UpdateManager::~UpdateManager()
{
  delete mImpl;
}

void UpdateManager::InstallRoot( SceneGraph::Layer* layer, bool systemLevel )
{
  DALI_ASSERT_DEBUG( layer->IsLayer() );
  DALI_ASSERT_DEBUG( layer->GetParent() == NULL);

  if ( !systemLevel )
  {
    DALI_ASSERT_DEBUG( mImpl->root == NULL && "Root Node already installed" );
    mImpl->root = layer;
  }
  else
  {
    DALI_ASSERT_DEBUG( mImpl->systemLevelRoot == NULL && "System-level Root Node already installed" );
    mImpl->systemLevelRoot = layer;
  }

  layer->SetRoot(true);
}

void UpdateManager::AddNode( Node* node )
{
  DALI_ASSERT_ALWAYS( NULL != node );
  DALI_ASSERT_ALWAYS( NULL == node->GetParent() ); // Should not have a parent yet

  mImpl->activeDisconnectedNodes.insert( node ); // Takes ownership of node
}

void UpdateManager::ConnectNode( Node* parent, Node* node )
{
  DALI_ASSERT_ALWAYS( NULL != parent );
  DALI_ASSERT_ALWAYS( NULL != node );
  DALI_ASSERT_ALWAYS( NULL == node->GetParent() ); // Should not have a parent yet

  // Move from active/disconnectedNodes to connectedNodes
  std::set<Node*>::size_type removed = mImpl->activeDisconnectedNodes.erase( node );
  if( !removed )
  {
    removed = mImpl->disconnectedNodes.erase( node );
    DALI_ASSERT_ALWAYS( removed );
  }
  mImpl->connectedNodes.insert( node );

  node->SetActive( true );

  parent->ConnectChild( node );
}

void UpdateManager::DisconnectNode( Node* node )
{
  Node* parent = node->GetParent();
  DALI_ASSERT_ALWAYS( NULL != parent );
  parent->SetDirtyFlag( ChildDeletedFlag ); // make parent dirty so that render items dont get reused

  // Move from connectedNodes to activeDisconnectedNodes (reset properties next frame)
  parent->DisconnectChild( mSceneGraphBuffers.GetUpdateBufferIndex(), *node, mImpl->connectedNodes, mImpl->activeDisconnectedNodes );
}

void UpdateManager::SetNodeActive( Node* node )
{
  DALI_ASSERT_ALWAYS( NULL != node );
  DALI_ASSERT_ALWAYS( NULL == node->GetParent() ); // Should not have a parent yet

  // Move from disconnectedNodes to activeDisconnectedNodes (reset properties next frame)
  std::set<Node*>::size_type removed = mImpl->disconnectedNodes.erase( node );
  DALI_ASSERT_ALWAYS( removed );
  mImpl->activeDisconnectedNodes.insert( node );

  node->SetActive( true );
}

void UpdateManager::DestroyNode( Node* node )
{
  DALI_ASSERT_ALWAYS( NULL != node );
  DALI_ASSERT_ALWAYS( NULL == node->GetParent() ); // Should have been disconnected

  // Transfer ownership from new/disconnectedNodes to the discard queue
  // This keeps the nodes alive, until the render-thread has finished with them
  std::set<Node*>::size_type removed = mImpl->activeDisconnectedNodes.erase( node );
  if( !removed )
  {
    removed = mImpl->disconnectedNodes.erase( node );
    DALI_ASSERT_ALWAYS( removed );
  }
  mImpl->discardQueue.Add( mSceneGraphBuffers.GetUpdateBufferIndex(), node );

  // Notify the Node about impending destruction
  node->OnDestroy();
}

//@todo MESH_REWORK Extend to allow arbitrary scene objects to connect to each other
void UpdateManager::AttachToNode( Node* node, NodeAttachment* attachment )
{
  DALI_ASSERT_DEBUG( node != NULL );
  DALI_ASSERT_DEBUG( attachment != NULL );

  // attach node to attachment first so that parent is known by the time attachment is connected
  node->Attach( *attachment ); // node takes ownership

  // @todo MESH_REWORK Remove after merge of SceneGraph::RenderableAttachment and SceneGraph::RendererAttachment
  if( dynamic_cast<SceneGraph::ImageAttachment*>( attachment ) != NULL )
  {
    attachment->Initialize( *mImpl->sceneController, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }
}

void UpdateManager::AddObject( PropertyOwner* object )
{
  DALI_ASSERT_DEBUG( NULL != object );

  mImpl->customObjects.PushBack( object );
}

void UpdateManager::RemoveObject( PropertyOwner* object )
{
  DALI_ASSERT_DEBUG( NULL != object );

  OwnerContainer< PropertyOwner* >& customObjects = mImpl->customObjects;

  // Find the object and destroy it
  for ( OwnerContainer< PropertyOwner* >::Iterator iter = customObjects.Begin(); iter != customObjects.End(); ++iter )
  {
    PropertyOwner* current = *iter;
    if ( current == object )
    {
      customObjects.Erase( iter );
      return;
    }
  }

  // Should not reach here
  DALI_ASSERT_DEBUG(false);
}

void UpdateManager::AddAnimation( Animation* animation )
{
  mImpl->animations.PushBack( animation );
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
  bool isRunning(false);
  AnimationContainer& animations = mImpl->animations;

  // Find any animation that isn't stopped or paused

  const AnimationIter endIter = animations.End();
  for ( AnimationIter iter = animations.Begin(); !isRunning && iter != endIter; ++iter )
  {
    const Animation::State state = (*iter)->GetState();

    if (state != Animation::Stopped &&
        state != Animation::Paused)
    {
      isRunning = true;
    }
  }

  return isRunning;
}

void UpdateManager::AddPropertyNotification( PropertyNotification* propertyNotification )
{
  mImpl->propertyNotifications.PushBack( propertyNotification );
}

void UpdateManager::RemovePropertyNotification( PropertyNotification* propertyNotification )
{
  PropertyNotificationContainer &propertyNotifications = mImpl->propertyNotifications;
  PropertyNotificationIter iter = propertyNotifications.Begin();

  while ( iter != propertyNotifications.End() )
  {
    if( *iter == propertyNotification )
    {
      propertyNotifications.Erase(iter);
      break;
    }
    ++iter;
  }
}

void UpdateManager::PropertyNotificationSetNotify( PropertyNotification* propertyNotification, PropertyNotification::NotifyMode notifyMode )
{
  DALI_ASSERT_DEBUG( propertyNotification && "propertyNotification scene graph object missing" );
  propertyNotification->SetNotifyMode( notifyMode );
}

ObjectOwnerContainer<Geometry>& UpdateManager::GetGeometryOwner()
{
  return mImpl->geometries;
}

ObjectOwnerContainer<Renderer>& UpdateManager::GetRendererOwner()
{
  return mImpl->renderers;
}


ObjectOwnerContainer<Material>& UpdateManager::GetMaterialOwner()
{
  return mImpl->materials;
}

void UpdateManager::AddShader( Shader* shader )
{
  DALI_ASSERT_DEBUG( NULL != shader );

  if( mImpl->shaders.Count() == 0 )
  {
    // the first added shader becomes our default shader
    // Construct message in the render queue memory; note that delete should not be called on the return value
    typedef MessageValue1< RenderManager, Shader* > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mImpl->renderManager, &RenderManager::SetDefaultShader, shader );
  }

  mImpl->shaders.PushBack( shader );

  // Allows the shader to dispatch texture requests to the cache
  shader->Initialize( mImpl->renderQueue, mImpl->sceneController->GetTextureCache() );
}

void UpdateManager::RemoveShader( Shader* shader )
{
  DALI_ASSERT_DEBUG(shader != NULL);

  ShaderContainer& shaders = mImpl->shaders;

  // Find the shader and destroy it
  for ( ShaderIter iter = shaders.Begin(); iter != shaders.End(); ++iter )
  {
    Shader& current = **iter;
    if ( &current == shader )
    {
      // Transfer ownership to the discard queue
      // This keeps the shader alive, until the render-thread has finished with it
      mImpl->discardQueue.Add( mSceneGraphBuffers.GetUpdateBufferIndex(), shaders.Release( iter ) );

      return;
    }
  }
  // Should not reach here
  DALI_ASSERT_DEBUG(false);
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

void UpdateManager::AddGesture( PanGesture* gesture )
{
  DALI_ASSERT_DEBUG( NULL != gesture );

  mImpl->gestures.PushBack( gesture );
}

void UpdateManager::RemoveGesture( PanGesture* gesture )
{
  DALI_ASSERT_DEBUG( gesture != NULL );

  GestureContainer& gestures = mImpl->gestures;

  // Find the gesture and destroy it
  for ( GestureIter iter = gestures.Begin(), endIter = gestures.End(); iter != endIter; ++iter )
  {
    PanGesture& current = **iter;
    if ( &current == gesture )
    {
      mImpl->gestures.Erase( iter );
      return;
    }
  }
  // Should not reach here
  DALI_ASSERT_DEBUG(false);
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
  PERF_MONITOR_START(PerformanceMonitor::RESET_PROPERTIES);

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

  // Reset the Connected Nodes
  const std::set<Node*>::iterator endIter = mImpl->connectedNodes.end();
  for( std::set<Node*>::iterator iter = mImpl->connectedNodes.begin(); endIter != iter; ++iter )
  {
    (*iter)->ResetToBaseValues( bufferIndex );
  }

  // If a Node is disconnected, it may still be "active" (requires a reset in next frame)
  for( std::set<Node*>::iterator iter = mImpl->activeDisconnectedNodes.begin(); mImpl->activeDisconnectedNodes.end() != iter; iter = mImpl->activeDisconnectedNodes.begin() )
  {
    Node* node = *iter;
    node->ResetToBaseValues( bufferIndex );
    node->SetActive( false );

    // Move everything from activeDisconnectedNodes to disconnectedNodes (no need to reset again)
    mImpl->activeDisconnectedNodes.erase( iter );
    mImpl->disconnectedNodes.insert( node );
  }

  // Reset system-level render-task list properties to base values
  const RenderTaskList::RenderTaskContainer& systemLevelTasks = mImpl->systemLevelTaskList.GetTasks();

  for (RenderTaskList::RenderTaskContainer::ConstIterator iter = systemLevelTasks.Begin(); iter != systemLevelTasks.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( bufferIndex );
  }

  // Reset render-task list properties to base values.
  const RenderTaskList::RenderTaskContainer& tasks = mImpl->taskList.GetTasks();

  for (RenderTaskList::RenderTaskContainer::ConstIterator iter = tasks.Begin(); iter != tasks.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( bufferIndex );
  }

  // Reset custom object properties to base values
  for (OwnerContainer<PropertyOwner*>::Iterator iter = mImpl->customObjects.Begin(); iter != mImpl->customObjects.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( bufferIndex );
  }

  mImpl->materials.ResetToBaseValues( bufferIndex );
  mImpl->geometries.ResetToBaseValues( bufferIndex );
  mImpl->renderers.ResetToBaseValues( bufferIndex );


  // Reset animatable shader properties to base values
  for (ShaderIter iter = mImpl->shaders.Begin(); iter != mImpl->shaders.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( bufferIndex );
  }

  PERF_MONITOR_END(PerformanceMonitor::RESET_PROPERTIES);
}

bool UpdateManager::ProcessGestures( BufferIndex bufferIndex, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds )
{
  bool gestureUpdated( false );

  // constrain gestures... (in construction order)
  GestureContainer& gestures = mImpl->gestures;

  for ( GestureIter iter = gestures.Begin(), endIter = gestures.End(); iter != endIter; ++iter )
  {
    PanGesture& gesture = **iter;
    gesture.ResetToBaseValues( bufferIndex ); // Needs to be done every time as gesture data is written directly to an update-buffer rather than via a message
    gestureUpdated |= gesture.UpdateProperties( lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds );
  }

  return gestureUpdated;
}

void UpdateManager::Animate( BufferIndex bufferIndex, float elapsedSeconds )
{
  PERF_MONITOR_START(PerformanceMonitor::ANIMATE_NODES);

  AnimationContainer &animations = mImpl->animations;
  AnimationIter iter = animations.Begin();
  while ( iter != animations.End() )
  {
    Animation* animation = *iter;
    bool finished = animation->Update( bufferIndex, elapsedSeconds );

    mImpl->animationFinishedDuringUpdate = mImpl->animationFinishedDuringUpdate || finished;

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

  if ( mImpl->animationFinishedDuringUpdate )
  {
    // The application should be notified by NotificationManager, in another thread
    mImpl->notificationManager.QueueCompleteNotification( &mImpl->animationFinishedNotifier );
  }

  PERF_MONITOR_END(PerformanceMonitor::ANIMATE_NODES);
}

void UpdateManager::ApplyConstraints( BufferIndex bufferIndex )
{
  PERF_MONITOR_START(PerformanceMonitor::APPLY_CONSTRAINTS);

  // constrain custom objects... (in construction order)
  OwnerContainer< PropertyOwner* >& customObjects = mImpl->customObjects;

  const OwnerContainer< PropertyOwner* >::Iterator endIter = customObjects.End();
  for ( OwnerContainer< PropertyOwner* >::Iterator iter = customObjects.Begin(); endIter != iter; ++iter )
  {
    PropertyOwner& object = **iter;
    ConstrainPropertyOwner( object, bufferIndex );
  }

  // constrain nodes... (in Depth First traversal order)
  if ( mImpl->root )
  {
    ConstrainNodes( *(mImpl->root), bufferIndex );
  }

  if ( mImpl->systemLevelRoot )
  {
    ConstrainNodes( *(mImpl->systemLevelRoot), bufferIndex );
  }

  // constrain other property-owners after nodes as they are more likely to depend on a node's
  // current frame property than vice versa. They tend to be final constraints (no further
  // constraints depend on their properties)
  // e.g. ShaderEffect uniform a function of Actor's position.
  // Mesh vertex a function of Actor's position or world position.

  // TODO: refactor this code (and reset nodes) as these are all just lists of property-owners
  // they can be all processed in a super-list of property-owners.

  // Constrain system-level render-tasks
  const RenderTaskList::RenderTaskContainer& systemLevelTasks = mImpl->systemLevelTaskList.GetTasks();

  for ( RenderTaskList::RenderTaskContainer::ConstIterator iter = systemLevelTasks.Begin(); iter != systemLevelTasks.End(); ++iter )
  {
    RenderTask& task = **iter;
    ConstrainPropertyOwner( task, bufferIndex );
  }

  // Constrain render-tasks
  const RenderTaskList::RenderTaskContainer& tasks = mImpl->taskList.GetTasks();

  for ( RenderTaskList::RenderTaskContainer::ConstIterator iter = tasks.Begin(); iter != tasks.End(); ++iter )
  {
    RenderTask& task = **iter;
    ConstrainPropertyOwner( task, bufferIndex );
  }

  // Constrain Materials and geometries
  mImpl->materials.ConstrainObjects( bufferIndex );
  mImpl->geometries.ConstrainObjects( bufferIndex );
  mImpl->renderers.ConstrainObjects( bufferIndex );

  // constrain shaders... (in construction order)
  ShaderContainer& shaders = mImpl->shaders;

  for ( ShaderIter iter = shaders.Begin(); iter != shaders.End(); ++iter )
  {
    Shader& shader = **iter;
    ConstrainPropertyOwner( shader, bufferIndex );
  }

  PERF_MONITOR_END(PerformanceMonitor::APPLY_CONSTRAINTS);
}

void UpdateManager::ProcessPropertyNotifications( BufferIndex bufferIndex )
{
  PropertyNotificationContainer &notifications = mImpl->propertyNotifications;
  PropertyNotificationIter iter = notifications.Begin();

  while ( iter != notifications.End() )
  {
    PropertyNotification* notification = *iter;
    bool valid = notification->Check( bufferIndex );
    if(valid)
    {
      mImpl->notificationManager.QueueMessage( PropertyChangedMessage( mImpl->propertyNotifier, notification, notification->GetValidity() ) );
    }
    ++iter;
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
      ShaderDataBinaryQueue::iterator i   = mImpl->updateCompiledShaders.begin();
      ShaderDataBinaryQueue::iterator end = mImpl->updateCompiledShaders.end();
      for( ; i != end; ++i )
      {
        mImpl->notificationManager.QueueMessage( ShaderCompiledMessage( factory, *i ) );
      }
      // we don't need them in update anymore
      mImpl->updateCompiledShaders.clear();
    }
  }
}

void UpdateManager::UpdateRenderers( BufferIndex bufferIndex )
{
  const OwnerContainer<Renderer*>& rendererContainer( mImpl->renderers.GetObjectContainer() );
  unsigned int rendererCount( rendererContainer.Size() );
  for( unsigned int i(0); i<rendererCount; ++i )
  {
    if( rendererContainer[i]->IsReferenced() )
    {
      rendererContainer[i]->PrepareResources(bufferIndex, mImpl->resourceManager);
      rendererContainer[i]->PrepareRender( bufferIndex );
    }
  }
}

void UpdateManager::UpdateNodes( BufferIndex bufferIndex )
{
  mImpl->nodeDirtyFlags = NothingFlag;

  if ( !mImpl->root )
  {
    return;
  }

  PERF_MONITOR_START( PerformanceMonitor::UPDATE_NODES );

  // Prepare resources, update shaders, update attachments, for each node
  // And add the renderers to the sorted layers. Start from root, which is also a layer
  mImpl->nodeDirtyFlags = UpdateNodesAndAttachments( *( mImpl->root ),
                                                     bufferIndex,
                                                     mImpl->resourceManager,
                                                     mImpl->renderQueue );

  if ( mImpl->systemLevelRoot )
  {
    mImpl->nodeDirtyFlags |= UpdateNodesAndAttachments( *( mImpl->systemLevelRoot ),
                                                        bufferIndex,
                                                        mImpl->resourceManager,
                                                        mImpl->renderQueue );
  }

  PERF_MONITOR_END( PerformanceMonitor::UPDATE_NODES );
}

unsigned int UpdateManager::Update( float elapsedSeconds,
                                    unsigned int lastVSyncTimeMilliseconds,
                                    unsigned int nextVSyncTimeMilliseconds )
{
  PERF_MONITOR_END(PerformanceMonitor::FRAME_RATE);   // Mark the End of the last frame
  PERF_MONITOR_NEXT_FRAME();             // Prints out performance info for the last frame (if enabled)
  PERF_MONITOR_START(PerformanceMonitor::FRAME_RATE); // Mark the start of this current frame

  // Measure the time spent in UpdateManager::Update
  PERF_MONITOR_START(PerformanceMonitor::UPDATE);

  const BufferIndex bufferIndex = mSceneGraphBuffers.GetUpdateBufferIndex();

  // 1) Clear nodes/resources which were previously discarded
  mImpl->discardQueue.Clear( bufferIndex );

  // 2) Grab any loaded resources
  bool resourceChanged = mImpl->resourceManager.UpdateCache( bufferIndex );

  // 3) Process Touches & Gestures
  mImpl->touchResampler.Update();
  const bool gestureUpdated = ProcessGestures( bufferIndex, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds );

  const bool updateScene =                                  // The scene-graph requires an update if..
      (mImpl->nodeDirtyFlags & RenderableUpdateFlags) ||    // ..nodes were dirty in previous frame OR
      IsAnimationRunning()                            ||    // ..at least one animation is running OR
      mImpl->messageQueue.IsSceneUpdateRequired()     ||    // ..a message that modifies the scene graph node tree is queued OR
      resourceChanged                                 ||    // ..one or more resources were updated/changed OR
      gestureUpdated;                                       // ..a gesture property was updated


  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // values if the scene was updated in the previous frame.
  if( updateScene || mImpl->previousUpdateScene )
  {
    // 4) Reset properties from the previous update
    ResetProperties( bufferIndex );
  }

  // 5) Process the queued scene messages
  mImpl->messageQueue.ProcessMessages( bufferIndex );

  // 6) Post Process Ids of resources updated by renderer
  mImpl->resourceManager.PostProcessResources( bufferIndex );

  // 6.1) Forward compiled shader programs to event thread for saving
  ForwardCompiledShadersToEventThread();

  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // renderer lists if the scene was updated in the previous frame.
  // We should not start skipping update steps or reusing lists until there has been two frames where nothing changes
  if( updateScene || mImpl->previousUpdateScene )
  {
    // 7) Animate
    Animate( bufferIndex, elapsedSeconds );

    // 8) Apply Constraints
    ApplyConstraints( bufferIndex );

    // 9) Check Property Notifications
    ProcessPropertyNotifications( bufferIndex );

    // 10) Clear the lists of renderable-attachments from the previous update
    ClearRenderables( mImpl->sortedLayers );
    ClearRenderables( mImpl->systemLevelSortedLayers );

    // 11) Update node hierarchy and perform sorting / culling.
    //     This will populate each Layer with a list of renderers which are ready.
    UpdateNodes( bufferIndex );
    UpdateRenderers( bufferIndex );


    // 12) Prepare for the next render
    PERF_MONITOR_START(PerformanceMonitor::PREPARE_RENDERABLES);

    PrepareRenderables( bufferIndex, mImpl->sortedLayers );
    PrepareRenderables( bufferIndex, mImpl->systemLevelSortedLayers );
    PERF_MONITOR_END(PerformanceMonitor::PREPARE_RENDERABLES);

    PERF_MONITOR_START(PerformanceMonitor::PROCESS_RENDER_TASKS);

    // 14) Process the RenderTasks; this creates the instructions for rendering the next frame.
    // reset the update buffer index and make sure there is enough room in the instruction container
    mImpl->renderInstructions.ResetAndReserve( bufferIndex,
                                               mImpl->taskList.GetTasks().Count() + mImpl->systemLevelTaskList.GetTasks().Count() );

    if ( NULL != mImpl->root )
    {
      ProcessRenderTasks(  bufferIndex,
                           mImpl->completeStatusManager,
                           mImpl->taskList,
                           *mImpl->root,
                           mImpl->sortedLayers,
                           mImpl->renderSortingHelper,
                           mImpl->renderInstructions );

      // Process the system-level RenderTasks last
      if ( NULL != mImpl->systemLevelRoot )
      {
        ProcessRenderTasks(  bufferIndex,
                             mImpl->completeStatusManager,
                             mImpl->systemLevelTaskList,
                             *mImpl->systemLevelRoot,
                             mImpl->systemLevelSortedLayers,
                             mImpl->renderSortingHelper,
                             mImpl->renderInstructions );
      }
    }
  }

  // check the countdown and notify (note, at the moment this is only done for normal tasks, not for systemlevel tasks)
  bool doRenderOnceNotify = false;
  mImpl->renderTaskWaiting = false;
  const RenderTaskList::RenderTaskContainer& tasks = mImpl->taskList.GetTasks();
  for ( RenderTaskList::RenderTaskContainer::ConstIterator iter = tasks.Begin(), endIter = tasks.End();
        endIter != iter; ++iter )
  {
    RenderTask& renderTask(*(*iter));

    renderTask.UpdateState();

    if( renderTask.IsWaitingToRender() &&
        renderTask.ReadyToRender( bufferIndex ) /*avoid updating forever when source actor is off-stage*/ )
    {
      mImpl->renderTaskWaiting = true; // keep update/render threads alive
    }

    if( renderTask.HasRendered() )
    {
      doRenderOnceNotify = true;
    }
  }

  if( doRenderOnceNotify )
  {
    DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "Notify a render task has finished\n");
    mImpl->notificationManager.QueueCompleteNotification( mImpl->taskList.GetCompleteNotificationInterface() );
  }

  PERF_MONITOR_END(PerformanceMonitor::PROCESS_RENDER_TASKS);

  // Macro is undefined in release build.
  SNAPSHOT_NODE_LOGGING;

  // A ResetProperties() may be required in the next frame
  mImpl->previousUpdateScene = updateScene;

  // Check whether further updates are required
  unsigned int keepUpdating = KeepUpdatingCheck( elapsedSeconds );

#ifdef PERFORMANCE_MONITOR_ENABLED
  // Always keep rendering when measuring FPS
  keepUpdating |= KeepUpdating::MONITORING_PERFORMANCE;
#endif

  // tell the update manager that we're done so the queue can be given to event thread
  mImpl->notificationManager.UpdateCompleted();

  // The update has finished; swap the double-buffering indices
  mSceneGraphBuffers.Swap();

  PERF_MONITOR_END(PerformanceMonitor::UPDATE);

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

void UpdateManager::SetShaderSaver( ShaderSaver& upstream )
{
  mImpl->shaderSaver = &upstream;
}

void UpdateManager::AddSampler( Render::Sampler* sampler )
{
  typedef MessageValue1< RenderManager, Render::Sampler* > DerivedType;

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

void UpdateManager::SetWrapMode( Render::Sampler* sampler, unsigned int uWrapMode, unsigned int vWrapMode )
{
  typedef MessageValue3< RenderManager, Render::Sampler*, unsigned int, unsigned int > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetWrapMode, sampler, uWrapMode, vWrapMode );
}

void UpdateManager::AddPropertyBuffer( Render::PropertyBuffer* propertyBuffer )
{
  typedef MessageValue1< RenderManager, Render::PropertyBuffer* > DerivedType;

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

void UpdateManager::SetPropertyBufferFormat(Render::PropertyBuffer* propertyBuffer, Render::PropertyBuffer::Format* format )
{
  typedef MessageValue2< RenderManager, Render::PropertyBuffer*, Render::PropertyBuffer::Format* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetPropertyBufferFormat, propertyBuffer, format );
}

void UpdateManager::SetPropertyBufferData(Render::PropertyBuffer* propertyBuffer, Dali::Vector<char>* data)
{
  typedef MessageValue2< RenderManager, Render::PropertyBuffer*, Dali::Vector<char>* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetPropertyBufferData, propertyBuffer, data );
}

void UpdateManager::SetPropertyBufferSize(Render::PropertyBuffer* propertyBuffer, size_t size )
{
  typedef MessageValue2< RenderManager, Render::PropertyBuffer*, size_t > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager,  &RenderManager::SetPropertyBufferSize, propertyBuffer, size );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
