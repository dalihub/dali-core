SET(PKG_NAME "dali-internal")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-internal")

SET(TC_SOURCES
        utc-Dali-Internal-AnimatableProperty.cpp
        utc-Dali-Internal-ActorObserver.cpp
        utc-Dali-Internal-ActorRelayout.cpp
        utc-Dali-Internal-ConstString.cpp
        utc-Dali-Internal-Core.cpp
        utc-Dali-Internal-DummyMemoryPool.cpp
        utc-Dali-Internal-FixedSizeMemoryPool.cpp
        utc-Dali-Internal-FrustumCulling.cpp
        utc-Dali-Internal-Gesture.cpp
        utc-Dali-Internal-GpuBuffer.cpp
        utc-Dali-Internal-Handles.cpp
        utc-Dali-Internal-IndexedConstStringMap.cpp
        utc-Dali-Internal-IndexedIntegerMap.cpp
        utc-Dali-Internal-LongPressGesture.cpp
        utc-Dali-Internal-MatrixUtils.cpp
        utc-Dali-Internal-MemoryPoolObjectAllocator.cpp
        utc-Dali-Internal-OrderedSet.cpp
        utc-Dali-Internal-OwnerPointer.cpp
        utc-Dali-Internal-PinchGesture.cpp
        utc-Dali-Internal-PinchGestureProcessor.cpp
        utc-Dali-Internal-PipelineCache.cpp
        utc-Dali-Internal-RotationGesture.cpp
        utc-Dali-Internal-Shader.cpp
        utc-Dali-Internal-TapGesture.cpp
        utc-Dali-Internal-TapGestureProcessor.cpp
        utc-Dali-Internal-Texture.cpp
        utc-Dali-Internal-TransformManagerProperty.cpp
)

LIST(APPEND TC_SOURCES
        ../dali/dali-test-suite-utils/mesh-builder.cpp
        ../dali/dali-test-suite-utils/test-custom-actor.cpp
        ../dali/dali-test-suite-utils/test-harness.cpp
        ../dali/dali-test-suite-utils/test-actor-utils.cpp
        ../dali/dali-test-suite-utils/dali-test-suite-utils.cpp
        ../dali/dali-test-suite-utils/test-application.cpp
        ../dali/dali-test-suite-utils/test-gl-abstraction.cpp
        ../dali/dali-test-suite-utils/test-graphics-buffer.cpp
        ../dali/dali-test-suite-utils/test-graphics-command-buffer.cpp
        ../dali/dali-test-suite-utils/test-graphics-controller.cpp
        ../dali/dali-test-suite-utils/test-graphics-framebuffer.cpp
        ../dali/dali-test-suite-utils/test-graphics-pipeline.cpp
        ../dali/dali-test-suite-utils/test-graphics-program.cpp
        ../dali/dali-test-suite-utils/test-graphics-reflection.cpp
        ../dali/dali-test-suite-utils/test-graphics-sampler.cpp
        ../dali/dali-test-suite-utils/test-graphics-shader.cpp
        ../dali/dali-test-suite-utils/test-graphics-texture.cpp
        ../dali/dali-test-suite-utils/test-graphics-sync-impl.cpp
        ../dali/dali-test-suite-utils/test-graphics-sync-object.cpp
        ../dali/dali-test-suite-utils/test-native-image.cpp
        ../dali/dali-test-suite-utils/test-platform-abstraction.cpp
        ../dali/dali-test-suite-utils/test-render-controller.cpp
        ../dali/dali-test-suite-utils/test-trace-call-stack.cpp
        ../dali/dali-test-suite-utils/test-addon-manager.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
     dali2-core
)

ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror )
ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ../../..
    .
    ${${CAPI_LIB}_INCLUDE_DIRS}
    ../dali/dali-test-suite-utils
    ../dali-devel
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    --coverage -ldl
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
