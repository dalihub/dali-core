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
#include <dali/internal/update/manager/update-manager.h>

// EXTERNAL INCLUDES
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/devel-api/common/set-wrapper.h>
#else
#include <unordered_map>
#include <unordered_set>
#endif

// INTERNAL INCLUDES
#include <dali/integration-api/core.h>
#include <dali/integration-api/trace.h>

#include <dali/internal/common/owner-key-container.h>

#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/common/property-notifier.h>
#include <dali/internal/event/effects/shader-factory.h>

#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller-impl.h>
#include <dali/internal/update/manager/frame-callback-processor.h>
#include <dali/internal/update/manager/render-task-processor.h>
#include <dali/internal/update/manager/resetter-container.h>
#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/manager/update-algorithms.h>
#include <dali/internal/update/manager/update-manager-debug.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/queue/update-message-queue.h>

#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-uniform-block.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>

// Un-comment to enable node tree debug logging
// #define NODE_TREE_LOGGING 1

#if(defined(DEBUG_ENABLED) && defined(NODE_TREE_LOGGING))
#define SNAPSHOT_NODE_LOGGING                                                   \
  const uint32_t FRAME_COUNT_TRIGGER = 16;                                      \
  if(mImpl->frameCounter >= FRAME_COUNT_TRIGGER)                                \
  {                                                                             \
    for(auto&& scene : mImpl->scenes)                                           \
    {                                                                           \
      if(scene && scene->root)                                                  \
      {                                                                         \
        mImpl->frameCounter = 0;                                                \
        PrintNodes(*scene->root, mSceneGraphBuffers.GetUpdateBufferIndex(), 0); \
      }                                                                         \
    }                                                                           \
  }                                                                             \
  mImpl->frameCounter++;
#else
#define SNAPSHOT_NODE_LOGGING
#endif

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gRenderTaskLogFilter;
namespace
{
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_UPDATE_MANAGER");
} // unnamed namespace
#endif
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_UPDATE_PROCESS, false);
} // namespace

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
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
/**
 * Flag whether property has changed, during the Update phase.
 */
enum ContainerRemovedFlagBits
{
  NOTHING               = 0x00,
  NODE                  = 0x01,
  RENDERER              = 0x02,
  SHADER                = 0x04,
  TEXTURE_SET           = 0x08,
  ANIMATION             = 0x10,
  PROPERTY_NOTIFICATION = 0x20,
  CUSTOM_OBJECT         = 0x40,
  UNIFORM_BLOCKS        = 0x80,
};

/**
 * @brief ContainerRemovedFlags alters behaviour of implementation
 */
using ContainerRemovedFlags = uint8_t;
#endif
/**
 * Helper to Erase an object from OwnerContainer using discard queue
 * @param container to remove from
 * @param object to remove
 * @param discardQueue to put the object to
 * @param updateBufferIndex to use
 */
template<class Type>
inline void EraseUsingDiscardQueue(OwnerContainer<Type*>& container, Type* object, DiscardQueue<Type*, OwnerContainer<Type*>>& discardQueue, BufferIndex updateBufferIndex)
{
  DALI_ASSERT_DEBUG(object && "NULL object not allowed");

  // need to use the reference version of auto as we need the pointer to the pointer for the Release call below
  for(auto&& iter : container)
  {
    if(iter == object)
    {
      // Transfer ownership to the discard queue, this keeps the object alive, until the render-thread has finished with it
      discardQueue.Add(updateBufferIndex, container.Release(&iter)); // take the address of the reference to a pointer (iter)
      return;                                                        // return as we only ever remove one object. Iterators to container are now invalidated as well so cannot continue
    }
  }
}

/**
 * Helper to Erase an object from std::vector using discard queue
 * @param container to remove from
 * @param object to remove
 * @param discardQueue to put the object to
 * @param updateBufferIndex to use
 */
template<class Type>
inline void EraseUsingDiscardQueue(OwnerKeyContainer<Type>& container, const MemoryPoolKey<Type>& key, DiscardQueue<MemoryPoolKey<Type>, OwnerKeyContainer<Type>>& discardQueue, BufferIndex updateBufferIndex)
{
  DALI_ASSERT_DEBUG(key && "INVALID Key not allowed");

  for(auto iter = container.begin(), end = container.end(); iter != end; ++iter)
  {
    if(*iter == key)
    {
      // Transfer ownership to the discard queue, this keeps the object alive, until the render-thread has finished with it
      discardQueue.Add(updateBufferIndex, container.Release(iter));
      return; // return as we only ever remove one object. Iterators to container are now invalidated as well so cannot continue
    }
  }
}

} // unnamed namespace

/**
 * Structure to contain UpdateManager internal data
 */
struct UpdateManager::Impl
{
  // SceneInfo keeps the root node of the Scene, its scene graph render task list, and the list of Layer pointers sorted by depth
  struct SceneInfo
  {
    SceneInfo(Layer* root) ///< Constructor
    : root(root)
    {
    }

    ~SceneInfo()                               = default; ///< Default non-virtual destructor
    SceneInfo(SceneInfo&& rhs)                 = default; ///< Move constructor
    SceneInfo& operator=(SceneInfo&& rhs)      = default; ///< Move assignment operator
    SceneInfo& operator=(const SceneInfo& rhs) = delete;  ///< Assignment operator
    SceneInfo(const SceneInfo& rhs)            = delete;  ///< Copy constructor

    Layer*                       root{nullptr};   ///< Root node (root is a layer). The layer is not stored in the node memory pool.
    OwnerPointer<RenderTaskList> taskList;        ///< Scene graph render task list
    SortedLayerPointers          sortedLayerList; ///< List of Layer pointers sorted by depth (one list of sorted layers per root)
    OwnerPointer<Scene>          scene;           ///< Scene graph object of the scene
  };

  Impl(NotificationManager&           notificationManager,
       CompleteNotificationInterface& animationPlaylist,
       PropertyNotifier&              propertyNotifier,
       RenderController&              renderController,
       RenderManager&                 renderManager,
       RenderQueue&                   renderQueue,
       SceneGraphBuffers&             sceneGraphBuffers,
       RenderTaskProcessor&           renderTaskProcessor)
  : renderMessageDispatcher(renderManager, renderQueue, sceneGraphBuffers),
    notificationManager(notificationManager),
    transformManager(),
    animationPlaylist(animationPlaylist),
    propertyNotifier(propertyNotifier),
    shaderSaver(nullptr),
    renderController(renderController),
    sceneController(nullptr),
    renderManager(renderManager),
    renderQueue(renderQueue),
    renderTaskProcessor(renderTaskProcessor),
    backgroundColor(Dali::DEFAULT_BACKGROUND_COLOR),
    renderers(),
    textureSets(),
    shaders(),
    uniformBlocks(),
    panGestureProcessor(nullptr),
    messageQueue(renderController, sceneGraphBuffers),
    frameCallbackProcessor(nullptr),
    nodeDirtyFlags(NodePropertyFlags::TRANSFORM), // set to TransformFlag to ensure full update the first time through Update()
    frameCounter(0),
    renderingBehavior(DevelStage::Rendering::IF_REQUIRED),
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    containerRemovedFlags(ContainerRemovedFlagBits::NOTHING),
#endif
    discardAnimationFinishedAge(0u),
    animationFinishedDuringUpdate(false),
    previousUpdateScene(false),
    renderTaskWaiting(false),
    renderersAdded(false),
    renderingRequired(false)
  {
    sceneController = new SceneControllerImpl(renderMessageDispatcher, renderQueue);

    // create first 'dummy' node
    nodes.PushBack(nullptr);
  }

  ~Impl()
  {
    // Disconnect render tasks from nodes, before destroying the nodes
    for(auto&& scene : scenes)
    {
      if(scene && scene->taskList)
      {
        RenderTaskList::RenderTaskContainer& tasks = scene->taskList->GetTasks();
        for(auto&& task : tasks)
        {
          task->SetSourceNode(nullptr);
        }
      }
    }

    // UpdateManager owns the Nodes. Although Nodes are pool allocated they contain heap allocated parts
    // like custom properties, which get released here
    Vector<Node*>::Iterator iter    = nodes.Begin() + 1;
    Vector<Node*>::Iterator endIter = nodes.End();
    for(; iter != endIter; ++iter)
    {
      (*iter)->OnDestroy();
      Node::Delete(*iter);
    }
    nodeIdMap.clear();

    for(auto&& scene : scenes)
    {
      if(scene && scene->root)
      {
        scene->root->OnDestroy();
        Node::Delete(scene->root);
      }
    }
    scenes.clear();

    delete sceneController;

    // Ensure to clear renderers
    renderers.Clear();
    shaders.Clear();
    uniformBlocks.Clear();
  }

