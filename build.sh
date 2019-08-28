mkdir -p build
pushd build

cmake -DCMAKE_SYSTEM_NAME=Android \
  	-DCMAKE_SYSTEM_VERSION=21 \
  	-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
  	-DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
  	-DANDROID_ABI="arm64-v8a" \
  	-DCMAKE_ANDROID_NDK=${ANDROID_NDK_HOME} \
  	-DCMAKE_ANDROID_STL_TYPE=gnustl_static \
  	..

make
popd