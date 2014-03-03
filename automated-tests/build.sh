#!/bin/bash

TEMP=`getopt -o 2vds: --long 2,verbose,desktop,scenario: \
     -n 'build_out.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

scenario=all
opt_verbose=0
opt_env=scratchbox

while true ; do
    case "$1" in
	-d|--desktop)   opt_env=desktop ; shift ;;
	-s|--scenario)  scenario="$2"   ; shift 2 ;;
	-v|--verbose)   opt_verbose=1   ; shift ;;
	-2|--2)         opt_env=sbs     ; shift ;;
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1 ;;
    esac
done


case "$opt_env" in
    desktop)
	. _export_desktop.sh
	cat <<EOF > coverage.mk
LDFLAGS += --coverage
EOF
	;;
    scratchbox)
	. _export_env.sh
	cat <<EOF > coverage.mk
LDFLAGS +=
EOF
	;;
    sbs)
	. _export_sbs.sh
	cat <<EOF > coverage.mk
LDFLAGS +=
EOF
	;;
esac


echo PATH=$PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo TET_ROOT=$TET_ROOT
echo TET_SUITE_ROOT=$TET_SUITE_ROOT
echo ARCH=$ARCH

RESULT_DIR=results-$ARCH
HTML_RESULT=$RESULT_DIR/build-tar-result-$FILE_NAME_EXTENSION.html
JOURNAL_RESULT=$RESULT_DIR/build-tar-result-$FILE_NAME_EXTENSION.journal

# Faster cleanup.
find . -name Makefile -execdir make -f {} clean \;

mkdir -p $RESULT_DIR
if [ $opt_verbose -eq 1 ] ; then
    tcc -b -j -  ./  $scenario | tee $JOURNAL_RESULT
else
    tcc -b -j $JOURNAL_RESULT -p ./  $scenario
fi
./tbp.pl $JOURNAL_RESULT