  /**
   * Lazy init for FrameCallbackProcessor.
   * @param[in]  updateManager  A reference to the update-manager
   */
  FrameCallbackProcessor& GetFrameCallbackProcessor(UpdateManager& updateManager)
  {
    if(!frameCallbackProcessor)
    {
      frameCallbackProcessor = new FrameCallbackProcessor(updateManager, transformManager);
    }
    return *frameCallbackProcessor;
  }

  /**
   * @brief Remove all owned render context at scene.
   *
   * @note Should be called at ContextDestroyed case.
   */
  void ContextDestroyed()
  {
    // Disconnect render tasks from nodes, before destroying the nodes
    for(auto&& scene : scenes)
    {
      if(scene)
      {
        if(scene->scene)
        {
          scene->scene->ContextDestroyed();
        }
        if(scene->taskList)
        {
          RenderTaskList::RenderTaskContainer& tasks = scene->taskList->GetTasks();
          for(auto&& task : tasks)
          {
            task->ContextDestroyed();
          }
        }
      }
    }
  }

  SceneGraphBuffers              sceneGraphBuffers;       ///< Used to keep track of which buffers are being written or read
  RenderMessageDispatcher        renderMessageDispatcher; ///< Used for passing messages to the render-thread
  NotificationManager&           notificationManager;     ///< Queues notification messages for the event-thread.
  TransformManager               transformManager;        ///< Used to update the transformation matrices of the nodes
  CompleteNotificationInterface& animationPlaylist;       ///< Holds handles to all the animations
  PropertyNotifier&              propertyNotifier;        ///< Provides notification to applications when properties are modified.
  ShaderSaver*                   shaderSaver;             ///< Saves shader binaries.
  RenderController&              renderController;        ///< render controller
  SceneControllerImpl*           sceneController;         ///< scene controller
  RenderManager&                 renderManager;           ///< This is responsible for rendering the results of each "update"
  RenderQueue&                   renderQueue;             ///< Used to queue messages for the next render
  RenderTaskProcessor&           renderTaskProcessor;     ///< Handles RenderTasks and RenderInstrucitons

  Vector4 backgroundColor; ///< The glClear color used at the beginning of each frame.

  using SceneInfoPtr = std::unique_ptr<SceneInfo>;
  std::vector<SceneInfoPtr> scenes; ///< A container of SceneInfo.

  Vector<Node*> nodes; ///< A container of all instantiated nodes

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  using NodeIdMap = std::map<uint32_t, Node*>;

  using PropertyBaseResetRequestedContainer = std::set<PropertyBase*>;
#else
  using NodeIdMap = std::unordered_map<uint32_t, Node*>;

  using PropertyBaseResetRequestedContainer = std::unordered_set<PropertyBase*>;
#endif
  NodeIdMap nodeIdMap; ///< A container of nodes map by id.

  Vector<Camera*> cameras; ///< A container of cameras. Note : these cameras are owned by Impl::nodes.

  OwnerContainer<PropertyOwner*> customObjects; ///< A container of owned objects (with custom properties)

  ResetterContainer<PropertyResetterBase> propertyResetters; ///< A container of property resetters
  ResetterContainer<NodeResetter>         nodeResetters;     ///< A container of node resetters
  ResetterContainer<RendererResetter>     rendererResetters; ///< A container of renderer resetters

  PropertyBaseResetRequestedContainer resetRequestedPropertyBases; ///< A container of property base to be resets

  OwnerContainer<Animation*>            animations;            ///< A container of owned animations
  OwnerContainer<PropertyNotification*> propertyNotifications; ///< A container of owner property notifications.
  OwnerKeyContainer<Renderer>           renderers;             ///< A container of owned renderers
  OwnerContainer<TextureSet*>           textureSets;           ///< A container of owned texture sets
  OwnerContainer<Shader*>               shaders;               ///< A container of owned shaders
  OwnerContainer<Render::UniformBlock*> uniformBlocks;         ///< A container of owned uniformBlocks

  DiscardQueue<Node*, OwnerContainer<Node*>>                                 nodeDiscardQueue; ///< Nodes are added here when disconnected from the scene-graph.
  DiscardQueue<Shader*, OwnerContainer<Shader*>>                             shaderDiscardQueue;
  DiscardQueue<Render::UniformBlock*, OwnerContainer<Render::UniformBlock*>> uniformBlockDiscardQueue;
  DiscardQueue<MemoryPoolKey<Renderer>, OwnerKeyContainer<Renderer>>         rendererDiscardQueue;
  DiscardQueue<Scene*, OwnerContainer<Scene*>>                               sceneDiscardQueue;

  CompleteNotificationInterface::ParameterList notifyRequiredAnimations; ///< A temperal container of complete notify required animations, like animation finished, stopped, or loop completed.

  OwnerPointer<PanGesture> panGestureProcessor; ///< Owned pan gesture processor; it lives for the lifecycle of UpdateManager

  MessageQueue                         messageQueue;          ///< The messages queued from the event-thread
  std::vector<Internal::ShaderDataPtr> renderCompiledShaders; ///< Shaders compiled on Render thread are inserted here for update thread to pass on to event thread.
  std::vector<Internal::ShaderDataPtr> updateCompiledShaders; ///< Shaders to be sent from Update to Event
  Mutex                                compiledShaderMutex;   ///< lock to ensure no corruption on the renderCompiledShaders

  OwnerPointer<FrameCallbackProcessor> frameCallbackProcessor; ///< Owned FrameCallbackProcessor, only created if required.

  std::atomic<std::size_t> renderInstructionCapacity{0u};
  NodePropertyFlags        nodeDirtyFlags;    ///< cumulative node dirty flags from previous frame
  uint32_t                 frameCounter;      ///< Frame counter used in debugging to choose which frame to debug and which to ignore.
  DevelStage::Rendering    renderingBehavior; ///< Set via DevelStage::SetRenderingBehavior

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  ContainerRemovedFlags containerRemovedFlags; ///< cumulative container removed flags during current frame
#endif

  uint8_t discardAnimationFinishedAge : 2; ///< Age of EndAction::Discard animation Stop/Finished. It will make we call ResetToBaseValue at least 2 frames.

  bool animationFinishedDuringUpdate : 1; ///< Flag whether any animations finished during the Update()
  bool previousUpdateScene : 1;           ///< True if the scene was updated in the previous frame (otherwise it was optimized out)
  bool renderTaskWaiting : 1;             ///< A REFRESH_ONCE render task is waiting to be rendered
  bool renderersAdded : 1;                ///< Flag to keep track when renderers have been added to avoid unnecessary processing
  bool renderingRequired : 1;             ///< True if required to render the current frame

private:
  Impl(const Impl&);            ///< Undefined
  Impl& operator=(const Impl&); ///< Undefined
};

UpdateManager::UpdateManager(NotificationManager&           notificationManager,
                             CompleteNotificationInterface& animationFinishedNotifier,
                             PropertyNotifier&              propertyNotifier,
                             RenderController&              controller,
                             RenderManager&                 renderManager,
                             RenderQueue&                   renderQueue,
                             RenderTaskProcessor&           renderTaskProcessor)
: mImpl(nullptr)
{
  PropertyBase::RegisterResetterManager(*this);

  mImpl = new Impl(notificationManager,
                   animationFinishedNotifier,
                   propertyNotifier,
                   controller,
                   renderManager,
                   renderQueue,
                   mSceneGraphBuffers,
                   renderTaskProcessor);
}

UpdateManager::~UpdateManager()
{
  delete mImpl;
  PropertyBase::UnregisterResetterManager();

  // Ensure to release memory pool
  Animation::ResetMemoryPool();
  Camera::ResetMemoryPool();
  Node::ResetMemoryPool();
  Renderer::ResetMemoryPool();
  RenderItem::ResetMemoryPool();
  RenderTaskList::ResetMemoryPool();
  TextureSet::ResetMemoryPool();
}

