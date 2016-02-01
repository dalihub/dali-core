#!/bin/bash


TEMP=`getopt -o rn --long rebuild,no-gen \
     -n 'genmake' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

opt_rebuild=false
opt_generate=true

while true ; do
    case "$1" in
        -r|--rebuild) opt_rebuild=true ; shift ;;
        -n|--no-gen)  opt_generate=false ; shift ;;
        --) shift ; break ;;
        *) shift ;;   # Ignore
    esac
done

if [ false == $opt_rebuild -o ! -d "build" ] ; then
    rm -rf build
    mkdir build
fi

function build
{
    if [ $opt_generate == true -o $opt_rebuild == false ] ; then
        (cd src/$1; ../../scripts/tcheadgen.sh tct-$1-core.h)
        if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
    fi
    (cd build ; cmake .. -DMODULE=$1 ; make -j7 )
}

if [ -n "$1" ] ; then
  echo BUILDING ONLY $1
  build $1
else
  for mod in `ls -1 src/ | grep -v CMakeList `
  do
    if [ $mod != 'common' ] && [ $mod != 'manual' ]; then
        echo BUILDING $mod
        build $mod
        if [ $? -ne 0 ]; then echo "Build failed" ; exit 1; fi
    fi
  done
fi

echo "Build succeeded"
exit 0
