/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>

#include <dali/integration-api/core.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/render-controller.h>
#include <dali/graphics-api/graphics-controller.h>

#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread-pool.h>
#include <dali/internal/common/core-impl.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/event/common/property-notifier.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/controllers/scene-controller-impl.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/graphics/graphics-algorithms.h>
#include <dali/internal/update/manager/frame-callback-processor.h>
#include <dali/internal/update/manager/render-task-processor.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/manager/update-algorithms.h>
#include <dali/internal/update/manager/update-manager-debug.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/queue/update-message-queue.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/update/rendering/shader-cache.h>
#include <dali/internal/update/rendering/scene-graph-scene.h>

#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-buffer.h>

// Un-comment to enable node tree debug logging
//#define NODE_TREE_LOGGING 1

#if ( defined( DEBUG_ENABLED ) && defined( NODE_TREE_LOGGING ) )
#define SNAPSHOT_NODE_LOGGING \
const uint32_t FRAME_COUNT_TRIGGER = 16;\
if( mImpl->frameCounter >= FRAME_COUNT_TRIGGER )\
  {\
    for( auto root : mImpl->roots )
    {\
      if ( NULL != root )\
      {\
        mImpl->frameCounter = 0;\
        PrintNodeTree( *root, mSceneGraphBuffers.GetUpdateBufferIndex(), "" );\
      }\
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
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_UPDATE" );
#endif

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
 * Structures describes a single request for texture upload
 */
struct TextureUploadRequest
{
  TextureUploadRequest( Texture* texture, const PixelDataPtr& pixelData, const Internal::Texture::UploadParams& uploadParams )
    : texture( texture ),
      pixelData( pixelData ),
      uploadParams( uploadParams )
  {}
  Texture*                        texture;
  PixelDataPtr                    pixelData;
  Internal::Texture::UploadParams uploadParams;
};

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
        SceneGraphBuffers& sceneGraphBuffers,
        RenderTaskProcessor& renderTaskProcessor,
        Graphics::Controller& graphicsController )
  : notificationManager( notificationManager ),
    transformManager(),
    animationPlaylist( animationPlaylist ),
    propertyNotifier( propertyNotifier ),
    discardQueue( discardQueue ),
    sceneController( nullptr ),
    graphicsAlgorithms( graphicsController ),
    renderInstructions( ),
    renderTaskProcessor( renderTaskProcessor ),
    graphicsController( graphicsController ),
    backgroundColor( Dali::Stage::DEFAULT_BACKGROUND_COLOR ),
    renderers(),
    textureSets(),
    shaders(),
    panGestureProcessor( nullptr ),
    messageQueue( renderController, sceneGraphBuffers ),
    frameCallbackProcessor( nullptr ),
    keepRenderingSeconds( 0.0f ),
    nodeDirtyFlags( NodePropertyFlags::TRANSFORM ), // set to TransformFlag to ensure full update the first time through Update()
    frameCounter( 0 ),
    renderingBehavior( DevelStage::Rendering::IF_REQUIRED ),
    animationFinishedDuringUpdate( false ),
    previousUpdateScene( false ),
    renderTaskWaiting( false ),
    renderersAdded( false ),
    surfaceRectChanged( false ),
    graphicsShutdown( false ),
    shaderCache( graphicsController )
  {
    sceneController = new SceneControllerImpl( discardQueue );

    /**
     * Create thread pool with just one thread ( there may be a need to create more threads in the future ).
     */

    threadPool = std::make_unique<Dali::ThreadPool>();
    threadPool->Initialize( 1 );

    // create first 'dummy' node
    nodes.PushBack(0u);
  }

  ~Impl()
  {
    threadPool.reset( nullptr ); // reset now to maintain correct destruction order

    // Disconnect render tasks from nodes, before destroying the nodes
    for( auto taskList : taskLists )
    {
      RenderTaskList::RenderTaskContainer& tasks = taskList->GetTasks();
      for ( auto&& task : tasks )
      {
        task->SetSourceNode( nullptr );
      }
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

    for( auto root : roots )
    {
      root->OnDestroy();
    }

    delete sceneController;
  }

  /**
   * Lazy init for FrameCallbackProcessor.
   * @param[in]  updateManager  A reference to the update-manager
   */
  FrameCallbackProcessor& GetFrameCallbackProcessor( UpdateManager& updateManager )
  {
    if( ! frameCallbackProcessor )
    {
      frameCallbackProcessor = new FrameCallbackProcessor( updateManager, transformManager );
    }
    return *frameCallbackProcessor;
  }

  /**
   * Processes texture uploads
   * @param[in] processParallel True enables parallel processing
   * @param[in] useSingleStagingBuffer if true a single staging buffer is used for all uploads during frame
   * @return Shared pointer to the Future object
   */
  Dali::SharedFuture ProcessTextureUploadRequests( bool processParallel, bool useSingleStagingBuffer )
  {
    if( textureUploadRequestContainer.empty() )
    {
      return {};
    }

    /**
     * Worker thread lambda function
     */
    auto workerFunction = [&, useSingleStagingBuffer]( int workerThread )
    {
      textureUpdateInfoArray.clear();
      textureUpdateSourceInfoArray.clear();
      textureUpdateInfoArray.reserve( textureUploadRequestContainer.size() );
      textureUpdateSourceInfoArray.reserve( textureUploadRequestContainer.size() );

      uint32_t sourceIndex { 0u };
      for( auto& request : textureUploadRequestContainer )
      {
        auto pixelData = request.pixelData;

        // Initialise texture
        if( useSingleStagingBuffer )
        {
          auto info = Graphics::TextureUpdateInfo{};

          // initialise texture object without allocating memory for it yet
          if(!request.texture->GetGraphicsObject())
          {
            request.texture->CreateTextureInternal( 0|Graphics::TextureUsageFlagBits::SAMPLE, nullptr, 0u );
          }

          // prepare transfer info structure
          info.dstTexture = request.texture->GetGraphicsObject();
          info.dstOffset2D = { request.uploadParams.xOffset, request.uploadParams.yOffset };
          info.srcOffset = 0u;
          info.srcSize = request.pixelData->GetBufferSize();
          info.srcReference = sourceIndex++;
          info.srcExtent2D = { request.pixelData->GetWidth(), request.pixelData->GetHeight() };
          textureUpdateInfoArray.emplace_back( info );

          // store source
          auto source = Graphics::TextureUpdateSourceInfo{};
          source.sourceType = Graphics::TextureUpdateSourceInfo::Type::MEMORY;
          source.memorySource.memory = pixelData->GetBuffer();
          textureUpdateSourceInfoArray.emplace_back( source );
        }
        else
        {
          request.texture->UploadTexture( pixelData, request.uploadParams );
        }
      }

      /**
       * When using single staging buffer delegate all uploads to the controller for further processing
       */
      if( useSingleStagingBuffer )
      {
        auto controller = &graphicsController;
        controller->UpdateTextures( textureUpdateInfoArray, textureUpdateSourceInfoArray );
      }
    };

    if( processParallel )
    {
      return threadPool->SubmitTask( 0u, workerFunction );
    }
    else
    {
      // If not processing in parallel, just call lambda directly and return empty future
      workerFunction(0);
    }

    return {};
  }

  SceneGraphBuffers                    sceneGraphBuffers;             ///< Used to keep track of which buffers are being written or read
  NotificationManager&                 notificationManager;           ///< Queues notification messages for the event-thread.
  TransformManager                     transformManager;              ///< Used to update the transformation matrices of the nodes
  CompleteNotificationInterface&       animationPlaylist;             ///< Holds handles to all the animations
  PropertyNotifier&                    propertyNotifier;              ///< Provides notification to applications when properties are modified.

  DiscardQueue&                        discardQueue;                  ///< Nodes are added here when disconnected from the scene-graph.
  SceneControllerImpl*                 sceneController;               ///< scene controller
  GraphicsAlgorithms                   graphicsAlgorithms;            ///< Graphics algorithms
  RenderInstructionContainer           renderInstructions;            ///< List of current instructions per frame
  RenderTaskProcessor&                 renderTaskProcessor;           ///< Handles RenderTasks and RenderInstrucitons
  Graphics::Controller&                graphicsController;            ///< Graphics controller

  Vector4                              backgroundColor;               ///< The glClear color used at the beginning of each frame.

  OwnerContainer<RenderTaskList*>      taskLists;                     ///< A container of scene graph render task lists

  OwnerContainer<Layer*>               roots;                         ///< A container of root nodes (root is a layer). The layers are not stored in the node memory pool.

  Vector<Node*>                        nodes;                         ///< A container of all instantiated nodes

  std::vector<SortedLayerPointers>     sortedLayerLists;              ///< A container of lists of Layer pointers sorted by depth (one list of sorted layers per root)

  OwnerContainer< Camera* >            cameras;                       ///< A container of cameras
  OwnerContainer< PropertyOwner* >     customObjects;                 ///< A container of owned objects (with custom properties)

  OwnerContainer< PropertyResetterBase* > propertyResetters;          ///< A container of property resetters
  OwnerContainer< Animation* >         animations;                    ///< A container of owned animations
  PropertyNotificationContainer        propertyNotifications;         ///< A container of owner property notifications.
  OwnerContainer< Renderer* >          renderers;                     ///< A container of owned renderers
  OwnerContainer< TextureSet* >        textureSets;                   ///< A container of owned texture sets
  OwnerContainer< Shader* >            shaders;                       ///< A container of owned shaders
  OwnerContainer< Sampler* >           samplerContainer;              ///< A container of owned samplers
  OwnerContainer< Texture* >           textureContainer;              ///< A container of owned textures
  OwnerContainer< FrameBuffer* >       frameBufferContainer;          ///< A container of owned framebuffers
  OwnerContainer< PropertyBuffer* >    propertyBufferContainer;       ///< A container of owned property buffers
  OwnerContainer< Geometry* >          geometryContainer;             ///< A container of owned Geometries

  OwnerPointer< PanGesture >           panGestureProcessor;           ///< Owned pan gesture processor; it lives for the lifecycle of UpdateManager

  MessageQueue                         messageQueue;                  ///< The messages queued from the event-thread

  OwnerPointer<FrameCallbackProcessor> frameCallbackProcessor;        ///< Owned FrameCallbackProcessor, only created if required.

  float                                keepRenderingSeconds;          ///< Set via Dali::Stage::KeepRendering
  NodePropertyFlags                    nodeDirtyFlags;                ///< cumulative node dirty flags from previous frame
  uint32_t                             frameCounter;                  ///< Frame counter used in debugging to choose which frame to debug and which to ignore.

  DevelStage::Rendering                renderingBehavior;             ///< Set via DevelStage::SetRenderingBehavior

  bool                                 animationFinishedDuringUpdate; ///< Flag whether any animations finished during the Update()
  bool                                 previousUpdateScene;           ///< True if the scene was updated in the previous frame (otherwise it was optimized out)
  bool                                 renderTaskWaiting;             ///< A REFRESH_ONCE render task is waiting to be rendered
  bool                                 renderersAdded;                ///< Flag to keep track when renderers have been added to avoid unnecessary processing
  bool                                 surfaceRectChanged;            ///< True if the default surface rect is changed
  bool                                 graphicsShutdown;              ///< True if the graphics subsystem has shutdown

  ShaderCache                          shaderCache;
  std::vector<TextureUploadRequest>    textureUploadRequestContainer;
  std::unique_ptr<Dali::ThreadPool> threadPool;

  std::vector<Graphics::TextureUpdateInfo> textureUpdateInfoArray;
  std::vector<Graphics::TextureUpdateSourceInfo> textureUpdateSourceInfoArray;

  OwnerPointer<Scene> defaultScene;

private:

  Impl( const Impl& ); ///< Undefined
  Impl& operator=( const Impl& ); ///< Undefined
};

UpdateManager::UpdateManager( NotificationManager&             notificationManager,
                              CompleteNotificationInterface&   animationFinishedNotifier,
                              PropertyNotifier&                propertyNotifier,
                              DiscardQueue&                    discardQueue,
                              RenderController&                controller,
                              RenderTaskProcessor&             renderTaskProcessor,
                              Graphics::Controller&            graphicsController )
: mImpl( new Impl( notificationManager,
                   animationFinishedNotifier,
                   propertyNotifier,
                   discardQueue,
                   controller,
                   mSceneGraphBuffers,
                   renderTaskProcessor,
                   graphicsController) )
{
  mImpl->defaultScene = new SceneGraph::Scene();
  Integration::DepthBufferAvailable depthNeeded=Integration::DepthBufferAvailable::TRUE;
  Integration::StencilBufferAvailable stencilBufferAvailable = Integration::StencilBufferAvailable::TRUE;
  mImpl->defaultScene->Initialize(graphicsController, depthNeeded, stencilBufferAvailable);
}

UpdateManager::~UpdateManager()
{
  // required due to unique_ptr to mImpl
}


void UpdateManager::InstallRoot( OwnerPointer<Layer>& layer )
{
  DALI_ASSERT_DEBUG( layer->IsLayer() );
  DALI_ASSERT_DEBUG( layer->GetParent() == NULL);

  Layer* rootLayer = layer.Release();

  DALI_ASSERT_DEBUG( std::find( mImpl->roots.begin(), mImpl->roots.end(), rootLayer ) == mImpl->roots.end() && "Root Node already installed" );

  rootLayer->CreateTransform( &mImpl->transformManager );
  rootLayer->SetRoot(true);
  mImpl->roots.PushBack( rootLayer );
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

  // Inform the frame-callback-processor, if set, about the node-hierarchy changing
  if( mImpl->frameCallbackProcessor )
  {
    mImpl->frameCallbackProcessor->NodeHierarchyChanged();
  }
}

void UpdateManager::DisconnectNode( Node* node )
{
  Node* parent = node->GetParent();
  DALI_ASSERT_ALWAYS( NULL != parent );
  parent->SetDirtyFlag( NodePropertyFlags::CHILD_DELETED ); // make parent dirty so that render items dont get reused

  parent->DisconnectChild( mSceneGraphBuffers.GetUpdateBufferIndex(), *node );

  // Inform the frame-callback-processor, if set, about the node-hierarchy changing
  if( mImpl->frameCallbackProcessor )
  {
    mImpl->frameCallbackProcessor->NodeHierarchyChanged();
  }
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

void UpdateManager::RemoveCamera( Camera* camera )
{
  // Find the camera and destroy it
  EraseUsingDiscardQueue( mImpl->cameras, camera, mImpl->discardQueue, mSceneGraphBuffers.GetUpdateBufferIndex() );
}

void UpdateManager::AddObject( OwnerPointer<PropertyOwner>& object )
{
  mImpl->customObjects.PushBack( object.Release() );
}

void UpdateManager::RemoveObject( PropertyOwner* object )
{
  mImpl->customObjects.EraseObject( object );
}

void UpdateManager::AddRenderTaskList( OwnerPointer<RenderTaskList>& taskList )
{
  RenderTaskList* taskListPointer = taskList.Release();
  mImpl->taskLists.PushBack( taskListPointer );
}

void UpdateManager::RemoveRenderTaskList( RenderTaskList* taskList )
{
  mImpl->taskLists.EraseObject( taskList );
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

void UpdateManager::AddPropertyResetter( OwnerPointer<PropertyResetterBase>& propertyResetter )
{
  propertyResetter->Initialize();
  mImpl->propertyResetters.PushBack( propertyResetter.Release() );
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
  shader->Initialize( mImpl->graphicsController, mImpl->shaderCache, mImpl->graphicsAlgorithms.GetUniformBufferManager());
  mImpl->shaders.PushBack( shader.Release() );
}

void UpdateManager::RemoveShader( Shader* shader )
{
  // Find the shader and destroy it
  EraseUsingDiscardQueue( mImpl->shaders, shader, mImpl->discardQueue, mSceneGraphBuffers.GetUpdateBufferIndex() );
}

void UpdateManager::AddRenderer( OwnerPointer< Renderer >& renderer )
{
  renderer->Initialize( mImpl->graphicsController );
  mImpl->renderers.PushBack( renderer.Release() );
  mImpl->renderersAdded = true;
}

void UpdateManager::RemoveRenderer( Renderer* renderer )
{
  // Find the renderer and destroy it
  // @todo Don't need to use discard queue for SceneGraph::Renderer any more ( No dependency from Graphics::RenderCommand )
  EraseUsingDiscardQueue( mImpl->renderers, renderer, mImpl->discardQueue, mSceneGraphBuffers.GetUpdateBufferIndex() );
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

uint32_t* UpdateManager::ReserveMessageSlot( uint32_t size, bool updateScene )
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

  // Reset all animating / constrained properties
  std::vector<PropertyResetterBase*>toDelete;
  for( auto&& element : mImpl->propertyResetters )
  {
    element->ResetToBaseValue( bufferIndex );
    if( element->IsFinished() )
    {
      toDelete.push_back( element );
    }
  }

  // If a resetter is no longer required (the animator or constraint has been removed), delete it.
  for( auto&& elementPtr : toDelete )
  {
    mImpl->propertyResetters.EraseObject( elementPtr );
  }

  // Clear root dirty flags
  for( auto&& rootLayer : mImpl->roots )
  {
    rootLayer->ResetDirtyFlags( bufferIndex );
  }

  // Clear node dirty flags
  Vector<Node*>::Iterator iter = mImpl->nodes.Begin()+1;
  Vector<Node*>::Iterator endIter = mImpl->nodes.End();
  for( ;iter != endIter; ++iter )
  {
    (*iter)->ResetDirtyFlags( bufferIndex );
  }
}

bool UpdateManager::ProcessGestures( BufferIndex bufferIndex, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds )
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
  auto&& iter = mImpl->animations.Begin();
  bool animationLooped = false;

  while ( iter != mImpl->animations.End() )
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
      iter = mImpl->animations.Erase(iter);
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
  // Constrain render-tasks
  for( auto taskList : mImpl->taskLists )
  {
    RenderTaskList::RenderTaskContainer& tasks = taskList->GetTasks();
    for ( auto&& task : tasks )
    {
      ConstrainPropertyOwner( *task, bufferIndex );
    }
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

void UpdateManager::UpdateRenderers( BufferIndex bufferIndex )
{
  auto rendererCount = mImpl->renderers.Count();
  for( auto i = 0u; i < rendererCount; ++i )
  {
    //Apply constraints
    auto renderer = mImpl->renderers[i];
    ConstrainPropertyOwner( *renderer, bufferIndex );
  }
}

void UpdateManager::PrepareNodes( BufferIndex updateBufferIndex )
{
  for( auto node : mImpl->nodes )
  {
    if(node)
    {
      node->PrepareRender( updateBufferIndex );
    }
  }
}

void UpdateManager::UploadTexture( Texture* texture,
                                   PixelDataPtr pixelData,
                                   const Internal::Texture::UploadParams& params )
{
  mImpl->textureUploadRequestContainer.emplace_back( texture, pixelData, params );
}

void UpdateManager::UpdateNodes( BufferIndex bufferIndex )
{
  mImpl->nodeDirtyFlags = NodePropertyFlags::NOTHING;

  for( auto&& rootLayer : mImpl->roots )
  {
    // Prepare resources, update shaders, for each node
    // And add the renderers to the sorted layers. Start from root, which is also a layer
    mImpl->nodeDirtyFlags |= UpdateNodeTree( *rootLayer,
                                            bufferIndex );
  }
}

uint32_t UpdateManager::Update( float elapsedSeconds,
                                uint32_t lastVSyncTimeMilliseconds,
                                uint32_t nextVSyncTimeMilliseconds,
                                bool renderToFboEnabled,
                                bool isRenderingToFbo )
{
  const BufferIndex bufferIndex = mSceneGraphBuffers.GetUpdateBufferIndex();

  //Clear nodes/resources which were previously discarded
  mImpl->discardQueue.Clear( bufferIndex );

  //Process Touches & Gestures
  const bool gestureUpdated = ProcessGestures( bufferIndex, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds );
  const bool resumed = mImpl->graphicsController.IsDrawOnResumeRequired();

  bool updateScene =                                   // The scene-graph requires an update if..
    (mImpl->nodeDirtyFlags & RenderableUpdateFlags) || // ..nodes were dirty in previous frame OR
    IsAnimationRunning()                            || // ..at least one animation is running OR
    mImpl->messageQueue.IsSceneUpdateRequired()     || // ..a message that modifies the scene graph node tree is queued OR
    gestureUpdated;                                    // ..a gesture property was updated

  bool keepRendererRendering = false;
  // Although the scene-graph may not require an update, we still need to synchronize
  // double-buffered values if the scene was updated in the previous frame.
  if( updateScene || mImpl->previousUpdateScene )
  {
    //Reset properties from the previous update
    ResetProperties( bufferIndex );
    mImpl->transformManager.ResetToBaseValue();
  }

  // Process the queued scene messages. Note, MessageQueue::FlushQueue may be called between calling
  // IsSceneUpdateRequired() above and here, so updateScene should be set again
  updateScene |= mImpl->messageQueue.ProcessMessages( bufferIndex );
  updateScene |= resumed;

  // Temporarily, switch between single and multithreaded texture
  // DALI_DISABLE_PARALLEL_TEXTURE_UPLOAD - disables running texture processing on the worker thread
  // DALI_DISABLE_SINGLE_STAGING_BUFFER - disables allocating single staging buffer
  auto processParallel = (getenv( "DALI_DISABLE_PARALLEL_TEXTURE_UPLOAD" ) && getenv( "DALI_DISABLE_PARALLEL_TEXTURE_UPLOAD" )[0] == '1' ) ? false : true;
  auto singleStagingBuffer = (getenv( "DALI_DISABLE_SINGLE_STAGING_BUFFER" ) && getenv( "DALI_DISABLE_SINGLE_STAGING_BUFFER" )[0] == '1' ) ? false : true;

  auto future = mImpl->ProcessTextureUploadRequests( processParallel, singleStagingBuffer  );

  // Although the scene-graph may not require an update, we still need to synchronize
  // double-buffered renderer lists if the scene was updated in the previous frame.  We should not
  // start skipping update steps or reusing lists until there have been two frames where nothing
  // changes.
  if( future || updateScene || mImpl->previousUpdateScene )
  {
    //Animate
    Animate( bufferIndex, elapsedSeconds );

    //Constraint custom objects
    ConstrainCustomObjects( bufferIndex );

    //Clear the lists of renderers from the previous update
    for( auto sortedLayers : mImpl->sortedLayerLists )
    {
      for( auto&& layer : sortedLayers )
      {
        layer->ClearRenderables();
      }
    }

    // Call the frame-callback-processor if set
    if( mImpl->frameCallbackProcessor )
    {
      mImpl->frameCallbackProcessor->Update( bufferIndex, elapsedSeconds );
    }

    //Update node hierarchy, apply constraints and perform sorting / culling.
    //This will populate each Layer with a list of renderers which are ready.
    UpdateNodes( bufferIndex );

    //Apply constraints to RenderTasks, shaders
    ConstrainRenderTasks( bufferIndex );
    ConstrainShaders( bufferIndex );

    //Update renderers and apply constraints
    UpdateRenderers( bufferIndex );

    //Update the transformations of all the nodes
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
      // Calculate how many render tasks we have in total
      VectorBase::SizeType numberOfRenderTasks = 0;

      const VectorBase::SizeType taskListCount = mImpl->taskLists.Count();
      for ( VectorBase::SizeType index = 0u; index < taskListCount; index++ )
      {
        numberOfRenderTasks += mImpl->taskLists[index]->GetTasks().Count();
      }

      mImpl->renderInstructions.ResetAndReserve( bufferIndex,
                                                 static_cast<uint32_t>( numberOfRenderTasks ) );

      for ( VectorBase::SizeType index = 0u; index < taskListCount; index++ )
      {
        if ( NULL != mImpl->roots[index] )
        {
          keepRendererRendering |= mImpl->renderTaskProcessor.Process( bufferIndex,
                                              *mImpl->taskLists[index],
                                              *mImpl->roots[index],
                                              mImpl->sortedLayerLists[index],
                                              mImpl->renderInstructions,
                                              renderToFboEnabled,
                                              isRenderingToFbo );
        }
      }

      // generate graphics objects
      PrepareNodes( bufferIndex );
      if( future )
      {
        future->Wait();
        future.reset();
      }
      mImpl->graphicsAlgorithms.ResetCommandBuffer();
      mImpl->graphicsAlgorithms.RenderScene(mImpl->defaultScene.Get(), bufferIndex, true);
      mImpl->graphicsAlgorithms.RenderScene(mImpl->defaultScene.Get(), bufferIndex, false);
    }
  }

  for( auto taskList : mImpl->taskLists )
  {
    RenderTaskList::RenderTaskContainer& tasks = taskList->GetTasks();

    // check the countdown and notify
    bool doRenderOnceNotify = false;
    mImpl->renderTaskWaiting = false;
    for ( auto&& renderTask : tasks )
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
      mImpl->notificationManager.QueueCompleteNotification( taskList->GetCompleteNotificationInterface() );
    }
  }

  // Macro is undefined in release build.
  SNAPSHOT_NODE_LOGGING;

  // A ResetProperties() may be required in the next frame
  mImpl->previousUpdateScene = updateScene;

  // Check whether further updates are required
  uint32_t keepUpdating = KeepUpdatingCheck( elapsedSeconds );

  // we need new flag to rerun update on idle and execute potential idle code
  if( updateScene )
  {
    keepUpdating |= KeepUpdating::DISCARD_RESOURCES;
  }


  if( keepRendererRendering )
  {
    keepUpdating |= KeepUpdating::STAGE_KEEP_RENDERING;
  }

  // tell the update manager that we're done so the queue can be given to event thread
  mImpl->notificationManager.UpdateCompleted();

  // The update has finished; swap the double-buffering indices
  mSceneGraphBuffers.Swap();

  //mImpl->graphicsController.SwapBuffers();

  // Clear texture upload requests
  if( !mImpl->textureUploadRequestContainer.empty() )
  {
    if( future )
    {
      future->Wait();
      future.reset();
    }
  }

  // Clear the texture upload requests container
  mImpl->textureUploadRequestContainer.clear();
  return keepUpdating;
}

uint32_t UpdateManager::KeepUpdatingCheck( float elapsedSeconds ) const
{
  // Update the duration set via Stage::KeepRendering()
  if ( mImpl->keepRenderingSeconds > 0.0f )
  {
    mImpl->keepRenderingSeconds -= elapsedSeconds;
  }

  uint32_t keepUpdatingRequest = KeepUpdating::NOT_REQUESTED;

  // If the rendering behavior is set to continuously render, then continue to render.
  // If Stage::KeepRendering() has been called, then continue until the duration has elapsed.
  // Keep updating until no messages are received and no animations are running.
  // If an animation has just finished, update at least once more for Discard end-actions.
  // No need to check for renderQueue as there is always a render after update and if that
  // render needs another update it will tell the adaptor to call update again

  if ( ( mImpl->renderingBehavior == DevelStage::Rendering::CONTINUOUSLY ) ||
       ( mImpl->keepRenderingSeconds > 0.0f ) )
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

  const BufferIndex bufferIndex = mSceneGraphBuffers.GetUpdateBufferIndex();
  if( !mImpl->discardQueue.IsEmpty( bufferIndex ) ||
      !mImpl->graphicsController.IsDiscardQueueEmpty() )
  {
    keepUpdatingRequest |= KeepUpdating::DISCARD_RESOURCES;
  }

  return keepUpdatingRequest;
}

void UpdateManager::SetBackgroundColor( const Vector4& color )
{
  // not in use
}

void UpdateManager::SetDefaultSurfaceRect( const Rect<int32_t>& rect )
{
  mImpl->surfaceRectChanged = true;

  DALI_ASSERT_ALWAYS( true && "GRAPHICS: FIXME" );
}

void UpdateManager::KeepRendering( float durationSeconds )
{
  mImpl->keepRenderingSeconds = std::max( mImpl->keepRenderingSeconds, durationSeconds );
}

void UpdateManager::SetRenderingBehavior( DevelStage::Rendering renderingBehavior )
{
  mImpl->renderingBehavior = renderingBehavior;
}

void UpdateManager::SetLayerDepths( const SortedLayerPointers& layers, const Layer* rootLayer )
{
  const VectorBase::SizeType rootCount = mImpl->roots.Count();

  // Make sure we reserve the correct size for the container so that
  // we can save the sorted layers in the same order as the root layer
  mImpl->sortedLayerLists.resize( rootCount );

  for ( VectorBase::SizeType rootIndex = 0u; rootIndex < rootCount; rootIndex++ )
  {
    Layer* root = mImpl->roots[rootIndex];
    if ( root == rootLayer )
    {
      mImpl->sortedLayerLists[rootIndex] = layers;
      break;
    }
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

  for( auto root : mImpl->roots )
  {
    // Go through node hierarchy and rearrange siblings according to depth-index
    SortSiblingNodesRecursively( *root );
  }
}

bool UpdateManager::IsDefaultSurfaceRectChanged()
{
  bool surfaceRectChanged = mImpl->surfaceRectChanged;

  // Reset the flag
  mImpl->surfaceRectChanged = false;

  return surfaceRectChanged;
}

void UpdateManager::AddFrameCallback( OwnerPointer< FrameCallback >& frameCallback, const Node* rootNode )
{
  mImpl->GetFrameCallbackProcessor( *this ).AddFrameCallback( frameCallback, rootNode );
}

void UpdateManager::RemoveFrameCallback( FrameCallbackInterface* frameCallback )
{
  mImpl->GetFrameCallbackProcessor( *this ).RemoveFrameCallback( frameCallback );
}

void UpdateManager::AddSampler( OwnerPointer< SceneGraph::Sampler >& sampler )
{
  sampler->Initialize( mImpl->graphicsController );
  mImpl->samplerContainer.PushBack( sampler.Release() );
}

void UpdateManager::RemoveSampler( SceneGraph::Sampler* sampler )
{
  mImpl->samplerContainer.EraseObject( sampler );
}

void UpdateManager::AddPropertyBuffer( OwnerPointer< SceneGraph::PropertyBuffer >& propertyBuffer )
{
  propertyBuffer->Initialize( mImpl->graphicsController );
  mImpl->propertyBufferContainer.PushBack( propertyBuffer.Release() );
}

void UpdateManager::RemovePropertyBuffer( SceneGraph::PropertyBuffer* propertyBuffer )
{
  mImpl->propertyBufferContainer.EraseObject( propertyBuffer );
}

void UpdateManager::AddGeometry( OwnerPointer< SceneGraph::Geometry >& geometry )
{
  geometry->Initialize( mImpl->graphicsController );
  mImpl->geometryContainer.PushBack( geometry.Release() );
}

void UpdateManager::RemoveGeometry( SceneGraph::Geometry* geometry )
{
  mImpl->geometryContainer.EraseObject( geometry );
}

void UpdateManager::AddTexture( OwnerPointer< SceneGraph::Texture >& texture )
{
  texture->Initialize( mImpl->graphicsController );
  mImpl->textureContainer.PushBack( texture.Release() );
}

void UpdateManager::RemoveTexture( SceneGraph::Texture* texture)
{
  DALI_ASSERT_DEBUG( NULL != texture );

  // Remove upload requests to this texture if exists ( there may be more than one )
  if( !mImpl->textureUploadRequestContainer.empty() )
  {
    decltype(mImpl->textureUploadRequestContainer) newRequests;
    for( auto& request : mImpl->textureUploadRequestContainer )
    {
      if( request.texture != texture )
      {
        newRequests.emplace_back( request );
      }
    }
    mImpl->textureUploadRequestContainer = std::move( newRequests );
  }

  // Find the texture, use reference to pointer so we can do the erase safely
  for ( auto&& iter : mImpl->textureContainer )
  {
    if ( iter == texture )
    {
      mImpl->textureContainer.Erase( &iter ); // Texture found; now destroy it
      return;
    }
  }
}

void UpdateManager::AddFrameBuffer( OwnerPointer< SceneGraph::FrameBuffer>& frameBuffer )
{
  frameBuffer->Initialize( mImpl->graphicsController );
  mImpl->frameBufferContainer.PushBack( frameBuffer.Release() );
}

void UpdateManager::RemoveFrameBuffer( SceneGraph::FrameBuffer* frameBuffer)
{
  DALI_ASSERT_DEBUG( NULL != frameBuffer );

  // Find the sampler, use reference so we can safely do the erase
  for ( auto&& iter : mImpl->frameBufferContainer )
  {
    if ( iter == frameBuffer )
    {
      mImpl->frameBufferContainer.Erase( &iter ); // frameBuffer found; now destroy it
      break;
    }
  }
}

void UpdateManager::DestroyGraphicsObjects()
{
  // Wait for the current frame to finish drawing
  mImpl->graphicsController.WaitIdle();

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Destroying graphics objects\n");

  // There will be no further Update after this point. Destroy everything!

  mImpl->renderInstructions.Shutdown(); //Prevent further access of dangling renderer ptrs

  for( auto& geometry : mImpl->geometryContainer )
  {
    geometry->DestroyGraphicsObjects();
  }

  for( auto& propertyBuffer : mImpl->propertyBufferContainer )
  {
    propertyBuffer->DestroyGraphicsObjects();
  }

  for( auto& texture : mImpl->textureContainer )
  {
    texture->DestroyGraphicsObjects();
  }

  for( auto& sampler : mImpl->samplerContainer )
  {
    sampler->DestroyGraphicsObjects();
  }

  for( auto& frameBuffer : mImpl->frameBufferContainer )
  {
    frameBuffer->DestroyGraphicsObjects();
  }

  for( auto& shader : mImpl->shaders )
  {
    shader->DestroyGraphicsObjects();
  }

  mImpl->shaderCache.DestroyGraphicsObjects();

  mImpl->graphicsShutdown = true;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Destruction complete\n");
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
