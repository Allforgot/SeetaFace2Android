cmake_minimum_required(VERSION 3.4.1)

# seetaface
set(SeetaFace_DIR ${CMAKE_SOURCE_DIR}/src/main/seetafacecpp)
find_package(SeetaFace REQUIRED)
include_directories(${SeetaFace_INCLUDE_DIRS})

# OpenCV
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/src/main/seetafacecpp/thirdparty/OpenCV-android-sdk/sdk/native/jni)
find_package(OpenCV REQUIRED)
include_directories(OpenCV_INCLUDE_DIRS)

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/main/seetafacecpp/src/seeta SEETA_SRC)

set(JNI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main/seetafacecpp/src/seetaface_jni.cpp)

add_library(seetaface SHARED ${SEETA_SRC} ${JNI_SRC})

target_link_libraries(seetaface ${SeetaFace_LIBS} ${OpenCV_LIBS})