void UpdateManager::ContextDestroyed()
{
  // Remove owned update context
  mImpl->ContextDestroyed();
}

void UpdateManager::InstallRoot(OwnerPointer<Layer>& layer)
{
  DALI_ASSERT_DEBUG(layer->IsLayer());
  DALI_ASSERT_DEBUG(layer->GetParent() == NULL);

  Layer* rootLayer = layer.Release();

  DALI_ASSERT_DEBUG(std::find_if(mImpl->scenes.begin(), mImpl->scenes.end(), [rootLayer](Impl::SceneInfoPtr& scene)
                                 { return scene && scene->root == rootLayer; }) == mImpl->scenes.end() &&
                    "Root Node already installed");

  rootLayer->CreateTransform(&mImpl->transformManager);
  rootLayer->SetRoot(true);

  rootLayer->AddInitializeResetter(*this);

  // Do not allow to insert duplicated nodes.
  // It could be happened if node id is overflowed.
  DALI_ASSERT_ALWAYS(mImpl->nodeIdMap.insert({rootLayer->GetId(), rootLayer}).second);

  mImpl->scenes.emplace_back(new Impl::SceneInfo(rootLayer));
}

void UpdateManager::UninstallRoot(Layer* layer)
{
  DALI_ASSERT_DEBUG(layer->IsLayer());
  DALI_ASSERT_DEBUG(layer->GetParent() == NULL);

  for(auto iter = mImpl->scenes.begin(); iter != mImpl->scenes.end(); ++iter)
  {
    if((*iter) && (*iter)->root == layer)
    {
      mImpl->scenes.erase(iter);
      break;
    }
  }

  mImpl->nodeIdMap.erase(layer->GetId());

  mImpl->nodeDiscardQueue.Add(mSceneGraphBuffers.GetUpdateBufferIndex(), layer);

  // Notify the layer about impending destruction
  layer->OnDestroy();
}

void UpdateManager::AddNode(OwnerPointer<Node>& node)
{
  DALI_ASSERT_ALWAYS(nullptr == node->GetParent()); // Should not have a parent yet

  Node* rawNode = node.Release();
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] AddNode\n", rawNode);

  // Add camera nodes occured rarely.
  if(DALI_UNLIKELY(rawNode->IsCamera()))
  {
    AddCamera(static_cast<Camera*>(rawNode));
  }

  // Do not allow to insert duplicated nodes.
  // It could be happened if node id is overflowed.
  DALI_ASSERT_ALWAYS(mImpl->nodeIdMap.insert({rawNode->GetId(), rawNode}).second);

  mImpl->nodes.PushBack(rawNode);

  rawNode->CreateTransform(&mImpl->transformManager);
}

void UpdateManager::ConnectNode(Node* parent, Node* node)
{
  DALI_ASSERT_ALWAYS(nullptr != parent);
  DALI_ASSERT_ALWAYS(nullptr != node);
  DALI_ASSERT_ALWAYS(nullptr == node->GetParent()); // Should not have a parent yet

  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] ConnectNode\n", node);

  parent->SetDirtyFlag(NodePropertyFlags::DESCENDENT_HIERARCHY_CHANGED); // make parent dirty so that render items dont get reused
  parent->ConnectChild(node);

  node->AddInitializeResetter(*this);

  // Inform the frame-callback-processor, if set, about the node-hierarchy changing
  if(mImpl->frameCallbackProcessor)
  {
    mImpl->frameCallbackProcessor->NodeHierarchyChanged();
  }
}

void UpdateManager::DisconnectNode(Node* node)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] DisconnectNode\n", node);

  Node* parent = node->GetParent();
  DALI_ASSERT_ALWAYS(nullptr != parent);
  parent->SetDirtyFlag(NodePropertyFlags::CHILD_DELETED | NodePropertyFlags::DESCENDENT_HIERARCHY_CHANGED); // make parent dirty so that render items dont get reused

  parent->DisconnectChild(mSceneGraphBuffers.GetUpdateBufferIndex(), *node);

  // Inform the frame-callback-processor, if set, about the node-hierarchy changing
  if(mImpl->frameCallbackProcessor)
  {
    mImpl->frameCallbackProcessor->NodeHierarchyChanged();
  }
}

void UpdateManager::DestroyNode(Node* node)
{
  DALI_ASSERT_ALWAYS(nullptr != node);
  DALI_ASSERT_ALWAYS(nullptr == node->GetParent()); // Should have been disconnected

  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] DestroyNode\n", node);

  Vector<Node*>::Iterator iter    = mImpl->nodes.Begin() + 1;
  Vector<Node*>::Iterator endIter = mImpl->nodes.End();
  for(; iter != endIter; ++iter)
  {
    if((*iter) == node)
    {
      mImpl->nodes.Erase(iter);
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
      mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::NODE;
#endif
      break;
    }
  }

  // Remove camera nodes occured rarely.
  if(DALI_UNLIKELY(node->IsCamera()))
  {
    RemoveCamera(static_cast<Camera*>(node));
  }

  mImpl->nodeIdMap.erase(node->GetId());

  mImpl->nodeDiscardQueue.Add(mSceneGraphBuffers.GetUpdateBufferIndex(), node);

  // Notify the Node about impending destruction
  node->OnDestroy();
}

void UpdateManager::AddCamera(Camera* camera)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] AddCamera\n", camera);

  mImpl->cameras.PushBack(camera);
}

void UpdateManager::RemoveCamera(Camera* camera)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] RemoveCamera\n", camera);

  // Find the camera and remove it from list of cameras
  Vector<Camera*>::Iterator iter    = mImpl->cameras.Begin();
  Vector<Camera*>::Iterator endIter = mImpl->cameras.End();
  for(; iter != endIter; ++iter)
  {
    if((*iter) == camera)
    {
      mImpl->cameras.Erase(iter);
      break;
    }
  }
}

void UpdateManager::AddObject(OwnerPointer<PropertyOwner>& object)
{
  mImpl->customObjects.PushBack(object.Release());
}

void UpdateManager::RemoveObject(PropertyOwner* object)
{
  mImpl->customObjects.EraseObject(object);
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::CUSTOM_OBJECT;
#endif
}

void UpdateManager::AddRenderTaskList(OwnerPointer<RenderTaskList>& taskList)
{
  RenderTaskList* taskListPointer = taskList.Release();
  taskListPointer->Initialize(*this, mImpl->renderMessageDispatcher);

  mImpl->scenes.back()->taskList = taskListPointer;
}

void UpdateManager::RemoveRenderTaskList(RenderTaskList* taskList)
{
  for(auto&& scene : mImpl->scenes)
  {
    if(scene && scene->taskList == taskList)
    {
      scene->taskList.Reset();
      break;
    }
  }
}

void UpdateManager::AddScene(OwnerPointer<Scene>& scene)
{
  auto& sceneInfo  = mImpl->scenes.back();
  sceneInfo->scene = scene.Release();

  // Set root to the Scene
  sceneInfo->scene->SetRoot(sceneInfo->root);
  sceneInfo->scene->SetRenderMessageDispatcher(&mImpl->renderMessageDispatcher);

  // Initialize the context from render manager
  typedef MessageValue1<RenderManager, SceneGraph::Scene*> DerivedType;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  SceneGraph::Scene& sceneObject = *sceneInfo->scene;
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::InitializeScene, &sceneObject);
}

void UpdateManager::RemoveScene(Scene* scene)
{
  // Initialize the context from render manager
  using DerivedType = MessageValue1<RenderManager, SceneGraph::Scene*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::UninitializeScene, scene);

  scene->RemoveSurfaceRenderTarget();

  for(auto&& sceneInfo : mImpl->scenes)
  {
    if(sceneInfo && sceneInfo->scene && sceneInfo->scene.Get() == scene)
    {
      mImpl->sceneDiscardQueue.Add(mSceneGraphBuffers.GetUpdateBufferIndex(), sceneInfo->scene.Release()); // take the address of the reference to a pointer
      break;
    }
  }
}

void UpdateManager::AddAnimation(OwnerPointer<SceneGraph::Animation>& animation)
{
  mImpl->animations.PushBack(animation.Release());
}

