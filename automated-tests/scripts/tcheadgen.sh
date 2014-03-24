#!/bin/bash

if [[ -z $1 ]]; then
    echo "Usage note: tcheadgen.sh <header_filename.h>"
    exit 1
fi

FILE="$PWD/$1"
TFILE="/tmp/retr.csv"
HEADER_NAME=$(echo $1 | tr '[:lower:]' '[:upper:]' | sed -e 's/-/_/g' -e 's/\./_/')
SCRIPT_DIR="$(cd "$(dirname $0)" && pwd)"

$SCRIPT_DIR/retriever.sh > $TFILE
if [ $? -ne 0 ]; then cat $TFILE; exit 1; fi
awk -F',' -v HEADER_NAME="$HEADER_NAME" '
    BEGIN {
        OFS = ", ";

        startup_counter = 0;
        startup_list[0] = "";

        cleanup_counter = 0;
        cleanup_list[0] = "";

        testcase_counter = 0;
        testcase_list[0] = "";

        tc_array_counter = 0;
        tc_array_list[0] = "";

print "#ifndef __" HEADER_NAME "__"
print "#define __" HEADER_NAME "__"
print ""
print "#include \"testcase.h\""
print ""
    }
    {
        testcase_list[testcase_counter++] = $1;

        if (startup_counter == 0 || startup_list[startup_counter-1] != $2)
            startup_list[startup_counter++] = $2;

        if (startup_counter == 0 || cleanup_list[cleanup_counter-1] != $3)
            cleanup_list[cleanup_counter++] = $3;

        tc_array_list[tc_array_counter++] = "\"" $1 "\", " $1 ", " $2 ", " $3;
    }
    END {
        sc_count = (startup_counter > cleanup_counter) ? startup_counter : cleanup_counter;
        for (i = 0; i < sc_count; i++) {
            if (i < startup_counter && startup_list[i] != "NULL")
print "extern void " startup_list[i] "(void);"
            if (i < cleanup_counter && cleanup_list[i] != "NULL")
print "extern void " cleanup_list[i] "(void);"
        }

print ""
        for (i = 0; i < testcase_counter; i++)
print "extern int " testcase_list[i] "(void);"

print ""
print "testcase tc_array[] = {"

        for (i = 0; i < tc_array_counter; i++)
print "    {" tc_array_list[i] "},"

print "    {NULL, NULL}"
print "};"
print ""
print "#endif // __" HEADER_NAME "__"
}' $TFILE > $FILE
