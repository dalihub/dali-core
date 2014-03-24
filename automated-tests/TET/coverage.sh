#!/bin/bash

( cd ../build/slp ; make cov_data )

for i in `find . -name Makefile` ; do
    (
        cd $(dirname $i)
        echo `pwd`
        covs=( `ls *.gcda 2>/dev/null` )
        if [[ $? -eq 0 ]]
        then
            make coverage
        fi
    )
done

(
    cd .. ;
    genhtml -o build/slp/doc/coverage `find . -name dali.info`
)