void UpdateManager::StopAnimation(Animation* animation)
{
  DALI_ASSERT_DEBUG(animation && "NULL animation called to stop");

  bool animationFinished = animation->Stop(mSceneGraphBuffers.GetUpdateBufferIndex());

  mImpl->animationFinishedDuringUpdate = mImpl->animationFinishedDuringUpdate || animationFinished;
}

void UpdateManager::ClearAnimation(Animation* animation)
{
  DALI_ASSERT_DEBUG(animation && "NULL animation called to clear");

  animation->ClearAnimator(mSceneGraphBuffers.GetUpdateBufferIndex());
}

void UpdateManager::RemoveAnimation(Animation* animation)
{
  DALI_ASSERT_DEBUG(animation && "NULL animation called to remove");

  animation->OnDestroy(mSceneGraphBuffers.GetUpdateBufferIndex());

  DALI_ASSERT_DEBUG(animation->GetState() == Animation::Destroyed);

  // Do not remove from container now. Destroyed animation will be removed at Animate.
}

bool UpdateManager::IsAnimationRunning() const
{
  // Find any animation that isn't stopped or paused
  for(auto&& iter : mImpl->animations)
  {
    const Animation::State state = iter->GetState();

    if(state != Animation::Stopped &&
       state != Animation::Paused)
    {
      return true; // stop iteration as soon as first one is found
    }
  }

  return false;
}

void UpdateManager::AddPropertyResetter(OwnerPointer<PropertyResetterBase>& propertyResetter)
{
  propertyResetter->Initialize();
  mImpl->propertyResetters.PushBack(propertyResetter.Release());
}

void UpdateManager::AddNodeResetter(const Node& node)
{
  OwnerPointer<SceneGraph::NodeResetter> nodeResetter = SceneGraph::NodeResetter::New(node);
  nodeResetter->Initialize();
  mImpl->nodeResetters.PushBack(nodeResetter.Release());
}

void UpdateManager::AddRendererResetter(const Renderer& renderer)
{
  OwnerPointer<SceneGraph::RendererResetter> rendererResetter = SceneGraph::RendererResetter::New(renderer);
  rendererResetter->Initialize();
  mImpl->rendererResetters.PushBack(rendererResetter.Release());
}

void UpdateManager::RequestPropertyBaseResetToBaseValue(PropertyBase* propertyBase)
{
  mImpl->resetRequestedPropertyBases.insert(propertyBase);
}

void UpdateManager::AddPropertyNotification(OwnerPointer<PropertyNotification>& propertyNotification)
{
  mImpl->propertyNotifications.PushBack(propertyNotification.Release());
}

void UpdateManager::RemovePropertyNotification(PropertyNotification* propertyNotification)
{
  auto iter = std::find(mImpl->propertyNotifications.Begin(), mImpl->propertyNotifications.End(), propertyNotification);
  if(iter != mImpl->propertyNotifications.End())
  {
    mImpl->propertyNotifications.Erase(iter);
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::PROPERTY_NOTIFICATION;
#endif
  }
}

void UpdateManager::PropertyNotificationSetNotify(PropertyNotification* propertyNotification, PropertyNotification::NotifyMode notifyMode)
{
  DALI_ASSERT_DEBUG(propertyNotification && "propertyNotification scene graph object missing");
  propertyNotification->SetNotifyMode(notifyMode);
}

void UpdateManager::AddShader(OwnerPointer<Shader>& shader)
{
  mImpl->shaders.PushBack(shader.Release());
}

void UpdateManager::RemoveShader(Shader* shader)
{
  // Find the shader and destroy it
  EraseUsingDiscardQueue(mImpl->shaders, shader, mImpl->shaderDiscardQueue, mSceneGraphBuffers.GetUpdateBufferIndex());
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::SHADER;
#endif
}

void UpdateManager::SaveBinary(Internal::ShaderDataPtr shaderData)
{
  DALI_ASSERT_DEBUG(shaderData && "No NULL shader data pointers please.");
  DALI_ASSERT_DEBUG(shaderData->GetBufferSize() > 0 && "Shader binary empty so nothing to save.");
  {
    // lock as update might be sending previously compiled shaders to event thread
    Mutex::ScopedLock lock(mImpl->compiledShaderMutex);
    mImpl->renderCompiledShaders.push_back(shaderData);
  }
}

void UpdateManager::SetShaderSaver(ShaderSaver& upstream)
{
  mImpl->shaderSaver = &upstream;
}

void UpdateManager::AddRenderer(const RendererKey& rendererKey)
{
  SceneGraph::Renderer* renderer = rendererKey.Get();

  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] AddRenderer\n", renderer);

  renderer->ConnectToSceneGraph(*mImpl->sceneController, mSceneGraphBuffers.GetUpdateBufferIndex());
  renderer->AddInitializeResetter(*this);

  mImpl->renderers.PushBack(rendererKey);
}

void UpdateManager::RemoveRenderer(const RendererKey& rendererKey)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%x] RemoveRenderer\n", rendererKey.Get());

  // Find the renderer and destroy it
  EraseUsingDiscardQueue(mImpl->renderers, rendererKey, mImpl->rendererDiscardQueue, mSceneGraphBuffers.GetUpdateBufferIndex());
  // Need to remove the render object as well
  rendererKey->DisconnectFromSceneGraph(*mImpl->sceneController, mSceneGraphBuffers.GetUpdateBufferIndex());

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::RENDERER;
#endif
}

void UpdateManager::AttachRenderer(Node* node, Renderer* renderer)
{
  node->AddRenderer(Renderer::GetKey(renderer));
  mImpl->renderersAdded = true;
}

void UpdateManager::AttachCacheRenderer(Node* node, Renderer* renderer)
{
  node->AddCacheRenderer(Renderer::GetKey(renderer));
  mImpl->renderersAdded = true;
}

void UpdateManager::SetPanGestureProcessor(PanGesture* panGestureProcessor)
{
  DALI_ASSERT_DEBUG(NULL != panGestureProcessor);

  mImpl->panGestureProcessor = panGestureProcessor;
}

void UpdateManager::AddTextureSet(OwnerPointer<TextureSet>& textureSet)
{
  textureSet->SetRenderMessageDispatcher(&mImpl->renderMessageDispatcher);
  mImpl->textureSets.PushBack(textureSet.Release());
}

void UpdateManager::RemoveTextureSet(TextureSet* textureSet)
{
  mImpl->textureSets.EraseObject(textureSet);
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::TEXTURE_SET;
#endif
}

void UpdateManager::AddUniformBlock(OwnerPointer<Render::UniformBlock>& uniformBlock)
{
  mImpl->uniformBlocks.PushBack(uniformBlock.Release());
}

void UpdateManager::RemoveUniformBlock(Render::UniformBlock* uniformBlock)
{
  EraseUsingDiscardQueue(mImpl->uniformBlocks, uniformBlock, mImpl->uniformBlockDiscardQueue, mSceneGraphBuffers.GetUpdateBufferIndex());
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::UNIFORM_BLOCKS;
#endif
}

uint32_t* UpdateManager::ReserveMessageSlot(uint32_t size, bool updateScene)
{
  return mImpl->messageQueue.ReserveMessageSlot(size, updateScene);
}

std::size_t UpdateManager::GetUpdateMessageQueueCapacity() const
{
  return mImpl->messageQueue.GetCapacity();
}

std::size_t UpdateManager::GetRenderMessageQueueCapacity() const
{
  return mImpl->renderQueue.GetCapacity();
}

std::size_t UpdateManager::GetRenderInstructionCapacity() const
{
  return mImpl->renderInstructionCapacity;
}

void UpdateManager::EventProcessingStarted()
{
  mImpl->messageQueue.EventProcessingStarted();
}

void UpdateManager::EventProcessingFinished()
{
  mImpl->messageQueue.EventProcessingFinished();
}

bool UpdateManager::FlushQueue()
{
  return mImpl->messageQueue.FlushQueue();
}

