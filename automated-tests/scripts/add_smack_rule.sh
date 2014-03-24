#!/bin/sh

SCRIPTDIR=`dirname $(readlink -f $0)`
SMACK_FILE=$SCRIPTDIR/all_smack.rule
	
echo ""
echo "Add smack rule in $SMACK_FILE"
echo "==================================================="

echo "sdb root on"
sdb root on

while read rule; do
	NO_BLANK=$(echo $rule | sed 's/ //g' | sed 's/  //g')
	if [ ${#NO_BLANK} -lt 1 ]; then
		#echo "Blank"
		continue
	elif [ `echo $NO_BLANK|cut -c1-1` = '#' ]; then
		#echo "Comment"
		continue
	fi

	echo "sdb shell \"echo $rule > /smack/load2\""
	sdb shell "echo $rule > /smack/load2"
done < $SMACK_FILE

echo "==================================================="
echo ""
