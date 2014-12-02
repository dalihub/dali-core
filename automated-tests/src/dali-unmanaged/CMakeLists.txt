SET(PKG_NAME "dali-unmanaged")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-unmanaged")

SET(TC_SOURCES
  utc-Dali-Constrainable.cpp
  utc-Dali-Context.cpp
  utc-Dali-CustomActor.cpp
  utc-Dali-DistanceField.cpp
  utc-Dali-DynamicsBody.cpp
  utc-Dali-DynamicsBodyConfig.cpp
  utc-Dali-DynamicsJoint.cpp
  utc-Dali-DynamicsShape.cpp
  utc-Dali-DynamicsWorld.cpp
  utc-Dali-DynamicsWorldConfig.cpp
  utc-Dali-EncodedBufferImage.cpp
  utc-Dali-HitTestAlgorithm.cpp
  utc-Dali-LightActor.cpp
  utc-Dali-LocklessBuffer.cpp
  utc-Dali-MeshActor.cpp
  utc-Dali-MeshData.cpp
  utc-Dali-Model.cpp
  utc-Dali-MouseWheelEvent.cpp
  utc-Dali-NinePatchImages.cpp
  utc-Dali-ObjectRegistry.cpp
  utc-Dali-Random.cpp
  utc-Dali-RenderTask.cpp
  utc-Dali-ShaderEffect.cpp
  utc-Dali-Scripting.cpp
  utc-Dali-Spline.cpp
  utc-Dali-TypeRegistry.cpp
  utc-Dali-Utf8.cpp
  utc-Dali-Vector.cpp
)

LIST(APPEND TC_SOURCES
        dali-test-suite-utils/mesh-builder.cpp
        dali-test-suite-utils/test-dynamics.cpp
        dali-test-suite-utils/test-platform-abstraction.cpp
        ../dali/dali-test-suite-utils/test-harness.cpp
        ../dali/dali-test-suite-utils/dali-test-suite-utils.cpp
        ../dali/dali-test-suite-utils/test-application.cpp
        ../dali/dali-test-suite-utils/test-gesture-manager.cpp
        ../dali/dali-test-suite-utils/test-gl-abstraction.cpp
        ../dali/dali-test-suite-utils/test-gl-sync-abstraction.cpp
        ../dali/dali-test-suite-utils/test-native-image.cpp
        ../dali/dali-test-suite-utils/test-render-controller.cpp
        ../dali/dali-test-suite-utils/test-trace-call-stack.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
    dali-core
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -ggdb --coverage -DDEBUG_ENABLED -Wall -Werror=return-type")

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ${${CAPI_LIB}_INCLUDE_DIRS}
    dali-test-suite-utils
    ../dali/dali-test-suite-utils
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
