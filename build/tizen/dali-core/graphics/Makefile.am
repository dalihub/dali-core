# Copyright (c) 2016 Samsung Electronics Co., Ltd.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Parse each file.list
# These files define <module>_src_files & <module>_header_files,
# where <module> is the subdirectory from core_src_dir

graphics_api_src_dir = ../../../../dali/graphics-api
include ../../../../dali/graphics-api/file.list

graphics_src_dir = ../../../../dali/graphics
include ../../../../dali/graphics/file.list


# Build the graphics library
noinst_LIBRARIES = libdali-graphics.a

libdali_graphics_a_SOURCES = $(graphics_api_src_files) \
                             $(graphics_src_files)

#libdali_graphics_a_LDFLAGS = -fPIC


dali_core_includes = \
                     -I../../../..


gcc_flags = -Wno-return-local-addr -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override \
            -Wstack-usage=256 -Wunsafe-loop-optimizations -Wzero-as-null-pointer-constant -Wuseless-cast

cxx_more_warnings = -Wold-style-cast -Woverloaded-virtual -Wdouble-promotion -Wswitch-enum \
                    -Wfloat-equal -Wshadow \
                    -Wlarger-than=256 -Wframe-larger-than=128 \
                    -Wcast-qual -Wcast-align \
                    -Wconversion -Wsign-conversion \
                    -W

# the following warnings should not be enforced
cxx_warnings_to_remove = \
                         -Wno-c++98-compat

#the following warnings should be added back when possible
cxx_warnings_to_preserve = \
                           -Wno-weak-vtables

cxx_flags = -fPIC -std=c++14 -Werror -Wall -Wextra -Weffc++ -Wpedantic \
            $(cxx_more_warnings) $(cxx_warnings_to_remove) $(cxx_warnings_to_preserve)

libdali_graphics_a_CXXFLAGS = $(cxx_flags) \
                              $(dali_core_includes) \
                              $(DALI_CFLAGS)


