#!/bin/bash

if [[ -z $1 ]]; then
    echo "Usage note: tctestsgen.sh <module_name>"
    exit 1
fi

MODULE_NAME=$1
MODULE_NAME_C=$(echo $MODULE_NAME | sed -e 's/-\([a-z]\)/\U\1/' -e 's/^\([a-z]\)/\U\1/')
SCRIPT_DIR="$(cd "$(dirname $0)" && pwd)"
TC_DIR="/opt/usr/bin/tct-$1-core"
if [[ $3 == "desktop" ]] ; then
  TC_DIR="build/src/$1"
fi

FILE="$2/tests.xml"
if [ -a $FILE ]; then
    rm $FILE
fi
TFILE="/tmp/tcs.csv"
if [ -a $TFILE ]; then
    rm $TFILE
fi

function gen {
    awk -F',' -v MODULE_NAME=$MODULE_NAME -v MODULE_NAME_C=$MODULE_NAME_C -v TC_DIR=$TC_DIR '
    BEGIN {
        set = ""
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
print "    <?xml-stylesheet type=\"text/xsl\" href=\"./testcase.xsl\"?>";
print "<test_definition>";
print "  <suite name=\"tct-"MODULE_NAME"-core-tests\" category=\"Core APIs\">";
    }
    {
        if (set != "" && set != $2) {
print "    </set>"
        }

        if (set != $2) {
            set = $2;
print "    <set name=\"" set "\">";
        }

        tcname = $1;
        tcpurpose = $3

print "      <testcase component=\"CoreAPI/" MODULE_NAME_C "/" set "\" execution_type=\"auto\" id=\"" tcname "\" purpose=\"" tcpurpose "\">";
print "        <description>";

print "          <test_script_entry test_script_expected_result=\"0\">" TC_DIR "/tct-" MODULE_NAME "-core " tcname "</test_script_entry>";
print "        </description>";
print "      </testcase>";
    }
    END {
        if (set != "") {
print "    </set>"
        }
print "  </suite>"
print "</test_definition>"
    }' $TFILE > $FILE
}

(cd $SCRIPT_DIR/..; scripts/retriever.sh -f src/$MODULE_NAME $4 > ${TFILE}_pre)
if [ $? -ne 0 ]; then cat ${TFILE}_pre; exit 1; fi
cat ${TFILE}_pre | sort -t',' -k2,2 -s > $TFILE
gen