void UpdateManager::ResetProperties(BufferIndex bufferIndex)
{
  // Clear the "animations finished" flag; This should be set if any (previously playing) animation is stopped
  mImpl->animationFinishedDuringUpdate = false;

  // Age down discard animations.
  mImpl->discardAnimationFinishedAge >>= 1;

  // Ensure that their was no request to reset to base values during the previous update
  // (Since requested property base doesn't consider the lifecycle of PropertyBase,
  // It might be invalid after the previous update finished)
  DALI_ASSERT_DEBUG(mImpl->resetRequestedPropertyBases.empty() && "Reset to base values requested during the previous update!");

  // Reset node properties
  mImpl->nodeResetters.RequestResetToBaseValues();

  // Reset renderer properties
  mImpl->rendererResetters.RequestResetToBaseValues();

  // Reset all animating / constrained properties
  mImpl->propertyResetters.RequestResetToBaseValues();

  // Actual reset to base values here
  if(!mImpl->resetRequestedPropertyBases.empty())
  {
    decltype(mImpl->resetRequestedPropertyBases) propertyBaseList;

    // Ensure to reset list as zero capacity (Since std::unordered_set::clear() is slow if capacity is not zero)
    propertyBaseList.swap(mImpl->resetRequestedPropertyBases);

    for(auto&& propertyBase : propertyBaseList)
    {
      propertyBase->ResetToBaseValue(bufferIndex);
    }
  }

  // Clear all root nodes dirty flags
  for(auto& scene : mImpl->scenes)
  {
    auto root = scene->root;
    root->ResetDirtyFlags(bufferIndex);
  }

  // Clear node dirty flags
  Vector<Node*>::Iterator iter    = mImpl->nodes.Begin() + 1;
  Vector<Node*>::Iterator endIter = mImpl->nodes.End();
  for(; iter != endIter; ++iter)
  {
    (*iter)->ResetDirtyFlags(bufferIndex);
  }
}

bool UpdateManager::ProcessGestures(BufferIndex bufferIndex, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds)
{
  bool gestureUpdated(false);

  if(mImpl->panGestureProcessor)
  {
    // gesture processor only supports default properties
    mImpl->panGestureProcessor->ResetDefaultProperties(bufferIndex); // Needs to be done every time as gesture data is written directly to an update-buffer rather than via a message
    gestureUpdated |= mImpl->panGestureProcessor->UpdateProperties(lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds);
  }

  return gestureUpdated;
}

bool UpdateManager::Animate(BufferIndex bufferIndex, float elapsedSeconds)
{
  bool animationActive = false;

  if(mImpl->animations.Empty())
  {
    return animationActive;
  }

  auto&& iter = mImpl->animations.Begin();

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_ANIMATION_ANIMATE", [&](std::ostringstream& oss)
                                          { oss << "[" << mImpl->animations.Count() << "]"; });

  while(iter != mImpl->animations.End())
  {
    Animation* animation             = *iter;
    bool       finished              = false;
    bool       stopped               = false;
    bool       progressMarkerReached = false;
    animation->Update(bufferIndex, elapsedSeconds, stopped, finished, progressMarkerReached);

    animationActive = animationActive || animation->IsActive();

    if(progressMarkerReached)
    {
      mImpl->notificationManager.QueueMessage(Internal::NotifyProgressReachedMessage(mImpl->animationPlaylist, animation->GetNotifyId()));
    }

    mImpl->animationFinishedDuringUpdate = mImpl->animationFinishedDuringUpdate || finished;

    // Check whether finished animation is Discard type. If then, we should update scene at least 2 frames.
    if(finished && animation->GetEndAction() == Animation::EndAction::DISCARD)
    {
      mImpl->discardAnimationFinishedAge |= 2u;
    }

    // queue the notification on finished or stopped
    if(finished || stopped)
    {
      mImpl->notifyRequiredAnimations.PushBack(animation->GetNotifyId());
    }

    // Remove animations that had been destroyed but were still waiting for an update
    if(animation->GetState() == Animation::Destroyed)
    {
      iter = mImpl->animations.Erase(iter);
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
      mImpl->containerRemovedFlags |= ContainerRemovedFlagBits::ANIMATION;
#endif
    }
    else
    {
      ++iter;
    }
  }

  // The application should be notified by NotificationManager, in another thread
  if(!mImpl->notifyRequiredAnimations.Empty())
  {
    mImpl->notificationManager.QueueNotification(&mImpl->animationPlaylist, std::move(mImpl->notifyRequiredAnimations));
  }

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_ANIMATION_ANIMATE", [&](std::ostringstream& oss)
                                        { oss << "[" << mImpl->animations.Count() << "]"; });

  return animationActive;
}

void UpdateManager::ConstrainCustomObjects(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex)
{
  // Constrain custom objects (in construction order)
  for(auto&& object : mImpl->customObjects)
  {
    ConstrainPropertyOwner(*object, bufferIndex);
    if(!object->GetPostConstraints().Empty())
    {
      postPropertyOwners.PushBack(object);
    }
  }
}

void UpdateManager::ConstrainRenderTasks(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex)
{
  // Constrain render-tasks
  for(auto&& scene : mImpl->scenes)
  {
    if(scene && scene->taskList)
    {
      RenderTaskList::RenderTaskContainer& tasks = scene->taskList->GetTasks();
      for(auto&& task : tasks)
      {
        ConstrainPropertyOwner(*task, bufferIndex);
        if(!task->GetPostConstraints().Empty())
        {
          postPropertyOwners.PushBack(task);
        }
      }
    }
  }
}

void UpdateManager::ConstrainShaders(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex)
{
  // constrain shaders... (in construction order)
  for(auto&& shader : mImpl->shaders)
  {
    ConstrainPropertyOwner(*shader, bufferIndex);
    if(!shader->GetPostConstraints().Empty())
    {
      postPropertyOwners.PushBack(shader);
    }
  }
  for(auto&& uniformBlock : mImpl->uniformBlocks)
  {
    ConstrainPropertyOwner(*uniformBlock, bufferIndex);
    if(!uniformBlock->GetPostConstraints().Empty())
    {
      postPropertyOwners.PushBack(uniformBlock);
    }
  }
}

void UpdateManager::ProcessPropertyNotifications(BufferIndex bufferIndex)
{
  for(auto&& notification : mImpl->propertyNotifications)
  {
    bool valid = notification->Check(bufferIndex);
    if(valid)
    {
      mImpl->notificationManager.QueueMessage(PropertyChangedMessage(mImpl->propertyNotifier, notification->GetNotifyId(), notification->GetValidity()));
    }
  }
}

void UpdateManager::ForwardCompiledShadersToEventThread()
{
  DALI_ASSERT_DEBUG((mImpl->shaderSaver != 0) && "shaderSaver should be wired-up during startup.");
  if(mImpl->shaderSaver)
  {
    // lock and swap the queues
    {
      // render might be attempting to send us more binaries at the same time
      Mutex::ScopedLock lock(mImpl->compiledShaderMutex);
      mImpl->renderCompiledShaders.swap(mImpl->updateCompiledShaders);
    }

    if(mImpl->updateCompiledShaders.size() > 0)
    {
      ShaderSaver& factory = *mImpl->shaderSaver;
      for(auto&& shader : mImpl->updateCompiledShaders)
      {
        mImpl->notificationManager.QueueMessage(ShaderCompiledMessage(factory, shader));
      }
      // we don't need them in update anymore
      mImpl->updateCompiledShaders.clear();
    }
  }
}

void UpdateManager::UpdateRenderers(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex)
{
  if(mImpl->renderers.Empty())
  {
    return;
  }

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_UPDATE_RENDERERS", [&](std::ostringstream& oss)
                                          { oss << "[" << mImpl->renderers.Count() << "]"; });

  for(const auto& rendererKey : mImpl->renderers)
  {
    // Apply constraints
    auto renderer = rendererKey.Get();
    ConstrainPropertyOwner(*renderer, bufferIndex);
    if(!renderer->GetPostConstraints().Empty())
    {
      postPropertyOwners.PushBack(renderer);
    }

    mImpl->renderingRequired = renderer->PrepareRender(bufferIndex) || mImpl->renderingRequired;
  }

  DALI_TRACE_END(gTraceFilter, "DALI_UPDATE_RENDERERS");
}

void UpdateManager::UpdateNodes(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex)
{
  mImpl->nodeDirtyFlags = NodePropertyFlags::NOTHING;

  for(auto&& scene : mImpl->scenes)
  {
    if(scene && scene->root)
    {
      // Prepare resources, update shaders, for each node
      // And add the renderers to the sorted layers. Start from root, which is also a layer
      mImpl->nodeDirtyFlags |= UpdateNodeTree(*scene->root,
                                              bufferIndex,
                                              mImpl->renderQueue,
                                              postPropertyOwners);
    }
  }
}

