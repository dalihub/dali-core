#!/bin/bash
NAME=$(basename $(cd $(dirname $0);pwd))
PKG_DIR=%{PKG_DIR} # directory supplied by external script
PKG_NAME=%{PKG_NAME} # name supplied by external script
PKG_FULLNAME=%{PKG_FULLNAME} # name supplied by external script

#parse params
USAGE="Usage: ./inst.sh [-i] [-u]
  -i install wgt and config environment
  -u uninstall wgt and remove source file
[-i] option was set as default."

function installpkg(){
    rpm -e `rpm -qa | grep $PKG_NAME`
    rpm -ivh --quiet /$PKG_DIR/$PKG_FULLNAME
    /tmp/add_all_smack_rule.sh
}

function uninstallpkg(){
### remove source file ###
if [ -d /opt/usr/media/tct/opt/$NAME ];then
    rm -rf /opt/usr/media/tct/opt/$NAME
    rpm -e $PKG_NAME
else
    echo "Remove source file fail, please check if the source file exist: /opt/usr/media/tct/opt/$NAME ..."
fi
}

case "$1" in
    -h|--help) echo "$USAGE"
               exit ;;
    ""|-i) installpkg;;
    -u) uninstallpkg;;
    *) echo "Unknown option: $1"
       echo "$USAGE"
       exit ;;
esac
