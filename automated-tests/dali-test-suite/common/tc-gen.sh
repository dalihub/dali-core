#!/bin/sh

TMPSTR=$0
SCRIPT=${TMPSTR##*/}

if [ $# -ne 1 ]; then
	echo "Usage) $SCRIPT ClassName"
	exit 1
fi

MODULE="Dali"
SECTION=${PWD##*/}
CLASS=$1

TESTSUITEPATH=`dirname $PWD`
TESTSUITENAME=${TESTSUITEPATH##*/}

TEMPLATE=../utc-MODULE-CLASS.cpp.in
TESTCASE=utc-${MODULE}-${CLASS}

# Create .cpp file
if [ ! -e "$TESTCASE.cpp" ]; then
	sed -e	'
		s^@CLASS@^'"$CLASS"'^g
		s^@MODULE@^'"$MODULE"'^g
		' $TEMPLATE > $TESTCASE.cpp
fi

if [ ! -e "$TESTCASE.cpp" ]; then
	echo "Failed"
	exit 1
fi


# file.list
if ! [ -f file.list ]; then
	touch file.list
	echo "TARGETS += \\" >> file.list
fi
echo "        $TESTCASE \\" >> file.list

# tslist
if ! [ -f tslist ]; then
	touch tslist
fi
echo "/$TESTSUITENAME/$SECTION/$TESTCASE" >> tslist

# Makefile
if ! [ -f Makefile ]; then
	ln -s ../master-makefile.mk Makefile
fi

echo "$TESTCASE" >> .gitignore

echo "Testcase file is $TESTCASE.cpp"
echo "$TESTCASE is added to tslist"
echo "$TESTCASE is added to file.list"
echo "Done"
