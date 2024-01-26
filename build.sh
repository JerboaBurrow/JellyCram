#!/bin/bash

WINDOWS=1
OSX=1
RELEASE=0
ANDROID_NDK=""
VK_SDK="$(pwd)/include/jGL/include/vendored/VulkanSDK"
CLEAN=1
PROFILE=0

while [[ $# -gt 0 ]]; do
  case $1 in
    -w|--windows)
      WINDOWS=0
      shift # past argument
      ;;
    -o|--osx)
      OSX=0
      shift
      ;;
    -r|--release)
      RELEASE=1
      shift
      ;;
    --android)
      ANDROID_NDK=$2
      shift
      shift
      ;;
    --vk)
      VK_SDK=$2
      shift
      shift
      ;;
    -c|--continue)
      CLEAN=0
      shift
      ;;
    -p|--profile)
      PROFILE=1
      shift
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

export VULKAN_SDK=$VK_SDK
export VULKAN_LIBRARY="$VK_SDK/Linux/Lib"
export VULKAN_INCLUDE_DIR="$VK_SDK/Include"

if [[ $CLEAN -eq 1 ]]; 
then
  if [ -d build ];
  then
    rm -rf build
  fi
  mkdir build

  STATUS=0

  if [[ $WINDOWS -eq 0 ]];
  then 
    export VULKAN_SDK=$VK_SDK/Windows
    export VULKAN_LIBRARY="$VK_SDK/Windows/Lib"
    export VULKAN_INCLUDE_DIR="$VK_SDK/Windows/Include" 
    ln -s "$VK_SDK/Windows/Lib" "$VK_SDK/Windows/lib"
    ln -s "$VK_SDK/Include" "$VK_SDK/Windows/Include"
    ln -s "$VK_SDK/Windows/Include" "$VK_SDK/Windows/include"
    cd build
    cmake .. -D WINDOWS=ON -D BUILD_DEMOS=$DEMO -D RELEASE=$RELEASE CMAKE_TOOLCHAIN_FILE=./windows.cmake && make -j 4
    STATUS=$?
    cd ..
    cp common/windows/*.dll build/
  elif [[ $OSX -eq 0 ]];
  then
    cd build
    cmake .. -D OSX=ON -D RELEASE=$RELEASE -D CMAKE_TOOLCHAIN_FILE=./osx.cmake && make -j 4
    STATUS=$?
    cd ..
  else
    cd build
    cmake -D PROFILE=$PROFILE -D RELEASE=$RELEASE .. && make -j 4
    STATUS=$?
    cd ..
  fi
else 
  cd build && make -j 4 
  STATUS=$?
  cd ..
fi

exit $STATUS