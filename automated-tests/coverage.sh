#!/bin/bash

opt_genhtml=true
if [ $1 == -n ] ; then
  opt_genhtml=false
fi

BUILD_DIR_NAME=tizen

BUILDSYSTEM=make

if [ -e ../build/tizen/build.ninja ] ; then
    BUILDSYSTEM=ninja
fi

(  cd ../build/$BUILD_DIR_NAME ; $BUILDSYSTEM cov_data )


# From lcov version 1.10 onwards, branch coverage is off by default and earlier versions do not support the rc option
LCOV_VERSION=`lcov --version | cut -d' ' -f4`
LCOV_OPTS=""

if [ `printf "$LCOV_VERSION\n1.10\n" | sort -V | head -n 1` = 1.10 ] ; then
    # lcov version 2.0 and later have different options for branch coverage, and need to ignore some errors
    if [ `printf "$LCOV_VERSION\n2.0\n" | sort -V | head -n 1` = 2.0 ] ; then
        LCOV_OPTS="--rc branch_coverage=1 --rc geninfo_unexecuted_blocks=1 --rc no_exception_branch=1 --ignore-errors unused --ignore-errors empty --ignore-errors negative --ignore-errors mismatch"
    else
        LCOV_OPTS="--rc lcov_branch_coverage=1"
    fi
fi
echo "lcov version $LCOV_VERSION detected"
echo "lcov option = \"$LCOV_OPTS\""

for i in `find . -name "*.dir"` ; do
    (
        cd $i
        echo `pwd`
        covs=( `ls *.gcda 2>/dev/null` )
        if [[ $? -eq 0 ]]
        then
            lcov $LCOV_OPTS --directory . -c -o dali.info
            lcov $LCOV_OPTS --remove dali.info "/usr/include/*" "*/automated-tests/*" "*/dali-env/*" -o dali.info
            if [ ! -s dali.info ]
            then
              rm -f dali.info
            fi
        fi
    )
done

(
    if [ $opt_genhtml == true ] ; then
        cd .. ;
        genhtml $LCOV_OPTS -o build/$BUILD_DIR_NAME/doc/coverage `find . -name dali.info`
        echo "Coverage output: ../build/$BUILD_DIR_NAME/doc/coverage/index.html"
    fi
)
