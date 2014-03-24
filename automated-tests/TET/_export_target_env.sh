export ARCH=target

export TET_INSTALL_PATH=/mnt/nfs/git/TETware  # path to mount
export TET_TARGET_PATH=$TET_INSTALL_PATH
export PATH=$TET_TARGET_PATH/bin:$PATH
export LD_LIBRARY_PATH=$TET_TARGET_PATH/lib/tet3:$LD_LIBRARY_PATH
export DALI_IMAGE_DIR=/opt/share/app.dalimenu/images/
export DALI_MODEL_DIR=/opt/share/app.dalimenu/models/
export DALI_STYLE_DIR=/opt/share/themes/dali/

export TET_ROOT=$TET_TARGET_PATH

set $(pwd)
export TET_SUITE_ROOT=$1

set $(date +%s)
FILE_NAME_EXTENSION=$1