void UpdateManager::UpdateLayers(BufferIndex bufferIndex)
{
  for(auto&& scene : mImpl->scenes)
  {
    if(scene && scene->root)
    {
      SceneGraph::UpdateLayerTree(*scene->root, bufferIndex);
    }
  }
}

uint32_t UpdateManager::Update(float    elapsedSeconds,
                               uint32_t lastVSyncTimeMilliseconds,
                               uint32_t nextVSyncTimeMilliseconds,
                               bool     renderToFboEnabled,
                               bool     isRenderingToFbo,
                               bool     uploadOnly,
                               bool&    rendererAdded)
{
  const BufferIndex bufferIndex = mSceneGraphBuffers.GetUpdateBufferIndex();

  // Clear nodes/resources which were previously discarded
  mImpl->nodeDiscardQueue.Clear(bufferIndex);
  mImpl->shaderDiscardQueue.Clear(bufferIndex);
  mImpl->rendererDiscardQueue.Clear(bufferIndex);
  mImpl->sceneDiscardQueue.Clear(bufferIndex);

  bool isAnimationRunning = IsAnimationRunning();

  // Process Touches & Gestures
  const bool gestureUpdated = ProcessGestures(bufferIndex, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds);

  bool updateScene =                                   // The scene-graph requires an update if..
    (mImpl->nodeDirtyFlags & RenderableUpdateFlags) || // ..nodes were dirty in previous frame OR
    isAnimationRunning ||                              // ..at least one animation is running OR
    mImpl->messageQueue.IsSceneUpdateRequired() ||     // ..a message that modifies the scene graph node tree is queued OR
    mImpl->frameCallbackProcessor ||                   // ..a frame callback processor is existed OR
    mImpl->discardAnimationFinishedAge > 0u ||         // ..at least one animation with EndAction::DISCARD finished
    gestureUpdated;                                    // ..a gesture property was updated

  uint32_t keepUpdating    = 0;
  mImpl->renderingRequired = false;

  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // values if the scene was updated in the previous frame.
  if(updateScene || mImpl->previousUpdateScene)
  {
    // Reset properties from the previous update
    ResetProperties(bufferIndex);
    mImpl->transformManager.ResetToBaseValue();
  }

  // Process the queued scene messages. Note, MessageQueue::FlushQueue may be called
  // between calling IsSceneUpdateRequired() above and here, so updateScene should
  // be set again
  updateScene |= mImpl->messageQueue.ProcessMessages(bufferIndex);

  // Forward compiled shader programs to event thread for saving
  ForwardCompiledShadersToEventThread();

  // Although the scene-graph may not require an update, we still need to synchronize double-buffered
  // renderer lists if the scene was updated in the previous frame.
  // We should not start skipping update steps or reusing lists until there has been two frames where nothing changes
  if(updateScene || mImpl->previousUpdateScene)
  {
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_UPDATE_INTERNAL", [&](std::ostringstream& oss)
                                            {
      oss << "[n:" << mImpl->nodes.Size() << ",";
      oss << "c:" << mImpl->customObjects.Size() << ",";
      oss << "a:" << mImpl->animations.Size() << ",";
      oss << "r:" << mImpl->renderers.Size() << ",";
      oss << "t:" << mImpl->textureSets.Size() << ",";
      oss << "s:" << mImpl->shaders.Size() << "]"; });

    // Animate
    bool animationActive = Animate(bufferIndex, elapsedSeconds);

    PropertyOwnerContainer postPropertyOwners;
    // Constraint custom objects
    ConstrainCustomObjects(postPropertyOwners, bufferIndex);

    // Clear the lists of renderers from the previous update
    for(auto&& scene : mImpl->scenes)
    {
      if(scene)
      {
        for(auto&& layer : scene->sortedLayerList)
        {
          if(layer)
          {
            layer->ClearRenderables();
          }
        }
      }
    }

    // Call the frame-callback-processor if set
    if(mImpl->frameCallbackProcessor)
    {
      if(mImpl->frameCallbackProcessor->Update(bufferIndex, elapsedSeconds))
      {
        keepUpdating |= KeepUpdating::FRAME_UPDATE_CALLBACK;
      }
    }

    // Update node hierarchy, apply constraints,
    UpdateNodes(postPropertyOwners, bufferIndex);

    // Apply constraints to RenderTasks, shaders
    ConstrainRenderTasks(postPropertyOwners, bufferIndex);
    ConstrainShaders(postPropertyOwners, bufferIndex); // shaders & uniform blocks

    // Update renderers and apply constraints
    UpdateRenderers(postPropertyOwners, bufferIndex);

    // Update the transformations of all the nodes
    if(mImpl->transformManager.Update())
    {
      mImpl->nodeDirtyFlags |= NodePropertyFlags::TRANSFORM;
    }

    // Constraint applied after transform manager updated. Only required property owner processed.
    for(auto&& propertyOwner : postPropertyOwners)
    {
      ConstrainPropertyOwner(*propertyOwner, bufferIndex, false);
    }

    // Initialise layer renderable reuse
    UpdateLayers(bufferIndex);

    // Process Property Notifications
    ProcessPropertyNotifications(bufferIndex);

    // Update cameras
    for(auto&& cameraIterator : mImpl->cameras)
    {
      cameraIterator->Update(bufferIndex);
    }

    // Process the RenderTasks if renderers exist. This creates the instructions for rendering the next frame.
    // reset the update buffer index and make sure there is enough room in the instruction container
    if(mImpl->renderersAdded)
    {
      rendererAdded = true;
      // Calculate how many render tasks we have in total
      std::size_t numberOfRenderTasks        = 0;
      std::size_t numberOfRenderInstructions = 0;
      bool        renderContinuously         = false;
      for(auto&& scene : mImpl->scenes)
      {
        if(scene && scene->taskList)
        {
          numberOfRenderTasks += scene->taskList->GetTasks().Count();
        }
      }

      mImpl->renderInstructionCapacity = 0u;
      for(auto&& scene : mImpl->scenes)
      {
        if(scene && scene->root && scene->taskList && scene->scene)
        {
          DALI_TRACE_BEGIN(gTraceFilter, "DALI_PROCESS_RENDER_TASK");
          scene->scene->GetRenderInstructions().ResetAndReserve(bufferIndex,
                                                                static_cast<uint32_t>(scene->taskList->GetTasks().Count()));

          bool sceneKeepUpdating = scene->scene->KeepRenderingCheck(elapsedSeconds);
          if(sceneKeepUpdating)
          {
            keepUpdating |= KeepUpdating::STAGE_KEEP_RENDERING;
          }

          // If there are animations running, only add render instruction if at least one animation is currently active (i.e. not delayed)
          // or the nodes are dirty
          // or keep rendering is requested
          if(!isAnimationRunning || animationActive || mImpl->renderingRequired || (mImpl->nodeDirtyFlags & RenderableUpdateFlags) || sceneKeepUpdating)
          {
            renderContinuously |= mImpl->renderTaskProcessor.Process(bufferIndex,
                                                                     *scene->taskList,
                                                                     scene->sortedLayerList,
                                                                     scene->scene->GetRenderInstructions(),
                                                                     renderToFboEnabled,
                                                                     isRenderingToFbo);

            mImpl->renderInstructionCapacity += scene->scene->GetRenderInstructions().GetCapacity();
            scene->scene->SetSkipRendering(false);
          }
          else
          {
            scene->scene->SetSkipRendering(true);
          }
          DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_PROCESS_RENDER_TASK", [&](std::ostringstream& oss)
                                                { oss << "[render instruction capacity : " << mImpl->renderInstructionCapacity << "]\n"; });

          numberOfRenderInstructions += scene->scene->GetRenderInstructions().Count(bufferIndex);
        }
      }

      if(renderContinuously)
      {
        keepUpdating |= KeepUpdating::RENDERER_CONTINUOUSLY;
      }

      DALI_LOG_INFO(gLogFilter, Debug::General, "Update: numberOfRenderTasks(%d), Render Instructions(%d)\n", numberOfRenderTasks, numberOfRenderInstructions);
    }

    DALI_TRACE_END(gTraceFilter, "DALI_UPDATE_INTERNAL");
  }

  if(!uploadOnly)
  {
    // check the countdown and notify
    mImpl->renderTaskWaiting = false;

    for(auto&& scene : mImpl->scenes)
    {
      if(scene && scene->root && scene->taskList)
      {
        RenderTaskList::RenderTaskContainer& tasks = scene->taskList->GetTasks();

        CompleteNotificationInterface::ParameterList notifyRequiredRenderTasks;

        for(auto&& renderTask : tasks)
        {
          renderTask->UpdateState();

          if(renderTask->IsWaitingToRender() &&
             renderTask->ReadyToRender(bufferIndex) /*avoid updating forever when source actor is off-stage*/)
          {
            mImpl->renderTaskWaiting = true; // keep update/render threads alive
          }

          if(renderTask->HasRendered())
          {
            notifyRequiredRenderTasks.PushBack(renderTask->GetNotifyId());
          }
        }

        if(!notifyRequiredRenderTasks.Empty())
        {
          DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "Notify a render task has finished\n");
          mImpl->notificationManager.QueueNotification(scene->taskList->GetCompleteNotificationInterface(), std::move(notifyRequiredRenderTasks));
        }
      }
    }
  }

  // Macro is undefined in release build.
  SNAPSHOT_NODE_LOGGING;

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  // Shrink relevant containers if required.
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::NODE)
  {
    mImpl->nodes.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::RENDERER)
  {
    mImpl->renderers.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::SHADER)
  {
    mImpl->shaders.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::TEXTURE_SET)
  {
    mImpl->textureSets.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::ANIMATION)
  {
    mImpl->animations.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::PROPERTY_NOTIFICATION)
  {
    mImpl->propertyNotifications.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::CUSTOM_OBJECT)
  {
    mImpl->customObjects.ShrinkToFitIfNeeded();
  }
  if(mImpl->containerRemovedFlags & ContainerRemovedFlagBits::UNIFORM_BLOCKS)
  {
    mImpl->uniformBlocks.ShrinkToFitIfNeeded();
  }

  // Reset flag
  mImpl->containerRemovedFlags = ContainerRemovedFlagBits::NOTHING;
