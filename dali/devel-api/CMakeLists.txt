SET( SOURCES ${SOURCES}
  ${CMAKE_CURRENT_SOURCE_DIR}/actors/actor-devel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/actors/custom-actor-devel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/animation/animation-data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/animation/animation-devel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/animation/path-constrainer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/hash.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/stage-devel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/hit-test-algorithm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/images/distance-field.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/images/texture-set-image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/images/nine-patch-image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/images/pixel-data-devel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/object/handle-devel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/object/weak-handle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/object/csharp-type-registry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/scripting/scripting.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/signals/signal-delegate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/threading/conditional-wait.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/threading/mutex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/threading/thread.cpp

  PARENT_SCOPE )

SET( DEVEL_API_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/actors/actor-devel.h
  ${CMAKE_CURRENT_SOURCE_DIR}/actors/custom-actor-devel.h
  ${CMAKE_CURRENT_SOURCE_DIR}/actors/layer-devel.h

  ${CMAKE_CURRENT_SOURCE_DIR}/animation/animation-data.h
  ${CMAKE_CURRENT_SOURCE_DIR}/animation/path-constrainer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/animation/animation-devel.h

  ${CMAKE_CURRENT_SOURCE_DIR}/common/circular-queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/hash.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/map-wrapper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/owner-container.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/ref-counted-dali-vector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/set-wrapper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/stage-devel.h

  ${CMAKE_CURRENT_SOURCE_DIR}/events/hit-test-algorithm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/touch-event-devel.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/key-event-devel.h

  ${CMAKE_CURRENT_SOURCE_DIR}/images/distance-field.h
  ${CMAKE_CURRENT_SOURCE_DIR}/images/native-image-interface-extension.h
  ${CMAKE_CURRENT_SOURCE_DIR}/images/nine-patch-image.h
  ${CMAKE_CURRENT_SOURCE_DIR}/images/texture-set-image.h
  ${CMAKE_CURRENT_SOURCE_DIR}/images/pixel-data-devel.h

  ${CMAKE_CURRENT_SOURCE_DIR}/object/csharp-type-info.h
  ${CMAKE_CURRENT_SOURCE_DIR}/object/csharp-type-registry.h
  ${CMAKE_CURRENT_SOURCE_DIR}/object/handle-devel.h
  ${CMAKE_CURRENT_SOURCE_DIR}/object/property-helper-devel.h

  ${CMAKE_CURRENT_SOURCE_DIR}/rendering/renderer-devel.h

  ${CMAKE_CURRENT_SOURCE_DIR}/signals/signal-delegate.h

  ${CMAKE_CURRENT_SOURCE_DIR}/scripting/enum-helper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/scripting/scripting.h

  ${CMAKE_CURRENT_SOURCE_DIR}/threading/conditional-wait.h
  ${CMAKE_CURRENT_SOURCE_DIR}/threading/mutex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/threading/thread.h

  PARENT_SCOPE )
