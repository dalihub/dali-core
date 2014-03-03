export ARCH=desktop


if [ -z "$DESKTOP_PREFIX" ] ; then
    if [ -z "$PREFIX" ] ; then
        echo "####################################################"
        echo "# DESKTOP_PREFIX is not set. Recommend running     #"
        echo "# dali_env -s to create setenv script              #"
        echo "####################################################"
    else
        echo "####################################################"
        echo "# DESKTOP_PREFIX is not set. Using PREFIX instead. #"
        echo "#                                                  #"
        echo "# Warning, PREFIX is deprecated, please use        #"
        echo "# dali_env to set up your environment.             #"
        echo "####################################################"
        export DESKTOP_PREFIX=$PREFIX
    fi
fi

export TET_INSTALL_PATH=$HOME/Packages/tetware-desktop  # Your tetware root path
export PATH=$TET_INSTALL_PATH/bin:$PATH
export LD_LIBRARY_PATH=$TET_INSTALL_PATH/lib/tet3:$LD_LIBRARY_PATH
export TET_ROOT=$TET_INSTALL_PATH
export DALI_IMAGE_DIR=$DESKTOP_PREFIX/share/app.dalimenu/images/
export DALI_MODEL_DIR=$DESKTOP_PREFIX/share/app.dalimenu/models/
export DALI_STYLE_DIR=$DESKTOP_PREFIX/share/themes/dali/

set $(pwd)
export TET_SUITE_ROOT=$1

set $(date +%s)
FILE_NAME_EXTENSION=$1