#endif

  // A ResetProperties() may be required in the next frame
  mImpl->previousUpdateScene = updateScene;

  // Check whether further updates are required
  keepUpdating |= KeepUpdatingCheck(elapsedSeconds);

  if(keepUpdating & (KeepUpdating::STAGE_KEEP_RENDERING | KeepUpdating::FRAME_UPDATE_CALLBACK | KeepUpdating::RENDERER_CONTINUOUSLY))
  {
    // Set dirty flags for next frame to continue rendering
    mImpl->nodeDirtyFlags |= RenderableUpdateFlags;
  }

  // tell the update manager that we're done so the queue can be given to event thread
  mImpl->notificationManager.UpdateCompleted();

  // The update has finished; swap the double-buffering indices
  mSceneGraphBuffers.Swap();

  return keepUpdating;
}

void UpdateManager::PostRender()
{
  // Reset dirty flag
  for(auto&& renderer : mImpl->renderers)
  {
    renderer->SetUpdated(false);
  }

  for(auto&& shader : mImpl->shaders)
  {
    shader->SetUpdated(false);
  }

  for(auto&& uniformBlock : mImpl->uniformBlocks)
  {
    uniformBlock->SetUpdated(false);
  }

  for(auto&& scene : mImpl->scenes)
  {
    scene->root->SetUpdatedTree(false);
  }
}

uint32_t UpdateManager::KeepUpdatingCheck(float elapsedSeconds) const
{
  uint32_t keepUpdatingRequest = KeepUpdating::NOT_REQUESTED;

  // If the rendering behavior is set to continuously render, then continue to render.
  // Keep updating until no messages are received and no animations are running.
  // If an animation has just finished, update at least two frames more for Discard end-actions.
  // No need to check for renderQueue as there is always a render after update and if that
  // render needs another update it will tell the adaptor to call update again

  if(mImpl->renderingBehavior == DevelStage::Rendering::CONTINUOUSLY)
  {
    keepUpdatingRequest |= KeepUpdating::STAGE_KEEP_RENDERING;
  }

  if(IsAnimationRunning() ||
     mImpl->animationFinishedDuringUpdate ||
     mImpl->discardAnimationFinishedAge > 0u)
  {
    keepUpdatingRequest |= KeepUpdating::ANIMATIONS_RUNNING;
  }

  if(mImpl->renderTaskWaiting)
  {
    keepUpdatingRequest |= KeepUpdating::RENDER_TASK_SYNC;
  }

  return keepUpdatingRequest;
}

void UpdateManager::SurfaceReplaced(Scene* scene)
{
  using DerivedType = MessageValue1<RenderManager, Scene*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SurfaceReplaced, scene);
}

void UpdateManager::KeepRendering(float durationSeconds)
{
  for(auto&& scene : mImpl->scenes)
  {
    if(scene->scene)
    {
      scene->scene->KeepRendering(durationSeconds);
    }
  }
}

void UpdateManager::SetRenderingBehavior(DevelStage::Rendering renderingBehavior)
{
  mImpl->renderingBehavior = renderingBehavior;
}

void UpdateManager::RequestRendering()
{
  mImpl->renderingRequired = true;
}

void UpdateManager::RequestClearProgramCache()
{
  // Message has ownership of format while in transit from update -> render
  using DerivedType = Message<RenderManager>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::ClearProgramCache);
}

Node* UpdateManager::GetNodePointerById(uint32_t nodeId) const
{
  Node* foundNodePointer = nullptr;
  auto  iter             = mImpl->nodeIdMap.find(nodeId);
  if(iter != mImpl->nodeIdMap.end())
  {
    foundNodePointer = iter->second;
  }
  return foundNodePointer;
}

void UpdateManager::SetLayerDepths(const SortedLayerPointers& layers, const Layer* rootLayer)
{
  for(auto&& scene : mImpl->scenes)
  {
    if(scene && scene->root == rootLayer)
    {
      scene->sortedLayerList = layers;
      break;
    }
  }
}

void UpdateManager::SetDepthIndices(OwnerPointer<NodeDepths>& nodeDepths)
{
  // note, this vector is already in depth order.
  // So if we reverse iterate, we can assume that
  // my descendant node's depth index are updated.

  // And also, This API is the last flushed message.
  // We can now setup the DESCENDENT_HIERARCHY_CHANGED flag here.
  for(auto rIter = nodeDepths->nodeDepths.rbegin(), rEndIter = nodeDepths->nodeDepths.rend(); rIter != rEndIter; rIter++)
  {
    auto* node = rIter->node;
    node->PropagateDescendentFlags();
    node->SetDepthIndex(rIter->sortedDepth);
    if(node->IsChildrenReorderRequired())
    {
      // Reorder children container only if sibiling order changed.
      NodeContainer& container = node->GetChildren();
      std::sort(container.Begin(), container.End(), [](Node* a, Node* b)
                { return a->GetDepthIndex() < b->GetDepthIndex(); });
    }
  }
}

void UpdateManager::AddFrameCallback(OwnerPointer<FrameCallback>& frameCallback, const Node* rootNode)
{
  mImpl->GetFrameCallbackProcessor(*this).AddFrameCallback(frameCallback, rootNode);
}

void UpdateManager::RemoveFrameCallback(FrameCallbackInterface* frameCallback)
{
  mImpl->GetFrameCallbackProcessor(*this).RemoveFrameCallback(frameCallback);
}

void UpdateManager::NotifyFrameCallback(FrameCallbackInterface* frameCallback, Dali::UpdateProxy::NotifySyncPoint syncPoint)
{
  mImpl->GetFrameCallbackProcessor(*this).NotifyFrameCallback(frameCallback, syncPoint);
}

void UpdateManager::KeepRenderResultToFrameBuffer(Render::FrameBuffer* frameBuffer)
{
  frameBuffer->KeepRenderResult();
}

void UpdateManager::ClearRenderResultToFrameBuffer(Render::FrameBuffer* frameBuffer)
{
  frameBuffer->ClearRenderResult();
}

