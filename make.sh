#~/bin/bash

#
# third party / dependencies
#

#export CXX="clang-13"
#export CC="clang-13"

ROOT_DIR=`pwd`
BUILD_DIR=${ROOT_DIR}/build
THIRD_PARTY=${ROOT_DIR}/third_party


CMAKE_PREFIX_PATH=${ROOT_DIR}/install
CMAKE_INSTALL_PATH=${ROOT_DIR}/install
CMAKE_ARGS="-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PATH}"

build_third_party()
{
TARGET_DIR=$1



# target build
mkdir -p $TARGET_DIR/build
cd $TARGET_DIR/build
echo "cmake command: "
echo "cmake ${CMAKE_ARGS} ${EIGEN_DIR}"
cmake ${CMAKE_ARGS} ${TARGET_DIR}
make install
}


build_third_party_tiems()
{

GTEST_DIR=${THIRD_PARTY}/googletest

build_third_party $GTEST_DIR
}

build_third_party_items


#
# project
#

mkdir -p $BUILD_DIR
cd $BUILD_DIR
echo "running command  cmake ${CMAKE_ARGS} .."
cmake ${CMAKE_ARGS} ..
make install
