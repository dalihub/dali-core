#!/bin/bash

PROJECT_DIR="$(cd "$(dirname $0)" && cd .. && pwd)"

ln -s $PROJECT_DIR/scripts/tcbuild.sh $PROJECT_DIR/tcbuild

echo "" >> $HOME/.bashrc
echo "# CoreAPI-tests convenience helpers" >> $HOME/.bashrc
echo "export TC_PROJECT_DIR=\"$PROJECT_DIR\"" >> $HOME/.bashrc
echo "source $PROJECT_DIR/scripts/autocompletion.sh" >> $HOME/.bashrc
