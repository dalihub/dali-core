# Add platform abstraction headers here

SET(SOURCES ${SOURCES}
  ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/profiling.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/input-options.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/system-overlay.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lockless-buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/gesture-event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/hover-event-integ.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/key-event-integ.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/long-press-gesture-event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/wheel-event-integ.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/multi-point-event-integ.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/pan-gesture-event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/pinch-gesture-event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/point.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/tap-gesture-event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/touch-event-combiner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/events/touch-event-integ.cpp
  PARENT_SCOPE )

SET(INTEGRATION_API_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/core.h
  ${CMAKE_CURRENT_SOURCE_DIR}/context-notifier.h
  ${CMAKE_CURRENT_SOURCE_DIR}/debug.h
  ${CMAKE_CURRENT_SOURCE_DIR}/profiling.h
  ${CMAKE_CURRENT_SOURCE_DIR}/input-options.h
  ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.h
  ${CMAKE_CURRENT_SOURCE_DIR}/resource-policies.h
  ${CMAKE_CURRENT_SOURCE_DIR}/resource-types.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gl-abstraction.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gl-defines.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gl-sync-abstraction.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gesture-manager.h
  ${CMAKE_CURRENT_SOURCE_DIR}/render-controller.h
  ${CMAKE_CURRENT_SOURCE_DIR}/platform-abstraction.h
  ${CMAKE_CURRENT_SOURCE_DIR}/system-overlay.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lockless-buffer.h

  ${CMAKE_CURRENT_SOURCE_DIR}/events/event.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/gesture-event.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/gesture-requests.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/hover-event-integ.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/key-event-integ.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/long-press-gesture-event.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/wheel-event-integ.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/multi-point-event-integ.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/pan-gesture-event.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/pinch-gesture-event.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/point.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/tap-gesture-event.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/touch-event-combiner.h
  ${CMAKE_CURRENT_SOURCE_DIR}/events/touch-event-integ.h

  PARENT_SCOPE )
