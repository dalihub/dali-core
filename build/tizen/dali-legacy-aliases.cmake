# DALi 2.x names on a DALi 3 installation.
#
# DALi 3 installs under versioned names and paths (libdali3-core,
# include/dali3, share/dali3, dali3-<tool>) so that it can sit beside a DALi
# 2.x installed from its own source. While applications still build and link
# against the 2.x names, DALI_LEGACY_ALIASES=ON also installs those names as
# links to the dali3 files: a binary built against 2.x loads the dali3 library
# through libdali2-*.so.2, and an application linked against the alias
# records the dali3 SONAME, so it becomes a dali3 binary without a source
# change.
#
# The option is OFF by default, except on Windows, where the applications
# still load the 2.x names and a symbolic link cannot be created, so the
# files are copied under the legacy name instead. The Tizen spec turns it on
# for the targets whose applications are still migrating, and never for
# Tizen 10.0 and below, where the real DALi 2.x is installed.
#
# This file is kept identical in every DALi repository.

# WIN32 is only set by PROJECT(), and this file may be included before it;
# CMAKE_SYSTEM_NAME (set by PROJECT() or a toolchain file) and the host
# answer the question either way.
IF(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR (CMAKE_HOST_WIN32 AND NOT CMAKE_SYSTEM_NAME))
  SET(DALI_LEGACY_ALIASES_DEFAULT ON)
ELSE()
  SET(DALI_LEGACY_ALIASES_DEFAULT OFF)
ENDIF()
OPTION(DALI_LEGACY_ALIASES "Also install the DALi 2.x names as links to the dali3 files" ${DALI_LEGACY_ALIASES_DEFAULT})

# dali_legacy_alias(<link> <target>)
#   <link>   install path of the alias; relative paths are taken from
#            CMAKE_INSTALL_PREFIX, and DESTDIR is honoured at install time
#   <target> what the link points at, relative to the link's own directory
FUNCTION(dali_legacy_alias link target)
  IF(NOT DALI_LEGACY_ALIASES)
    RETURN()
  ENDIF()
  IF(NOT IS_ABSOLUTE "${link}")
    SET(link "${CMAKE_INSTALL_PREFIX}/${link}")
  ENDIF()
  # <target> may be a generator expression naming an installed file, which
  # install(CODE) expands only under this policy. Set here, it holds wherever
  # the function is called, including sub-projects with their own
  # cmake_minimum_required.
  IF(POLICY CMP0087)
    CMAKE_POLICY(SET CMP0087 NEW)
  ENDIF()
  INSTALL(CODE "
    set(_link \"\$ENV{DESTDIR}${link}\")
    get_filename_component(_dir \"\${_link}\" DIRECTORY)
    file(MAKE_DIRECTORY \"\${_dir}\")
    if(IS_SYMLINK \"\${_link}\" OR NOT IS_DIRECTORY \"\${_link}\")
      file(REMOVE \"\${_link}\")
    else()
      file(REMOVE_RECURSE \"\${_link}\")
    endif()
    file(CREATE_LINK \"${target}\" \"\${_link}\" SYMBOLIC RESULT _result)
    if(NOT _result EQUAL 0)
      get_filename_component(_abs \"\${_dir}/${target}\" ABSOLUTE)
      if(IS_DIRECTORY \"\${_abs}\")
        execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${_abs}\" \"\${_link}\")
      else()
        execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy \"\${_abs}\" \"\${_link}\")
      endif()
    endif()
    message(STATUS \"Legacy alias: \${_link} -> ${target}\")
  ")
ENDFUNCTION()

# dali_legacy_library_alias(<target> <legacy name> [<legacy soversion>])
#   Installs lib<legacy name>.so beside the target's library, pointing at its
#   link-time file, and lib<legacy name>.so.<legacy soversion> pointing at its
#   SONAME file. Without a soversion (plugins), lib<legacy name>.so points at
#   the library file itself. On Windows the DLL and the import library are
#   copied under the legacy name. Uses the caller's LIB_DIR and BIN_DIR.
FUNCTION(dali_legacy_library_alias tgt legacy)
  IF(NOT DALI_LEGACY_ALIASES)
    RETURN()
  ENDIF()
  IF(WIN32)
    dali_legacy_alias("${BIN_DIR}/${legacy}.dll" "$<TARGET_FILE_NAME:${tgt}>")
    dali_legacy_alias("${LIB_DIR}/${legacy}.lib" "$<TARGET_LINKER_FILE_NAME:${tgt}>")
  ELSEIF(ARGC GREATER 2)
    dali_legacy_alias("${LIB_DIR}/lib${legacy}.so" "$<TARGET_LINKER_FILE_NAME:${tgt}>")
    dali_legacy_alias("${LIB_DIR}/lib${legacy}.so.${ARGV2}" "$<TARGET_SONAME_FILE_NAME:${tgt}>")
  ELSE()
    dali_legacy_alias("${LIB_DIR}/lib${legacy}.so" "$<TARGET_FILE_NAME:${tgt}>")
  ENDIF()
ENDFUNCTION()

# dali_legacy_pkgconfig_alias(<pc file> <legacy name>)
#   Installs a copy of the pkg-config file as <legacy name>.pc. A copy, not a
#   link, so that rpm derives pkgconfig(<legacy name>) from it. The content
#   needs no change: it already names the dali3 library and include directory.
FUNCTION(dali_legacy_pkgconfig_alias pcfile legacy)
  IF(NOT DALI_LEGACY_ALIASES)
    RETURN()
  ENDIF()
  INSTALL(FILES ${pcfile} DESTINATION ${LIB_DIR}/pkgconfig RENAME ${legacy}.pc)
ENDFUNCTION()

# dali_legacy_include_alias(<name>)
#   include/<name> -> include/dali3/<name>, from the caller's INCLUDE_DIR
#   (which ends in the versioned subdirectory).
FUNCTION(dali_legacy_include_alias name)
  IF(NOT DALI_LEGACY_ALIASES)
    RETURN()
  ENDIF()
  GET_FILENAME_COMPONENT(_root "${INCLUDE_DIR}" DIRECTORY)
  GET_FILENAME_COMPONENT(_sub "${INCLUDE_DIR}" NAME)
  dali_legacy_alias("${_root}/${name}" "${_sub}/${name}")
ENDFUNCTION()

# dali_legacy_cmake_package_alias(<name> <legacy> [<extra lines>])
#   Installs share/<legacy>/<legacy>-config.cmake, which loads the dali3
#   CMake package and offers its target under the legacy name
#   (<legacy>::<legacy> -> <name>::<name>), plus any extra lines given.
#   Ubuntu and Windows only: the Tizen packages ship no CMake package files.
FUNCTION(dali_legacy_cmake_package_alias name legacy)
  IF(NOT DALI_LEGACY_ALIASES)
    RETURN()
  ENDIF()
  SET(_extra "")
  IF(ARGC GREATER 2)
    SET(_extra "${ARGV2}")
  ENDIF()
  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${legacy}-config.cmake "
include(\${CMAKE_CURRENT_LIST_DIR}/../${name}/${name}-config.cmake)
if(NOT TARGET ${legacy}::${legacy})
  add_library(${legacy}::${legacy} INTERFACE IMPORTED)
  set_target_properties(${legacy}::${legacy} PROPERTIES INTERFACE_LINK_LIBRARIES ${name}::${name})
endif()
${_extra}
")
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${legacy}-config.cmake DESTINATION share/${legacy})
ENDFUNCTION()
