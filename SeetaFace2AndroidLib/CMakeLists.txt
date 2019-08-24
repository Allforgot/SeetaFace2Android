cmake_minimum_required(VERSION 3.4.1)

project(seetaface)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

# Android 5.0 以上需要在此处设置 PIE
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")

#设置编译选项
add_definitions(-std=c++11)
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -fopenmp -O2 -fvisibility=hidden -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math ")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -fopenmp -std=c++11 -frtti -fexceptions -O2 -fvisibility=hidden -fvisibility-inlines-hidden -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math ")

# seetaface
file(GLOB SeetaFace_LIBS_FILE ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/*.so)
set(SeetaFace_LIBS ${SeetaFace_LIBS_FILE})
include_directories(${CMAKE_SOURCE_DIR}/include)

# OpenCV
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/thirdparty/OpenCV-android-sdk/sdk/native/jni)
find_package(OpenCV REQUIRED)
include_directories(OpenCV_INCLUDE_DIRS)

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/main/seetafacecpp/src/seeta SEETA_SRC)

set(JNI_SRC ${CMAKE_SOURCE_DIR}/src/seetaface_jni.cpp)

add_library(${PROJECT_NAME} SHARED ${SEETA_SRC} ${JNI_SRC})

target_link_libraries(${PROJECT_NAME} ${SeetaFace_LIBS} ${OpenCV_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/jniLibs/${ANDROID_ABI})
