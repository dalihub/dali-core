#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Flora License, Version 1.0 (the License);
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://floralicense.org/license/

#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

CC = g++

TARGETS =
include file.list

PKGS = dali-core
include ../../rules.mk
include ../../coverage.mk

all: $(TARGETS)

%: %.cpp
	$(CC) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

clean:
	@rm -f $(notdir $(TARGETS))
	@rm -f tet_captured
	@rm -f *~
	@rm -f *.gcda *.gcno

coverage:
	@lcov --directory . -c -o dali.info
	@lcov --remove dali.info "*boost*" "/usr/include/*" "*/automated-tests/*" -o dali.info
