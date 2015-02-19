SET(PKG_NAME "dali")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali")

SET(TC_SOURCES
        utc-Dali-ActiveConstraint.cpp
        utc-Dali-Actor.cpp
        utc-Dali-AlphaFunctions.cpp
        utc-Dali-AngleAxis.cpp
        utc-Dali-AnimatableMesh.cpp
        utc-Dali-Animation.cpp
        utc-Dali-Any.cpp
        utc-Dali-Atlas.cpp
        utc-Dali-BaseHandle.cpp
        utc-Dali-BitmapImage.cpp
        utc-Dali-CameraActor.cpp
        utc-Dali-Character.cpp
        utc-Dali-Constrainable.cpp
        utc-Dali-Constraint.cpp
        utc-Dali-Context.cpp
        utc-Dali-CustomActor.cpp
        utc-Dali-Degree.cpp
        utc-Dali-DistanceField.cpp
        utc-Dali-DynamicsBody.cpp
        utc-Dali-DynamicsBodyConfig.cpp
        utc-Dali-DynamicsJoint.cpp
        utc-Dali-DynamicsShape.cpp
        utc-Dali-DynamicsWorld.cpp
        utc-Dali-DynamicsWorldConfig.cpp
        utc-Dali-EncodedBufferImage.cpp
        utc-Dali-Font.cpp
        utc-Dali-FontParameters.cpp
        utc-Dali-FrameBufferImage.cpp
        utc-Dali-Gesture.cpp
        utc-Dali-GestureDetector.cpp
        utc-Dali-Handle.cpp
        utc-Dali-HitTestAlgorithm.cpp
        utc-Dali-HoverProcessing.cpp
        utc-Dali-Image.cpp
        utc-Dali-ImageActor.cpp
        utc-Dali-ImageAttributes.cpp
        utc-Dali-KeyEvent.cpp
        utc-Dali-Layer.cpp
        utc-Dali-LightActor.cpp
        utc-Dali-LocklessBuffer.cpp
        utc-Dali-LongPressGesture.cpp
        utc-Dali-LongPressGestureDetector.cpp
        utc-Dali-Material.cpp
        utc-Dali-MathUtils.cpp
        utc-Dali-Matrix.cpp
        utc-Dali-Matrix3.cpp
        utc-Dali-MeshActor.cpp
        utc-Dali-MeshData.cpp
        utc-Dali-Model.cpp
        utc-Dali-MouseWheelEvent.cpp
        utc-Dali-Mutex.cpp
        utc-Dali-NinePatchImages.cpp
        utc-Dali-ObjectRegistry.cpp
        utc-Dali-PanGesture.cpp
        utc-Dali-PanGestureDetector.cpp
        utc-Dali-Path.cpp
        utc-Dali-PinchGesture.cpp
        utc-Dali-PinchGestureDetector.cpp
        utc-Dali-Pixel.cpp
        utc-Dali-PropertyMap.cpp
        utc-Dali-PropertyNotification.cpp
        utc-Dali-Quaternion.cpp
        utc-Dali-Radian.cpp
        utc-Dali-Random.cpp
        utc-Dali-Rect.cpp
        utc-Dali-RenderTask.cpp
        utc-Dali-RenderTaskList.cpp
        utc-Dali-RenderableActor.cpp
        utc-Dali-ShaderEffect.cpp
        utc-Dali-Scripting.cpp
        utc-Dali-SignalTemplatesFunctors.cpp
        utc-Dali-SignalTemplates.cpp
        utc-Dali-Spline.cpp
        utc-Dali-Stage.cpp
        utc-Dali-TapGesture.cpp
        utc-Dali-TapGestureDetector.cpp
        utc-Dali-Text.cpp
        utc-Dali-TextActor.cpp
        utc-Dali-TextStyle.cpp
        utc-Dali-TouchEventCombiner.cpp
        utc-Dali-TouchProcessing.cpp
        utc-Dali-TypeRegistry.cpp
        utc-Dali-Utf8.cpp
        utc-Dali-Vector.cpp
        utc-Dali-Vector2.cpp
        utc-Dali-Vector3.cpp
        utc-Dali-Vector4.cpp
)

LIST(APPEND TC_SOURCES
        dali-test-suite-utils/mesh-builder.cpp
        dali-test-suite-utils/test-harness.cpp
        dali-test-suite-utils/dali-test-suite-utils.cpp
        dali-test-suite-utils/test-application.cpp
        dali-test-suite-utils/test-dynamics.cpp
        dali-test-suite-utils/test-gesture-manager.cpp
        dali-test-suite-utils/test-gl-abstraction.cpp
        dali-test-suite-utils/test-gl-sync-abstraction.cpp
        dali-test-suite-utils/test-native-image.cpp
        dali-test-suite-utils/test-platform-abstraction.cpp
        dali-test-suite-utils/test-render-controller.cpp
        dali-test-suite-utils/test-trace-call-stack.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
     dali-core
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -ggdb --coverage -Wall -Werror=return-type")

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ${${CAPI_LIB}_INCLUDE_DIRS}
    dali-test-suite-utils
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    -lpthread
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
