#!/bin/bash

TEMP=`getopt -o sr --long serial,rerun -n 'execute.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

opt_parallel=1
opt_rerun=""
while true ; do
    case "$1" in
        -s|--serial)   opt_parallel=0 ; shift ;;
        -r|--rerun)    opt_rerun="-r" ; shift ;;
        --) shift; break;;
	*) echo "Internal error $1!" ; exit 1 ;;
    esac
done

function execute
{
    scripts/tctestsgen.sh $1 `pwd` desktop $2
    testkit-lite -f `pwd`/tests.xml -o tct-${1}-core-tests.xml  -A --comm localhost
    scripts/add_style.pl $1
}

# Clean up old test results
rm -f tct*core-tests.xml

# Clean up old coverage data
if [ -d ../build/tizen ] ; then
    rm -f ../build/tizen/dali-core/.libs/*.gcda
fi

find build \( -name "*.gcda" \) -exec rm '{}' \;

if [ $opt_parallel = 1 ] ; then

    if [ -n "$1" ] ; then
        if [ -f "build/src/$1/tct-$1-core" ] ; then
            build/src/$1/tct-$1-core -p $opt_rerun
        fi
    else
        for mod in `ls -1 src/ | grep -v CMakeList `
        do
            if [ $mod != 'common' ] && [ $mod != 'manual' ]; then
                echo EXECUTING $mod
                build/src/$mod/tct-$mod-core -p $opt_rerun
            fi
        done
    fi

else
    if [ -n "$1" ] ; then
        execute $1 $*
    else
        for mod in `ls -1 src/ | grep -v CMakeList `
        do
            if [ $mod != 'common' ] && [ $mod != 'manual' ]; then
                echo EXECUTING $mod
                execute $mod $*
            fi
        done
    fi

    scripts/summarize.pl
fi
