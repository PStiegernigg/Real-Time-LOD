#!/bin/sh

SCRIPT_DIR="$( cd "$( dirname "$0" )" && pwd )"
cd "$SCRIPT_DIR"

echo Building GLEW...
cd $SCRIPT_DIR/vendor/glew-2.1.0
make

echo Building AntTweakBar...
cd $SCRIPT_DIR/vendor/AntTweakBar/src
make

echo Building GLFW...
cd $SCRIPT_DIR/vendor/glfw-3.2.1
cmake -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF
make

echo Building AssImp...
cd $SCRIPT_DIR/vendor/assimp-3.3.1
cmake -DZLIB_LIBRARIES=zlibstatic
make



