cmake_minimum_required(VERSION 3.11)

if (WIN32)

PROJECT (dali-core)

add_subdirectory(dali/devel-api)
add_subdirectory(dali/integration-api)
#add_subdirectory(dali/internal)
add_subdirectory(dali/public-api)

SET(INTERNAL_DIR ${dali_SOURCE_DIR}/dali-core/dali/internal)

SET(SOURCES ${SOURCES}
  ${dali_SOURCE_DIR}/dali-core/dali/integration-api/trace.cpp
  ${INTERNAL_DIR}/common/blending-options.cpp
  ${INTERNAL_DIR}/common/core-impl.cpp
  ${INTERNAL_DIR}/common/internal-constants.cpp
  ${INTERNAL_DIR}/common/math.cpp
  ${INTERNAL_DIR}/common/message-buffer.cpp
  ${INTERNAL_DIR}/common/mutex-trace.cpp
  ${INTERNAL_DIR}/common/image-sampler.cpp
  ${INTERNAL_DIR}/common/image-attributes.cpp
  ${INTERNAL_DIR}/common/fixed-size-memory-pool.cpp

  ${INTERNAL_DIR}/event/actors/actor-impl.cpp
  ${INTERNAL_DIR}/event/actors/custom-actor-internal.cpp
  ${INTERNAL_DIR}/event/actors/layer-impl.cpp
  ${INTERNAL_DIR}/event/actors/layer-list.cpp
  ${INTERNAL_DIR}/event/actors/camera-actor-impl.cpp
  ${INTERNAL_DIR}/event/animation/animation-impl.cpp
  ${INTERNAL_DIR}/event/animation/animation-playlist.cpp
  ${INTERNAL_DIR}/event/animation/constrainer.cpp
  ${INTERNAL_DIR}/event/animation/constraint-base.cpp
  ${INTERNAL_DIR}/event/animation/key-frames-impl.cpp
  ${INTERNAL_DIR}/event/animation/linear-constrainer-impl.cpp
  ${INTERNAL_DIR}/event/animation/path-impl.cpp
  ${INTERNAL_DIR}/event/animation/path-constrainer-impl.cpp
  ${INTERNAL_DIR}/event/common/event-thread-services.cpp
  ${INTERNAL_DIR}/event/common/notification-manager.cpp
  ${INTERNAL_DIR}/event/common/object-impl.cpp
  ${INTERNAL_DIR}/event/common/object-registry-impl.cpp
  ${INTERNAL_DIR}/event/common/projection.cpp
  ${INTERNAL_DIR}/event/common/property-buffer-impl.cpp
  ${INTERNAL_DIR}/event/common/property-conditions-impl.cpp
  ${INTERNAL_DIR}/event/common/property-metadata.cpp
  ${INTERNAL_DIR}/event/common/property-notification-impl.cpp
  ${INTERNAL_DIR}/event/common/property-notification-manager.cpp
  ${INTERNAL_DIR}/event/common/property-helper.cpp
  ${INTERNAL_DIR}/event/common/stage-impl.cpp
  ${INTERNAL_DIR}/event/common/system-overlay-impl.cpp
  ${INTERNAL_DIR}/event/common/thread-local-storage.cpp
  ${INTERNAL_DIR}/event/common/type-info-impl.cpp
  ${INTERNAL_DIR}/event/common/type-registry-impl.cpp
  ${INTERNAL_DIR}/event/effects/shader-factory.cpp
  ${INTERNAL_DIR}/event/events/actor-gesture-data.cpp
  ${INTERNAL_DIR}/event/events/actor-observer.cpp
  ${INTERNAL_DIR}/event/events/event-processor.cpp
  ${INTERNAL_DIR}/event/events/key-event-impl.cpp
  ${INTERNAL_DIR}/event/events/key-event-processor.cpp
  ${INTERNAL_DIR}/event/events/gesture-detector-impl.cpp
  ${INTERNAL_DIR}/event/events/gesture-event-processor.cpp
  ${INTERNAL_DIR}/event/events/gesture-processor.cpp
  ${INTERNAL_DIR}/event/events/hit-test-algorithm-impl.cpp
  ${INTERNAL_DIR}/event/events/hover-event-processor.cpp
  ${INTERNAL_DIR}/event/events/long-press-gesture-detector-impl.cpp
  ${INTERNAL_DIR}/event/events/long-press-gesture-processor.cpp
  ${INTERNAL_DIR}/event/events/wheel-event-processor.cpp
  ${INTERNAL_DIR}/event/events/multi-point-event-util.cpp
  ${INTERNAL_DIR}/event/events/pan-gesture-detector-impl.cpp
  ${INTERNAL_DIR}/event/events/pan-gesture-processor.cpp
  ${INTERNAL_DIR}/event/events/pinch-gesture-detector-impl.cpp
  ${INTERNAL_DIR}/event/events/pinch-gesture-processor.cpp
  ${INTERNAL_DIR}/event/events/tap-gesture-detector-impl.cpp
  ${INTERNAL_DIR}/event/events/tap-gesture-processor.cpp
  ${INTERNAL_DIR}/event/events/touch-data-impl.cpp
  ${INTERNAL_DIR}/event/events/touch-event-processor.cpp
  ${INTERNAL_DIR}/event/images/bitmap-packed-pixel.cpp
  ${INTERNAL_DIR}/event/images/bitmap-compressed.cpp
  ${INTERNAL_DIR}/event/images/image-impl.cpp
  ${INTERNAL_DIR}/event/images/buffer-image-impl.cpp
  ${INTERNAL_DIR}/event/images/frame-buffer-image-impl.cpp
  ${INTERNAL_DIR}/event/images/encoded-buffer-image-impl.cpp
  ${INTERNAL_DIR}/event/images/nine-patch-image-impl.cpp
  ${INTERNAL_DIR}/event/images/resource-image-impl.cpp
  ${INTERNAL_DIR}/event/images/native-image-impl.cpp
  ${INTERNAL_DIR}/event/images/pixel-data-impl.cpp
  ${INTERNAL_DIR}/event/object/custom-object-internal.cpp
  ${INTERNAL_DIR}/event/render-tasks/render-task-impl.cpp
  ${INTERNAL_DIR}/event/render-tasks/render-task-list-impl.cpp
  ${INTERNAL_DIR}/event/rendering/frame-buffer-impl.cpp
  ${INTERNAL_DIR}/event/rendering/geometry-impl.cpp
  ${INTERNAL_DIR}/event/rendering/texture-impl.cpp
  ${INTERNAL_DIR}/event/rendering/texture-set-impl.cpp
  ${INTERNAL_DIR}/event/rendering/renderer-impl.cpp
  ${INTERNAL_DIR}/event/rendering/sampler-impl.cpp
  ${INTERNAL_DIR}/event/rendering/shader-impl.cpp
  ${INTERNAL_DIR}/event/size-negotiation/memory-pool-relayout-container.cpp
  ${INTERNAL_DIR}/event/size-negotiation/relayout-controller-impl.cpp

  ${INTERNAL_DIR}/render/common/render-algorithms.cpp
  ${INTERNAL_DIR}/render/common/render-debug.cpp
  ${INTERNAL_DIR}/render/common/render-instruction.cpp
  ${INTERNAL_DIR}/render/common/render-instruction-container.cpp
  ${INTERNAL_DIR}/render/common/render-item.cpp
  ${INTERNAL_DIR}/render/common/render-tracker.cpp
  ${INTERNAL_DIR}/render/common/render-manager.cpp
  ${INTERNAL_DIR}/render/data-providers/render-data-provider.cpp
  ${INTERNAL_DIR}/render/gl-resources/context.cpp
  ${INTERNAL_DIR}/render/gl-resources/frame-buffer-state-cache.cpp
  ${INTERNAL_DIR}/render/gl-resources/gl-call-debug.cpp
  ${INTERNAL_DIR}/render/gl-resources/gpu-buffer.cpp
  ${INTERNAL_DIR}/render/queue/render-queue.cpp
  ${INTERNAL_DIR}/render/renderers/render-frame-buffer.cpp
  ${INTERNAL_DIR}/render/renderers/render-geometry.cpp
  ${INTERNAL_DIR}/render/renderers/render-property-buffer.cpp
  ${INTERNAL_DIR}/render/renderers/render-renderer.cpp
  ${INTERNAL_DIR}/render/renderers/render-texture.cpp
  ${INTERNAL_DIR}/render/shaders/program.cpp
  ${INTERNAL_DIR}/render/shaders/program-controller.cpp
  ${INTERNAL_DIR}/render/shaders/scene-graph-shader.cpp

  ${INTERNAL_DIR}/update/animation/scene-graph-animation.cpp
  ${INTERNAL_DIR}/update/animation/scene-graph-constraint-base.cpp
  ${INTERNAL_DIR}/update/common/discard-queue.cpp
  ${INTERNAL_DIR}/update/common/property-base.cpp
  ${INTERNAL_DIR}/update/common/property-owner-messages.cpp
  ${INTERNAL_DIR}/update/common/property-condition-functions.cpp
  ${INTERNAL_DIR}/update/common/property-condition-step-functions.cpp
  ${INTERNAL_DIR}/update/common/property-condition-variable-step-functions.cpp
  ${INTERNAL_DIR}/update/common/property-owner.cpp
  ${INTERNAL_DIR}/update/common/scene-graph-buffers.cpp
  ${INTERNAL_DIR}/update/common/scene-graph-connection-change-propagator.cpp
  ${INTERNAL_DIR}/update/common/scene-graph-property-notification.cpp
  ${INTERNAL_DIR}/update/common/uniform-map.cpp
  ${INTERNAL_DIR}/update/controllers/render-message-dispatcher.cpp
  ${INTERNAL_DIR}/update/controllers/scene-controller-impl.cpp
  ${INTERNAL_DIR}/update/gestures/pan-gesture-profiling.cpp
  ${INTERNAL_DIR}/update/gestures/scene-graph-pan-gesture.cpp
  ${INTERNAL_DIR}/update/queue/update-message-queue.cpp
  ${INTERNAL_DIR}/update/manager/render-instruction-processor.cpp
  ${INTERNAL_DIR}/update/manager/render-task-processor.cpp
  ${INTERNAL_DIR}/update/manager/transform-manager.cpp
  ${INTERNAL_DIR}/update/manager/update-algorithms.cpp
  ${INTERNAL_DIR}/update/manager/update-manager.cpp
  ${INTERNAL_DIR}/update/manager/update-manager-debug.cpp
  ${INTERNAL_DIR}/update/render-tasks/scene-graph-camera.cpp
  ${INTERNAL_DIR}/update/nodes/node.cpp
  ${INTERNAL_DIR}/update/nodes/node-messages.cpp
  ${INTERNAL_DIR}/update/nodes/scene-graph-layer.cpp
  ${INTERNAL_DIR}/update/render-tasks/scene-graph-render-task.cpp
  ${INTERNAL_DIR}/update/render-tasks/scene-graph-render-task-list.cpp
  ${INTERNAL_DIR}/update/rendering/scene-graph-texture-set.cpp
  ${INTERNAL_DIR}/update/rendering/scene-graph-renderer.cpp
  )

link_directories(
${dali_SOURCE_DIR}/dali-adaptor/dali-windows-backend/ExLib
)

ADD_DEFINITIONS(
/vmg
/NODEFAULTLIB:"libcmt.lib"
/FI"${dali_SOURCE_DIR}/dali-adaptor/dali-windows-backend/ExInclude/PreprocessorDefinitions.h"
/DBUILDING_DALI_CORE
/MP
)
#head file path  
INCLUDE_DIRECTORIES(  
./
${dali_SOURCE_DIR}/dali-adaptor/dali-windows-backend/ExInclude
)

set(LIBRARY_OUTPUT_PATH ${dali_SOURCE_DIR}/bin)

ADD_LIBRARY(dali-core SHARED ${SOURCES})

target_link_libraries(dali-core WindowsPlatform.lib)

endif()

#add link library  
#TARGET_LINK_LIBRARIES(${FS_BUILD_BINARY_PREFIX}sqrt ${LIBRARIES})