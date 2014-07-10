#!/bin/bash

function execute
{
    scripts/tctestsgen.sh $1 `pwd` desktop $2
    testkit-lite -f `pwd`/tests.xml -o tct-${1}-core-tests.xml  -A --comm localhost
    scripts/add_style.pl $1
}

# Clean up old test results
rm -f tct*core-tests.xml

# Clean up old coverage data
if [ -d ../build/slp ] ; then
    rm -f ../build/slp/dali-core/.libs/*.gcda
    rm -f ../build/slp/dali-core/.libs/*.gcno
elif [ -d ../build/tizen ] ; then
    rm -f ../build/tizen/dali-core/.libs/*.gcda
    rm -f ../build/tizen/dali-core/.libs/*.gcno
fi

find build \( -name "*.gcda" -o -name "*.gcno" \) -exec rm '{}' \;

if [ -n "$1" ] ; then
  echo EXECUTING ONLY $1
  execute $*

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
