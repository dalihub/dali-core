#!/bin/bash

opt_genhtml=true
if [ "$1" == "-n" ] ; then
  opt_genhtml=false
fi

BUILD_DIR_NAME=tizen

BUILDSYSTEM=make

if [ -e ../build/tizen/build.ninja ] ; then
    BUILDSYSTEM=ninja
fi

(  cd ../build/$BUILD_DIR_NAME ; $BUILDSYSTEM cov_data )


# fastcov.py doesn't require version detection or complex rc options
FASTCOV_OPTS="--branch-coverage --lcov"
echo "Using fastcov.py for coverage data collection"

for i in `find . -name "*.dir"` ; do
    (
        cd $i
        echo `pwd`
        covs=( `ls *.gcda 2>/dev/null` )
        if [[ $? -eq 0 ]]
        then
            ${DESKTOP_PREFIX}/bin/fastcov.py $FASTCOV_OPTS \
                -d . \
                --exclude /usr/include automated-tests dali-env \
                -o dali.info
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
        genhtml --branch-coverage -o build/$BUILD_DIR_NAME/doc/coverage `find . -name dali.info`
        echo "Coverage output: ../build/$BUILD_DIR_NAME/doc/coverage/index.html"
    fi
)
