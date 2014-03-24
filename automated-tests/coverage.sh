#!/bin/bash

( cd ../build/slp ; make cov_data )


for i in `find . -name "*.dir"` ; do
    (
        cd $i
        echo `pwd`
        covs=( `ls *.gcda 2>/dev/null` )
        if [[ $? -eq 0 ]]
        then
            lcov --directory . -c -o dali.info
            lcov --remove dali.info "*boost*" "/usr/include/*" "*/automated-tests/*" -o dali.info
        fi
    )
done

(
    cd .. ;
    genhtml -o build/slp/doc/coverage `find . -name dali.info`
)
