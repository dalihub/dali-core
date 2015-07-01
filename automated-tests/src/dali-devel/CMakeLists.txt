SET(PKG_NAME "dali-devel")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-devel")

SET(TC_SOURCES
        utc-Dali-Atlas.cpp
        utc-Dali-Context.cpp
        utc-Dali-Constrainer.cpp
        utc-Dali-DistanceField.cpp
        utc-Dali-DynamicsBodyConfig.cpp
        utc-Dali-DynamicsShape.cpp
        utc-Dali-DynamicsWorld.cpp
        utc-Dali-DynamicsWorldConfig.cpp
        utc-Dali-Geometry.cpp
        utc-Dali-Hash.cpp
        utc-Dali-HitTestAlgorithm.cpp
        utc-Dali-Material.cpp
        utc-Dali-Mutex.cpp
        utc-Dali-PropertyBuffer.cpp
        utc-Dali-Renderer.cpp
        utc-Dali-Sampler.cpp
        utc-Dali-Scripting.cpp
        utc-Dali-Shader.cpp
)

LIST(APPEND TC_SOURCES
        ../dali/dali-test-suite-utils/mesh-builder.cpp
        ../dali/dali-test-suite-utils/dali-test-suite-utils.cpp
        ../dali/dali-test-suite-utils/test-harness.cpp
        ../dali/dali-test-suite-utils/test-application.cpp
        ../dali/dali-test-suite-utils/test-dynamics.cpp
        ../dali/dali-test-suite-utils/test-gesture-manager.cpp
        ../dali/dali-test-suite-utils/test-gl-abstraction.cpp
        ../dali/dali-test-suite-utils/test-gl-sync-abstraction.cpp
        ../dali/dali-test-suite-utils/test-native-image.cpp
        ../dali/dali-test-suite-utils/test-platform-abstraction.cpp
        ../dali/dali-test-suite-utils/test-render-controller.cpp
        ../dali/dali-test-suite-utils/test-trace-call-stack.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
     dali-core
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${${CAPI_LIB}_CFLAGS_OTHER} -O0 -ggdb --coverage -Wall -Werror=return-type")

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ../../..
    .
    ${${CAPI_LIB}_INCLUDE_DIRS}
    ../dali/dali-test-suite-utils
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    -lpthread
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
