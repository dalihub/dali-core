#!/bin/bash

if [ -z $1 -o -z $2 ]; then
    echo "Usage note: tcpackageslistsgen.sh <module_name> <output_file.xml>"
    exit 1
fi

MODULE_NAME=$1
FILE=$2
if [ ! -f $FILE ]; then
    echo "No such file: $2"
    exit
fi
SCRIPT_DIR="$(cd "$(dirname $0)" && pwd)"
AUTO_NUM=$(cd $SCRIPT_DIR/..; scripts/retriever.sh -anum src/$MODULE_NAME)
if [ $? -ne 0 ]; then echo $AUTO_NUM; exit 1; fi
MAN_NUM=$(cd $SCRIPT_DIR/..; scripts/retriever.sh -mnum src/$MODULE_NAME)
if [ $? -ne 0 ]; then echo $MAN_NUM; exit 1; fi

TFILE="/tmp/tempfile.xml"
if [ -f $TFILE ]; then
    rm $TFILE
fi

function regen {
    awk -v RS='\r\n' -v ORS='\r\n' -v MODULE_NAME=$MODULE_NAME -v AUTO_NUM=$AUTO_NUM -v MAN_NUM=$MAN_NUM '    
    BEGIN {
        found = 0;
        replaced = 0;
    }
    $0 ~ "<suite name=\"tct-" MODULE_NAME "-core-tests\" category=\"Core APIs\">" {
        found = 1;
        next
    }
    /<\/suite>/ {
        if (found == 1) {
print "  <suite name=\"tct-" MODULE_NAME "-core-tests\" category=\"Core APIs\">";
print "    <auto_tcn>" AUTO_NUM "</auto_tcn>";
print "    <manual_tcn>" MAN_NUM "</manual_tcn>";
print "    <total_tcn>" AUTO_NUM+MAN_NUM "</total_tcn>";
print "    <pkg_name>tct-" MODULE_NAME "-core-tests-2.2.1-1.zip</pkg_name>";
print "  </suite>";
            found = 0;
            replaced = 1;
        } else {
            print $0;
        }
        next
    }
    /<\/ns3:testplan>/ {
        if (replaced == 0) {
print "  <suite name=\"tct-" MODULE_NAME "-core-tests\" category=\"Core APIs\">";
print "    <auto_tcn>" AUTO_NUM "</auto_tcn>";
print "    <manual_tcn>" MAN_NUM "</manual_tcn>";
print "    <total_tcn>" AUTO_NUM+MAN_NUM "</total_tcn>";
print "    <pkg_name>tct-" MODULE_NAME "-core-tests-2.2.1-1.zip</pkg_name>";
print "  </suite>";
print $0
        } else {
            print $0
        }
        next
    }
    {
        if (found == 0) {
            print $0;
        }
    }' $FILE > $TFILE
    cat $TFILE > $FILE
}

regen
