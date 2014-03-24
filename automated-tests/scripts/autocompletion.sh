#!/bin/bash
if [ -z "$TC_PROJECT_DIR" ]; then
    echo "CoreAPI project directory can't be found. `basename $0` exitting..."
    exit 1
fi

_tcbuild () {
    local cur
    cur=${COMP_WORDS[$COMP_CWORD]}
    if [ $COMP_CWORD -eq 1 ]; then
        COMPREPLY=( $( compgen -W "addmod build install rmmod" -- $cur ) )
    else #if [ $COMP_CWORD -eq 2 ]; then
        COMPREPLY=( $( cd $TC_PROJECT_DIR/src; compgen -d -X "common" -- $cur ) )
    fi
    return 0
}

complete -F _tcbuild tcbuild
