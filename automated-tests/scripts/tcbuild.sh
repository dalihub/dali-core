#!/bin/bash

#---------- DEBUG_BEGIN ----------
#ARG="-d" # debug-on flag, might be set as $1
# keyprompt "introductory message" -- wait until any key pressed
function keyprompt { echo -ne "\n\e[1;31m$1 -- " && read -n 1 && echo -e "\n\e[0m"; }
# d_bp -- breakpoint at which user need to press any key to proceed
function d_bp { if [[ "$ARG" == "-d" ]]; then keyprompt "d >> Press any key"; fi }
# d_showVar VARNAME -- print bash variable name
function d_showVar { if [ "$ARG" == "-d" -a -n "$1" ]; then echo "d >> ${1} = ${!1}"; fi }
# d_print "message" -- print a debug message
function d_print { if [ "$ARG" == "-d" -a -n "$1" ]; then echo -e "d >> $1"; fi }
#----------  DEBUG_END  ----------

PROJECT_DIR="$(cd "$(dirname $0)" && pwd)"
d_showVar PROJECT_DIR

function gbs_profile {
perl -e "
use Config::Tiny;
my \$Config = Config::Tiny->read( \"\$ENV{HOME}/.gbs.conf\" );
my \$profile = \$Config->{general}->{profile};
\$profile =~ s/profile.//;
print \$profile;"
}

PROFILE=`gbs_profile`
RPM_DIR="$HOME/GBS-ROOT/local/repos/$PROFILE/armv7l/RPMS"
d_showVar RPM_DIR

function add_module {
    # argument check
    if [ -z "$1" ]; then echo "Usage: `basename $0` addmod <module_name> [module_lib_name]"; exit 1; fi

    MODULE_NAME=$1
    d_showVar MODULE_NAME
    MODULE_NAME_C=$(echo $MODULE_NAME | sed -e 's/-\([a-z]\)/\U\1/' -e 's/^\([a-z]\)/\U\1/')
    d_showVar MODULE_NAME_C
    MODULE_NAME_U=$(echo $MODULE_NAME | sed -e 's/-/_/')
    d_showVar MODULE_NAME_U
#    MODULE_LIBNAME=${2:-capi-$MODULE_NAME}
    MODULE_LIBNAME=$1
    d_showVar MODULE_LIBNAME

    echo "Adding $MODULE_NAME module to project..."
    d_bp
    cd $PROJECT_DIR
    # prepare .spec file
    echo "-- Generating packaging/core-$MODULE_NAME-tests.spec file"
    if [ ! -d packaging ]; then mkdir packaging; fi
    sed -e "s:\[MODULE_NAME\]:$MODULE_NAME:g" -e "s:\[MODULE_LIBNAME\]:$MODULE_LIBNAME:g" \
        templates/core-\[module_name\]-tests.spec > packaging/core-$MODULE_NAME-tests.spec
    # prepare src directory
    mkdir src/$MODULE_NAME
    echo "-- Generating src/$MODULE_NAME/CMakeLists.txt file"
    sed -e "s:%{MODULE_NAME}:$MODULE_NAME:g" -e "s:%{MODULE_LIBNAME}:$MODULE_LIBNAME:g" \
        templates/src-directory/CMakeLists.txt > src/$MODULE_NAME/CMakeLists.txt
    echo "-- Generating src/$MODULE_NAME/tct-$MODULE_NAME-core.c file"
    sed -e "s:%{MODULE_NAME}:$MODULE_NAME:g" \
        templates/src-directory/tct-\[module_name\]-core.c > src/$MODULE_NAME/tct-$MODULE_NAME-core.c
    echo "-- Generating src/$MODULE_NAME/utc-$MODULE_NAME.c file"
    sed -e "s:%{MODULE_NAME_U}:$MODULE_NAME_U:g" -e "s:%{MODULE_NAME_C}:$MODULE_NAME_C:g" \
        templates/src-directory/utc-\[module_name\].c > src/$MODULE_NAME/utc-$MODULE_NAME.c
    echo "Task finished successfully"
}

function rm_module {
    # argument check
    if [ -z "$1" ]; then echo "Usage: `basename $0` rmmod <module_name>"; exit 1; fi

    MODULE_NAME=$1
    d_showVar MODULE_NAME

    echo "Removing $MODULE_NAME module from project..."
    d_bp
    echo "---- Updating /opt/tct/packages/package_list.xml"
    scripts/tcpackageslistsgen.sh $MODULE_NAME /opt/tct/packages/package_list.xml 1
    if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
   # echo "---- Updating test plans"
   # scripts/tcpackageslistsgen.sh $MODULE_NAME /opt/tct/manager/plan/*.xml 1
   # if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi

    cd $PROJECT_DIR
    echo "-- Removing packaging/core-$MODULE_NAME-tests.spec file"
    rm packaging/core-$MODULE_NAME-tests.spec
    echo "-- Removing src/$MODULE_NAME directory"
    rm -r src/$MODULE_NAME
    echo "Task finished successfully"
}

function build {
    if [ -n "$1" ]; then
        (cd src/$1; ../../scripts/tcheadgen.sh tct-$1-core.h)
        if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
        cp packaging/core-$1-tests.spec ../packaging
        gbs build -A armv7l --spec core-$1-tests.spec --include-all --keep-packs | \
            tee build.log | stdbuf -o0 sed -e 's/error:/\x1b[1;91m&\x1b[0m/' \
                                           -e 's/warning:/\x1b[93m&\x1b[0m/'
        rm ../packaging/core-$1-tests.spec
    else
        echo "Build requires a module name"
        exit 1
    fi
}

function inst {
    if [ -z "$1" ]
    then
        for mod in `ls -1 src/ | grep -v CMakeLists`
        do

	if [ $mod != 'common' ] && [ $mod != 'manual' ]; then

            PKG_NAME="core-$mod-tests"
            d_showVar PKG_NAME
            VER=$(cat packaging/$PKG_NAME.spec | awk '/^Version:/ { print $2; exit; }')
            d_showVar VER
            PKG_VNAME="$PKG_NAME-$VER"
            d_showVar PKG_VNAME
            PKG_FNAME="$PKG_VNAME-0.armv7l.rpm"
            d_showVar PKG_FNAME

            if [ -f "$RPM_DIR/$PKG_FNAME" ]
            then
                inst $mod
                echo ""
            fi
	fi
        done
    else
        cd $PROJECT_DIR
        # setting variables
        MOD_NAME="$1"
        d_showVar MOD_NAME
        PKG_NAME="core-$MOD_NAME-tests"
        d_showVar PKG_NAME
        VER=$(cat packaging/$PKG_NAME.spec | awk '/^Version:/ { print $2; exit; }')
        d_showVar VER
        PKG_VNAME="$PKG_NAME-$VER"
        d_showVar PKG_VNAME
        PKG_FNAME="$PKG_VNAME-0.armv7l.rpm"
        d_showVar PKG_FNAME
        TCT_DIR="opt/tct-$MOD_NAME-core-tests"
        d_showVar TCT_DIR

        echo "Deploying $MOD_NAME suite to tct-mgr..."
        d_bp
        # prepare tct directory and files
        echo "-- Preparing suite .zip file..."
        echo "---- Creating /tmp/$TCT_DIR directory"
        rm -r /tmp/opt > /dev/null 2>&1
        mkdir -p /tmp/$TCT_DIR
        # README
        echo "---- Copying /tmp/$TCT_DIR"
        cp templates/tct-package/README /tmp/$TCT_DIR
        # rpm
        echo "---- Copying /tmp/$TCT_DIR package"
        cp $RPM_DIR/$PKG_FNAME /tmp/$TCT_DIR
        if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
        # inst.sh
        echo "---- Generating /tmp/$TCT_DIR/inst.sh file"
        sed -e "s:%{PKG_NAME}:\"$PKG_NAME\":g" -e "s:%{PKG_FULLNAME}:\"$PKG_FNAME\":g" \
            -e "s:%{PKG_DIR}:\"/opt/usr/media/tct/$TCT_DIR\":g" \
            templates/tct-package/inst.sh > /tmp/$TCT_DIR/inst.sh
        chmod a+x /tmp/$TCT_DIR/inst.sh
        # tests.xml
        echo "---- Generating /tmp/$TCT_DIR"
        scripts/tctestsgen.sh $MOD_NAME /tmp/$TCT_DIR target
        if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
        # zip
        echo "---- Preparing /tmp/tct/packages/tct-$MOD_NAME-core-tests-2.2.1-1.zip file"
        # clear old zips
        rm -r /tmp/tct/packages > /dev/null 2>&1
        mkdir -p /tmp/tct/packages
        # create new zip
        ( cd /tmp; zip -r /tmp/tct/packages/tct-$MOD_NAME-core-tests-2.2.1-1.zip opt > /dev/null 2>&1; )
        # deployment
        echo "-- Suite deployment..."
        echo "---- Copying /opt/tct/packages/tct-$MOD_NAME-core-tests-2.2.1-1.zip"
        cp /tmp/tct/packages/tct-$MOD_NAME-core-tests-2.2.1-1.zip /opt/tct/packages/
        echo "---- Updating /opt/tct/packages/package_list.xml"
        scripts/tcpackageslistsgen.sh $MOD_NAME /opt/tct/packages/package_list.xml 0
        if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
       # echo "---- Updating test plans"
       # for file in `grep -r tct-$MOD_NAME-core-tests /opt/tct/manager/plan/ | cut -d: -f1 | uniq`
       # do
       #     scripts/tcpackageslistsgen.sh $MOD_NAME $file
       # done
       # scripts/tcpackageslistsgen.sh $MOD_NAME /opt/tct/manager/plan/Full_test.xml
       # if [ $? -ne 0 ]; then echo "Aborting..."; exit 1; fi
        echo "Task finished successfully"
    fi
}

if [ -z "$1" ]; then
    # usage note
    echo "Usage: `basename $0` <addmod|rmmod|build|install> <module_name> [module_lib_name]"
    exit 1
elif [ "addmod" == "$1" ]; then
    # add new module
    add_module $2 $3
elif [ "rmmod" == "$1" ]; then
    # remove module
    rm_module $2
elif [ "build" == "$1" ]; then
    # build the binary
    build $2
elif [ "install" == "$1" ]; then
    # install
    inst $2
else
    echo "Invalid subcommand: $1"
fi
