SET(PKG_NAME "dali-internal")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-internal")

SET(TC_SOURCES
        utc-Dali-Internal-Font.cpp
        utc-Dali-Internal-Handles.cpp
        utc-Dali-Internal-ImageFactory.cpp
        utc-Dali-Internal-Material.cpp
        utc-Dali-Internal-Text.cpp
        utc-Dali-Internal-ResourceClient.cpp
        utc-Dali-Internal-Image-Culling.cpp
)

LIST(APPEND TC_SOURCES
        ../dali/dali-test-suite-utils/dali-test-suite-utils.cpp
        ../dali/dali-test-suite-utils/test-application.cpp
        ../dali/dali-test-suite-utils/test-gesture-manager.cpp
        ../dali/dali-test-suite-utils/test-gl-abstraction.cpp
        ../dali/dali-test-suite-utils/test-gl-sync-abstraction.cpp
        ../dali/dali-test-suite-utils/test-native-image.cpp
        ../dali/dali-test-suite-utils/test-platform-abstraction.cpp
        ../dali/dali-test-suite-utils/test-render-controller.cpp
        ../dali/dali-test-suite-utils/test-trace-call-stack.cpp
        ../dali-unmanaged/dali-test-suite-utils/mesh-builder.cpp
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
    ../dali/dali-test-suite-utils
    ../dali-unmanaged/dali-test-suite-utils
    ../../..
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