void UpdateManager::AddSampler(OwnerPointer<Render::Sampler>& sampler)
{
  // Message has ownership of Sampler while in transit from update to render
  using DerivedType = MessageValue1<RenderManager, OwnerPointer<Render::Sampler>>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AddSampler, sampler);
}

void UpdateManager::RemoveSampler(Render::Sampler* sampler)
{
  using DerivedType = MessageValue1<RenderManager, Render::Sampler*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::RemoveSampler, sampler);
}

void UpdateManager::SetFilterMode(Render::Sampler* sampler, uint32_t minFilterMode, uint32_t magFilterMode)
{
  using DerivedType = MessageValue3<RenderManager, Render::Sampler*, uint32_t, uint32_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetFilterMode, sampler, minFilterMode, magFilterMode);
}

void UpdateManager::SetWrapMode(Render::Sampler* sampler, uint32_t rWrapMode, uint32_t sWrapMode, uint32_t tWrapMode)
{
  using DerivedType = MessageValue4<RenderManager, Render::Sampler*, uint32_t, uint32_t, uint32_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetWrapMode, sampler, rWrapMode, sWrapMode, tWrapMode);
}

void UpdateManager::AddVertexBuffer(OwnerPointer<Render::VertexBuffer>& vertexBuffer)
{
  // Message has ownership of format while in transit from update -> render
  using DerivedType = MessageValue1<RenderManager, OwnerPointer<Render::VertexBuffer>>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AddVertexBuffer, vertexBuffer);
}

void UpdateManager::RemoveVertexBuffer(Render::VertexBuffer* vertexBuffer)
{
  using DerivedType = MessageValue1<RenderManager, Render::VertexBuffer*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::RemoveVertexBuffer, vertexBuffer);
}

void UpdateManager::SetVertexBufferFormat(Render::VertexBuffer* vertexBuffer, OwnerPointer<Render::VertexBuffer::Format>& format)
{
  // Message has ownership of format while in transit from update -> render
  using DerivedType = MessageValue2<RenderManager, Render::VertexBuffer*, OwnerPointer<Render::VertexBuffer::Format>>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetVertexBufferFormat, vertexBuffer, format);
}

void UpdateManager::SetVertexBufferData(Render::VertexBuffer* vertexBuffer, OwnerPointer<Vector<uint8_t>>& data, uint32_t size)
{
  // Message has ownership of format while in transit from update -> render
  using DerivedType = MessageValue3<RenderManager, Render::VertexBuffer*, OwnerPointer<Dali::Vector<uint8_t>>, uint32_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetVertexBufferData, vertexBuffer, data, size);
}

void UpdateManager::SetVertexBufferDivisor(Render::VertexBuffer* vertexBuffer, uint32_t divisor)
{
  using LocalType = MessageValue1<Render::VertexBuffer, uint32_t>;
  uint32_t* slot  = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(LocalType));
  new(slot) LocalType(vertexBuffer, &Render::VertexBuffer::SetDivisor, divisor);
}

void UpdateManager::SetVertexBufferUpdateCallback(Render::VertexBuffer* vertexBuffer, Dali::VertexBufferUpdateCallback* callback)
{
  // Message has ownership of format while in transit from update -> render
  using DerivedType = MessageValue2<RenderManager, Render::VertexBuffer*, Dali::VertexBufferUpdateCallback*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetVertexBufferUpdateCallback, vertexBuffer, callback);
}

void UpdateManager::AddGeometry(OwnerPointer<Render::Geometry>& geometry)
{
  // Message has ownership of format while in transit from update -> render
  using DerivedType = MessageValue1<RenderManager, OwnerPointer<Render::Geometry>>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AddGeometry, geometry);
}

void UpdateManager::RemoveGeometry(Render::Geometry* geometry)
{
  using DerivedType = MessageValue1<RenderManager, Render::Geometry*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::RemoveGeometry, geometry);
}

void UpdateManager::SetGeometryType(Render::Geometry* geometry, uint32_t geometryType)
{
  using DerivedType = MessageValue2<RenderManager, Render::Geometry*, uint32_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetGeometryType, geometry, geometryType);
}

void UpdateManager::SetIndexBuffer(Render::Geometry* geometry, Render::Geometry::Uint16ContainerType& indices)
{
  using DerivedType = IndexBufferMessage<RenderManager, Render::Geometry::Uint16ContainerType>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, geometry, indices);
}

void UpdateManager::SetIndexBuffer(Render::Geometry* geometry, Render::Geometry::Uint32ContainerType& indices)
{
  using DerivedType = IndexBufferMessage<RenderManager, Render::Geometry::Uint32ContainerType>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, geometry, indices);
}

void UpdateManager::RemoveVertexBuffer(Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer)
{
  using DerivedType = MessageValue2<RenderManager, Render::Geometry*, Render::VertexBuffer*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::RemoveVertexBuffer, geometry, vertexBuffer);
}

void UpdateManager::AttachVertexBuffer(Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer)
{
  using DerivedType = MessageValue2<RenderManager, Render::Geometry*, Render::VertexBuffer*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AttachVertexBuffer, geometry, vertexBuffer);
}

void UpdateManager::AddTexture(const Render::TextureKey& texture)
{
  using DerivedType = MessageValue1<RenderManager, Render::TextureKey>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AddTexture, texture);
}

void UpdateManager::RemoveTexture(const Render::TextureKey& texture)
{
  using DerivedType = MessageValue1<RenderManager, Render::TextureKey>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::RemoveTexture, texture);
}

void UpdateManager::UploadTexture(const Render::TextureKey& texture, PixelDataPtr pixelData, const Graphics::UploadParams& params)
{
  using DerivedType = MessageValue3<RenderManager, Render::TextureKey, PixelDataPtr, Graphics::UploadParams>;

  // Reserve some memory inside the message queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::UploadTexture, texture, pixelData, params);
}

void UpdateManager::GenerateMipmaps(const Render::TextureKey& texture)
{
  using DerivedType = MessageValue1<RenderManager, Render::TextureKey>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::GenerateMipmaps, texture);
}

void UpdateManager::SetTextureSize(const Render::TextureKey& texture, const Dali::ImageDimensions& size)
{
  using DerivedType = MessageValue2<RenderManager, Render::TextureKey, Dali::ImageDimensions>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetTextureSize, texture, size);
}

void UpdateManager::SetTextureFormat(const Render::TextureKey& texture, Dali::Pixel::Format pixelFormat)
{
  using DerivedType = MessageValue2<RenderManager, Render::TextureKey, Dali::Pixel::Format>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetTextureFormat, texture, pixelFormat);
}

void UpdateManager::AddFrameBuffer(OwnerPointer<Render::FrameBuffer>& frameBuffer)
{
  using DerivedType = MessageValue1<RenderManager, OwnerPointer<Render::FrameBuffer>>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AddFrameBuffer, frameBuffer);
}

void UpdateManager::RemoveFrameBuffer(Render::FrameBuffer* frameBuffer)
{
  using DerivedType = MessageValue1<RenderManager, Render::FrameBuffer*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::RemoveFrameBuffer, frameBuffer);
}

void UpdateManager::AttachColorTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer)
{
  using DerivedType = MessageValue4<RenderManager, Render::FrameBuffer*, Render::Texture*, uint32_t, uint32_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AttachColorTextureToFrameBuffer, frameBuffer, texture, mipmapLevel, layer);
}

void UpdateManager::AttachDepthTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel)
{
  using DerivedType = MessageValue3<RenderManager, Render::FrameBuffer*, Render::Texture*, uint32_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AttachDepthTextureToFrameBuffer, frameBuffer, texture, mipmapLevel);
}

void UpdateManager::AttachDepthStencilTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel)
{
  using DerivedType = MessageValue3<RenderManager, Render::FrameBuffer*, Render::Texture*, uint32_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::AttachDepthStencilTextureToFrameBuffer, frameBuffer, texture, mipmapLevel);
}

void UpdateManager::SetMultiSamplingLevelToFrameBuffer(Render::FrameBuffer* frameBuffer, uint8_t multiSamplingLevel)
{
  using DerivedType = MessageValue2<RenderManager, Render::FrameBuffer*, uint8_t>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mImpl->renderQueue.ReserveMessageSlot(mSceneGraphBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mImpl->renderManager, &RenderManager::SetMultiSamplingLevelToFrameBuffer, frameBuffer, multiSamplingLevel);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
