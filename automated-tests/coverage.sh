#!/bin/bash

opt_genhtml=true
if [ $1 == -n ] ; then
  opt_genhtml=false
fi

BUILD_DIR_NAME=tizen
function MakeCovData()
{
    (  cd ../build/$BUILD_DIR_NAME ; make cov_data )
}

MakeCovData
if [[ $? -ne 0 ]]
then
    BUILD_DIR_NAME=tizen-cmake
    MakeCovData
fi

# From lcov version 1.10 onwards, branch coverage is off by default and earlier versions do not support the rc option
LCOV_OPTS=`if [ \`printf "\\\`lcov --version | cut -d' ' -f4\\\`\n1.10\n" | sort -V | head -n 1\` = 1.10 ] ; then echo "--rc lcov_branch_coverage=1" ; fi`

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
