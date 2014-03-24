#!/bin/sh

TEMP=`getopt -o ds: --long desktop,scenario: \
     -n 'build_out.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

scenario=all
env=./_export_env.sh

while true ; do
    case "$1" in
	-d|--desktop)   env=./_export_desktop.sh ; shift ;;
	-s|--scenario)  scenario="$2" ; shift 2 ;;
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1 ;;
	esac
done


# Source correct environment
. $env

echo PATH=$PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo TET_ROOT=$TET_ROOT
echo TET_SUITE_ROOT=$TET_SUITE_ROOT
echo ARCH=$ARCH

RESULT_DIR=results-$ARCH
HTML_RESULT=$RESULT_DIR/exec-tar-result-$FILE_NAME_EXTENSION.html
JOURNAL_RESULT=$RESULT_DIR/exec-tar-result-$FILE_NAME_EXTENSION.journal

mkdir -p $RESULT_DIR

tmp_script=/tmp/tetexec$$

cat > $tmp_script <<!EOF
LD_LIBRARY_PATH=$TET_ROOT/lib/tet3:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
gdb \$*
!EOF


chmod 755 $tmp_script
tcc -v TET_XTERM=/usr/bin/rxvt-xpm -v TET_XTERM_DISPLAY=192.168.129.2:0 -v TET_EXEC_TOOL=$TET_ROOT/bin/tet_start -v TET_EXEC_FILE=$tmp_script -e -j - ./ $scenario

rm $tmp_script
