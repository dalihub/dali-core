# Copyright (c) 2017 Samsung Electronics Co., Ltd.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


SUBDIRS = dali-core/graphics dali-core

pkgconfigdir = $(libdir)/pkgconfig

if ENABLE_CXX03_ABI
pkgconfig_DATA = dali-core-cxx03.pc
else
pkgconfig_DATA = dali-core.pc
endif


MAINTAINERCLEANFILES = \
    aclocal.m4 \
    autom4te.cache \
    config.guess \
    config.sub \
    configure \
    depcomp \
    install-sh \
    ltmain.sh \
    missing \
    `find "$(srcdir)" -type f -name Makefile.in -print` \
    `find . \( -name "*.gcov" -o -name "*.gcno" -o -name "*.gcda" \) -print`

CLEANFILES = \
    `find . \( -name "*.gcov" -o -name "*.gcno" -o -name "*.gcda" \) -print`

COVERAGE_DIR=.cov
COVERAGE_OUTPUT_DIR=doc/coverage

# From lcov version 1.10 onwards, branch coverage is off by default and earlier versions do not support the rc option
LCOV_OPTS=`if [ \`printf "\\\`lcov --version | cut -d' ' -f4\\\`\n1.10\n" | sort -V | head -n 1\` = 1.10 ] ; then echo "--rc lcov_branch_coverage=1" ; fi`

rename_cov_data:
	@test -z $(COVERAGE_DIR) || mkdir -p $(COVERAGE_DIR)
	@rm -f $(COVERAGE_DIR)/*
	@cp dali-core/.libs/*.gcda dali-core/.libs/*.gcno $(COVERAGE_DIR)
	@for i in `find $(COVERAGE_DIR) -name "libdali*_la-*.gcda" -o -name "libdali*_la-*.gcno"` ;\
		do mv $$i `echo $$i | sed s/libdali_.*_la-//` ; echo $$i ; done

cov_data: rename_cov_data
	@cd $(COVERAGE_DIR) ; lcov $(LCOV_OPTS) --base-directory . --directory . -c -o dali.info
	@cd $(COVERAGE_DIR) ; lcov $(LCOV_OPTS) --remove dali.info "*/dali-env/*" "/usr/include/*" "public-api/shader-effects/*" "*/image-actor*" -o dali.info
	@test -z $(COVERAGE_OUTPUT_DIR) || mkdir -p $(COVERAGE_OUTPUT_DIR)

coverage: cov_data
	@genhtml $(LCOV_OPTS) -o $(COVERAGE_OUTPUT_DIR) $(COVERAGE_DIR)/dali.info

reset_coverage:
	@lcov -z --directory `pwd`
