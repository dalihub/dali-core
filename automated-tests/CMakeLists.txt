CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
PROJECT(tct_coreapi_utc)
SET(CMAKE_CXX_STANDARD 17)

INCLUDE(FindPkgConfig)
SET(BIN_DIR "/opt/usr/bin")

INCLUDE_DIRECTORIES(
  src/common
)

ADD_SUBDIRECTORY(src)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ")
