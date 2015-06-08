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
#include <dali/internal/update/manager/update-manager.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/common/set-wrapper.h>

#include <dali/integration-api/core.h>
#include <dali/integration-api/render-controller.h>
#include <dali/integration-api/shader-data.h>
#include <dali/integration-api/debug.h>

#include <dali/internal/common/core-impl.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/common/message.h>

#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/event/common/property-notifier.h>

#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/manager/prepare-render-algorithms.h>
#include <dali/internal/update/manager/process-render-tasks.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/manager/update-algorithms.h>
#include <dali/internal/update/queue/update-message-queue.h>
#include <dali/internal/update/manager/update-manager-debug.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller-impl.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/modeling/scene-graph-material.h>
#include <dali/internal/update/node-attachments/scene-graph-camera-attachment.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/touch/touch-resampler.h>

#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/renderers/render-material.h>
#include <dali/internal/render/shaders/shader.h>

#ifdef DALI_DYNAMICS_SUPPORT
#include <dali/integration-api/dynamics/dynamics-world-settings.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>
#endif

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

typedef AnimatableMeshContainer::Iterator      AnimatableMeshIter;
typedef AnimatableMeshContainer::ConstIterator AnimatableMeshConstIter;
typedef MaterialContainer::Iterator            MaterialIter;

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
    messageQueue( renderController, sceneGraphBuffers ),
    dynamicsChanged( false ),
    keepRenderingSeconds( 0.0f ),
    animationFinishedDuringUpdate( false ),
    nodeDirtyFlags( TransformFlag ), // set to TransformFlag to ensure full update the first time through Update()
    previousUpdateScene( false ),
    frameCounter( 0 ),
    renderSortingHelper(),
    renderTaskWaiting( false )
  {
    sceneController = new SceneControllerImpl( renderMessageDispatcher, renderQueue, discardQueue, textureCache, completeStatusManager );
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

  ShaderContainer                     shaders;                       ///< A container of owned shaders
  AnimatableMeshContainer             animatableMeshes;              ///< A container of owned animatable meshes
  MaterialContainer                   materials;                     ///< A container of owned materials

  MessageQueue                        messageQueue;                  ///< The messages queued from the event-thread

#ifdef DALI_DYNAMICS_SUPPORT
  OwnerPointer<DynamicsWorld>         dynamicsWorld;                 ///< Wrapper for dynamics simulation
#endif
  bool                                dynamicsChanged;               ///< This is set to true if an object is changed in the dynamics simulation tick

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

void UpdateManager::ConnectNode( Node* parent, Node* node, int index )
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

  parent->ConnectChild( node, index );
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

void UpdateManager::AttachToNode( Node* node, NodeAttachment* attachment )
{
  DALI_ASSERT_DEBUG( node != NULL );
  DALI_ASSERT_DEBUG( attachment != NULL );

  // attach node to attachment first so that parent is known by the time attachment is connected
  node->Attach( *attachment ); // node takes ownership
  attachment->ConnectToSceneGraph( *mImpl->sceneController, mSceneGraphBuffers.GetUpdateBufferIndex() );
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

void UpdateManager::RemoveShader(Shader* shader)
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

void UpdateManager::SetShaderProgram( Shader* shader, GeometryType geometryType, ShaderSubTypes subType, ResourceId resourceId, size_t shaderHash, bool modifiesGeometry )
{
  DALI_LOG_TRACE_METHOD_FMT(Debug::Filter::gShader, " - (geometryType:%d subType:%d id:%d hash:%d)\n", geometryType, subType, resourceId, shaderHash);

  DALI_ASSERT_ALWAYS( NULL != shader && "shader is uninitialized" );

  Integration::ShaderDataPtr shaderData( mImpl->resourceManager.GetShaderData(resourceId) );
  if( shaderData )
  {
    shaderData->SetHashValue( shaderHash );
    shaderData->SetResourceId( resourceId );

    typedef MessageValue6< Shader, GeometryType, Internal::ShaderSubTypes, Integration::ResourceId, Integration::ShaderDataPtr, ProgramCache*, bool> DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( shader, &Shader::SetProgram, geometryType, subType, resourceId, shaderData, mImpl->renderManager.GetProgramCache(), modifiesGeometry );
  }
}

void UpdateManager::AddAnimatableMesh( AnimatableMesh* animatableMesh )
{
  mImpl->animatableMeshes.PushBack(animatableMesh);
}

void UpdateManager::RemoveAnimatableMesh( AnimatableMesh* animatableMesh )
{
  DALI_ASSERT_DEBUG(animatableMesh != NULL);

  AnimatableMeshContainer& animatableMeshes = mImpl->animatableMeshes;

  // Find the animatableMesh and destroy it
  for ( AnimatableMeshIter iter = animatableMeshes.Begin(); iter != animatableMeshes.End(); ++iter )
  {
    AnimatableMesh& current = **iter;
    if ( &current == animatableMesh )
    {
      animatableMeshes.Erase( iter );
      break;
    }
  }
}

void UpdateManager::AddMaterial( Material* material )
{
  DALI_ASSERT_DEBUG( NULL != material );

  mImpl->materials.PushBack( material );
  RenderMaterial* renderMaterial = new RenderMaterial();

  typedef MessageValue1< RenderManager, RenderMaterial* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mImpl->renderManager, &RenderManager::AddRenderMaterial, renderMaterial );

  material->SetRenderMaterial( renderMaterial );
  material->OnStageConnection( *mImpl->sceneController );
}

void UpdateManager::RemoveMaterial( Material* theMaterial )
{
  // Caused by last reference to material being released (e.g. app or internal mesh-actor)

  for ( MaterialIter iter=mImpl->materials.Begin(), end=mImpl->materials.End() ; iter != end ; ++iter )
  {
    const Material* aMaterial = *iter;

    if( aMaterial == theMaterial )
    {
      typedef MessageValue1< RenderManager, RenderMaterial* > DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = mImpl->renderQueue.ReserveMessageSlot( mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( &mImpl->renderManager, &RenderManager::RemoveRenderMaterial, theMaterial->GetRenderMaterial() );

      mImpl->materials.Erase( iter );
      break;
    }
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

void UpdateManager::ResetNodeProperty( Node& node )
{
  node.ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() );
}

void UpdateManager::ResetProperties()
{
  PERF_MONITOR_START(PerformanceMonitor::RESET_PROPERTIES);

  // Clear the "animations finished" flag; This should be set if any (previously playing) animation is stopped
  mImpl->animationFinishedDuringUpdate = false;

  // Animated properties have to be reset to their original value each frame

  // Reset node properties
  if ( mImpl->root )
  {
    ResetNodeProperty( *mImpl->root );
  }

  if ( mImpl->systemLevelRoot )
  {
    ResetNodeProperty( *mImpl->systemLevelRoot );
  }

  // Reset the Connected Nodes
  const std::set<Node*>::iterator endIter = mImpl->connectedNodes.end();
  for( std::set<Node*>::iterator iter = mImpl->connectedNodes.begin(); endIter != iter; ++iter )
  {
    ResetNodeProperty( **iter );
  }

  // If a Node is disconnected, it may still be "active" (requires a reset in next frame)
  for( std::set<Node*>::iterator iter = mImpl->activeDisconnectedNodes.begin(); mImpl->activeDisconnectedNodes.end() != iter; iter = mImpl->activeDisconnectedNodes.begin() )
  {
    Node* node = *iter;
    node->ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() );
    node->SetActive( false );

    // Move everything from activeDisconnectedNodes to disconnectedNodes (no need to reset again)
    mImpl->activeDisconnectedNodes.erase( iter );
    mImpl->disconnectedNodes.insert( node );
  }

  // Reset system-level render-task list properties to base values
  const RenderTaskList::RenderTaskContainer& systemLevelTasks = mImpl->systemLevelTaskList.GetTasks();

  for (RenderTaskList::RenderTaskContainer::ConstIterator iter = systemLevelTasks.Begin(); iter != systemLevelTasks.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // Reset render-task list properties to base values.
  const RenderTaskList::RenderTaskContainer& tasks = mImpl->taskList.GetTasks();

  for (RenderTaskList::RenderTaskContainer::ConstIterator iter = tasks.Begin(); iter != tasks.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // Reset custom object properties to base values
  for (OwnerContainer<PropertyOwner*>::Iterator iter = mImpl->customObjects.Begin(); iter != mImpl->customObjects.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // Reset animatable shader properties to base values
  for (ShaderIter iter = mImpl->shaders.Begin(); iter != mImpl->shaders.End(); ++iter)
  {
    (*iter)->ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // Reset animatable mesh properties to base values
  for ( AnimatableMeshIter iter = mImpl->animatableMeshes.Begin(); iter != mImpl->animatableMeshes.End(); ++iter )
  {
    (*iter)->ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  PERF_MONITOR_END(PerformanceMonitor::RESET_PROPERTIES);
}

bool UpdateManager::ProcessGestures( unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds )
{
  bool gestureUpdated( false );

  // constrain gestures... (in construction order)
  GestureContainer& gestures = mImpl->gestures;

  for ( GestureIter iter = gestures.Begin(), endIter = gestures.End(); iter != endIter; ++iter )
  {
    PanGesture& gesture = **iter;
    gesture.ResetToBaseValues( mSceneGraphBuffers.GetUpdateBufferIndex() ); // Needs to be done every time as gesture data is written directly to an update-buffer rather than via a message
    gestureUpdated |= gesture.UpdateProperties( lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds );
  }

  return gestureUpdated;
}

void UpdateManager::Animate( float elapsedSeconds )
{
  PERF_MONITOR_START(PerformanceMonitor::ANIMATE_NODES);

  AnimationContainer &animations = mImpl->animations;
  AnimationIter iter = animations.Begin();
  while ( iter != animations.End() )
  {
    Animation* animation = *iter;
    bool finished = animation->Update(mSceneGraphBuffers.GetUpdateBufferIndex(), elapsedSeconds);

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

void UpdateManager::ApplyConstraints()
{
  PERF_MONITOR_START(PerformanceMonitor::APPLY_CONSTRAINTS);

  // constrain custom objects... (in construction order)
  OwnerContainer< PropertyOwner* >& customObjects = mImpl->customObjects;

  const OwnerContainer< PropertyOwner* >::Iterator endIter = customObjects.End();
  for ( OwnerContainer< PropertyOwner* >::Iterator iter = customObjects.Begin(); endIter != iter; ++iter )
  {
    PropertyOwner& object = **iter;
    ConstrainPropertyOwner( object, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // constrain nodes... (in Depth First traversal order)
  if ( mImpl->root )
  {
    ConstrainNodes( *(mImpl->root), mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  if ( mImpl->systemLevelRoot )
  {
    ConstrainNodes( *(mImpl->systemLevelRoot), mSceneGraphBuffers.GetUpdateBufferIndex() );
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
    ConstrainPropertyOwner( task, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // Constrain render-tasks
  const RenderTaskList::RenderTaskContainer& tasks = mImpl->taskList.GetTasks();

  for ( RenderTaskList::RenderTaskContainer::ConstIterator iter = tasks.Begin(); iter != tasks.End(); ++iter )
  {
    RenderTask& task = **iter;
    ConstrainPropertyOwner( task, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // constrain meshes (in construction order)
  AnimatableMeshContainer& meshes = mImpl->animatableMeshes;
  for ( AnimatableMeshIter iter = meshes.Begin(); iter != meshes.End(); ++iter )
  {
    AnimatableMesh& mesh = **iter;
    ConstrainPropertyOwner( mesh, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  // constrain shaders... (in construction order)
  ShaderContainer& shaders = mImpl->shaders;

  for ( ShaderIter iter = shaders.Begin(); iter != shaders.End(); ++iter )
  {
    Shader& shader = **iter;
    ConstrainPropertyOwner( shader, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  PERF_MONITOR_END(PerformanceMonitor::APPLY_CONSTRAINTS);
}

void UpdateManager::ProcessPropertyNotifications()
{
  PropertyNotificationContainer &notifications = mImpl->propertyNotifications;
  PropertyNotificationIter iter = notifications.Begin();

  while ( iter != notifications.End() )
  {
    PropertyNotification* notification = *iter;

    bool valid = notification->Check( mSceneGraphBuffers.GetUpdateBufferIndex() );

    if(valid)
    {
      mImpl->notificationManager.QueueMessage( PropertyChangedMessage( mImpl->propertyNotifier, notification, notification->GetValidity() ) );
    }
    ++iter;
  }
}

void UpdateManager::UpdateNodes()
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
                                                     mSceneGraphBuffers.GetUpdateBufferIndex(),
                                                     mImpl->resourceManager,
                                                     mImpl->renderQueue );

  if ( mImpl->systemLevelRoot )
  {
    mImpl->nodeDirtyFlags |= UpdateNodesAndAttachments( *( mImpl->systemLevelRoot ),
                                                        mSceneGraphBuffers.GetUpdateBufferIndex(),
                                                        mImpl->resourceManager,
                                                        mImpl->renderQueue );
  }

  PERF_MONITOR_END( PerformanceMonitor::UPDATE_NODES );
}

void UpdateManager::UpdateMeshes( BufferIndex updateBufferIndex, AnimatableMeshContainer& meshes )
{
  for ( AnimatableMeshIter iter = meshes.Begin(); iter != meshes.End(); ++iter )
  {
    AnimatableMesh& current = **iter;
    current.UpdateMesh( updateBufferIndex );
  }
}

void UpdateManager::UpdateMaterials( BufferIndex updateBufferIndex, MaterialContainer& materials )
{
  for( MaterialIter iter = materials.Begin(), end = materials.End(); iter != end; iter++ )
  {
    Material* material = *iter;
    material->PrepareResources( updateBufferIndex, mImpl->resourceManager );
  }
}

void UpdateManager::PrepareMaterials( BufferIndex updateBufferIndex, MaterialContainer& materials )
{
  for( MaterialIter iter = materials.Begin(), end = materials.End(); iter != end; iter++ )
  {
    Material* material = *iter;
    material->PrepareRender( updateBufferIndex );
  }
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

  // Update the frame time delta on the render thread.
  mImpl->renderManager.SetFrameDeltaTime(elapsedSeconds);

  // 1) Clear nodes/resources which were previously discarded
  mImpl->discardQueue.Clear( mSceneGraphBuffers.GetUpdateBufferIndex() );

  // 2) Grab any loaded resources
  bool resourceChanged = mImpl->resourceManager.UpdateCache( mSceneGraphBuffers.GetUpdateBufferIndex() );

  // 3) Process Touches & Gestures
  mImpl->touchResampler.Update();
  const bool gestureUpdated = ProcessGestures( lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds );

  const bool updateScene =                                            // The scene-graph requires an update if..
      (mImpl->nodeDirtyFlags & RenderableUpdateFlags) ||              // ..nodes were dirty in previous frame OR
      IsAnimationRunning() ||                                         // ..at least one animation is running OR
      mImpl->dynamicsChanged ||                                       // ..there was a change in the dynamics simulation OR
      mImpl->messageQueue.IsSceneUpdateRequired() ||                  // ..a message that modifies the scene graph node tree is queued OR
      resourceChanged ||                                              // ..one or more resources were updated/changed OR
      gestureUpdated;                                                // ..a gesture property was updated

  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // values if the scene was updated in the previous frame.
  if( updateScene || mImpl->previousUpdateScene )
  {
    // 4) Reset properties from the previous update
    ResetProperties();
  }

  // 5) Process the queued scene messages
  mImpl->messageQueue.ProcessMessages();

  // 6) Post Process Ids of resources updated by renderer
  mImpl->resourceManager.PostProcessResources( mSceneGraphBuffers.GetUpdateBufferIndex() );

  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // renderer lists if the scene was updated in the previous frame.
  // We should not start skipping update steps or reusing lists until there has been two frames where nothing changes
  if( updateScene || mImpl->previousUpdateScene )
  {
    // 7) Animate
    Animate( elapsedSeconds );

    // 8) Apply Constraints
    ApplyConstraints();

#ifdef DALI_DYNAMICS_SUPPORT
    // 9) Update dynamics simulation
    mImpl->dynamicsChanged = false;
    if( mImpl->dynamicsWorld )
    {
      mImpl->dynamicsChanged = mImpl->dynamicsWorld->Update( elapsedSeconds );
    }
#endif

    // 10) Check Property Notifications
    ProcessPropertyNotifications();

    // 11) Clear the lists of renderable-attachments from the previous update
    ClearRenderables( mImpl->sortedLayers );
    ClearRenderables( mImpl->systemLevelSortedLayers );

    // 12) Update animated meshes
    UpdateMeshes( mSceneGraphBuffers.GetUpdateBufferIndex(), mImpl->animatableMeshes );

    // 13) Update materials. Prepares image resources
    UpdateMaterials( mSceneGraphBuffers.GetUpdateBufferIndex(), mImpl->materials );

    // 14) Update node hierarchy and perform sorting / culling.
    //     This will populate each Layer with a list of renderers which are ready.
    UpdateNodes();

    // 15) Prepare for the next render
    PERF_MONITOR_START(PerformanceMonitor::PREPARE_RENDERABLES);
    PrepareMaterials( mSceneGraphBuffers.GetUpdateBufferIndex(), mImpl->materials );
    PrepareRenderables( mSceneGraphBuffers.GetUpdateBufferIndex(), mImpl->sortedLayers );
    PrepareRenderables( mSceneGraphBuffers.GetUpdateBufferIndex(), mImpl->systemLevelSortedLayers );
    PERF_MONITOR_END(PerformanceMonitor::PREPARE_RENDERABLES);

    PERF_MONITOR_START(PerformanceMonitor::PROCESS_RENDER_TASKS);

    // 16) Process the RenderTasks; this creates the instructions for rendering the next frame.
    // reset the update buffer index and make sure there is enough room in the instruction container
    mImpl->renderInstructions.ResetAndReserve( mSceneGraphBuffers.GetUpdateBufferIndex(),
                                               mImpl->taskList.GetTasks().Count() + mImpl->systemLevelTaskList.GetTasks().Count() );

    if ( NULL != mImpl->root )
    {
      ProcessRenderTasks(  mSceneGraphBuffers.GetUpdateBufferIndex(),
                           mImpl->completeStatusManager,
                           mImpl->taskList,
                           *mImpl->root,
                           mImpl->sortedLayers,
                           mImpl->renderSortingHelper,
                           mImpl->renderInstructions );

      // Process the system-level RenderTasks last
      if ( NULL != mImpl->systemLevelRoot )
      {
        ProcessRenderTasks(  mSceneGraphBuffers.GetUpdateBufferIndex(),
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
        renderTask.ReadyToRender(mSceneGraphBuffers.GetUpdateBufferIndex()) /*avoid updating forever when source actor is off-stage*/ )
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

  // The update has finished; swap the double-buffering indices
  mSceneGraphBuffers.Swap();

  // tell the update manager that we're done so the queue can be given to event thread
  mImpl->notificationManager.UpdateCompleted();

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

  if ( mImpl->dynamicsChanged )
  {
    keepUpdatingRequest |= KeepUpdating::DYNAMICS_CHANGED;
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

#ifdef DALI_DYNAMICS_SUPPORT

void UpdateManager::InitializeDynamicsWorld( SceneGraph::DynamicsWorld* dynamicsWorld, Integration::DynamicsWorldSettings* worldSettings )
{
  dynamicsWorld->Initialize( mImpl->sceneController, worldSettings, &mSceneGraphBuffers );
  mImpl->dynamicsWorld = dynamicsWorld;
}

void UpdateManager::TerminateDynamicsWorld()
{
  mImpl->dynamicsWorld.Reset();
}

#endif // DALI_DYNAMICS_SUPPORT

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
