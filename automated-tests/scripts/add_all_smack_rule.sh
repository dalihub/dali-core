#!/bin/sh

SCRIPTDIR=`dirname $(readlink -f $0)`
SMACK_FILE=$SCRIPTDIR/all_smack.rule
	
echo ""
echo "Add smack rule in $SMACK_FILE"
echo "==================================================="
echo ""

while read rule; do
	NO_BLANK=$(echo $rule | sed 's/ //g' | sed 's/  //g')
	if [ ${#NO_BLANK} -lt 1 ]; then
		echo "Blank"
		continue
	elif [ `echo $NO_BLANK|cut -c1-1` = '#' ]; then
		echo "Comment"
		continue
	fi

	echo "echo \"$rule\" > /smack/load2"
	echo "$rule" > /smack/load2
done < $SMACK_FILE

echo "==================================================="
echo ""
