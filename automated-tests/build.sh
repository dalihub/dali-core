#!/bin/bash

rm -rf build
mkdir build

function build
{
  (cd src/$1; ../../scripts/tcheadgen.sh tct-$1-core.h)
  if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
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
    fi
  done
fi
