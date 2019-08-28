#pragma warning(disable: 4819)

#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>
#include <seeta/FaceDatabase.h>
#include <seeta/Struct_cv.h>
#include <seeta/Struct.h>

#include <array>
#include <map>
#include <iostream>

#include "jni.h"

#define TAG "seetaface"

using namespace seeta;

static FaceDetector *mFaceDetector;
static FaceLandmarker *mFaceLandmarker;
static FaceRecognizer *mFaceRecognizer;

//static std::string modelDirString;
//std::string faceDetectModelPath;
//std::string faceLandmarkModelPath;
//std::string faceRecognitionModelPath;
//
//static ModelSetting::Device device = ModelSetting::CPU;
//static int id = 0;


extern "C" {
JNIEXPORT jboolean JNICALL
Java_com_seetatech_seetaface_SeetaFace_native_1FaceModelInit(JNIEnv *env, jobject instance, jstring jmodelDir, jint minFaceSize) {
    const char *modelDir = env->GetStringUTFChars(jmodelDir, JNI_FALSE);
    std::string modelDirString = std::string(modelDir);
    std::string faceDetectModelPath = modelDirString + "/fd_2_00.dat";
    std::string faceLandmarkModelPath = modelDirString + "/pd_2_00_pts5.dat";
    std::string faceRecognitionModelPath = modelDirString + "/fr_2_10.dat";

    ModelSetting::Device device = ModelSetting::CPU;
    int id = 0;
    const ModelSetting FD_model(faceDetectModelPath, device, id );
    const ModelSetting PD_model(faceLandmarkModelPath, device, id );
    const ModelSetting FR_model(faceRecognitionModelPath, device, id );
    mFaceDetector = new FaceDetector(FD_model);
    mFaceLandmarker = new FaceLandmarker(PD_model);
    mFaceRecognizer = new FaceRecognizer(FR_model);

    mFaceDetector->set(mFaceDetector->PROPERTY_MIN_FACE_SIZE, minFaceSize);

    return JNI_TRUE;
}

JNIEXPORT jintArray JNICALL
Java_com_seetatech_seetaface_SeetaFace_native_1FaceDetect(JNIEnv *env, jobject instance, jbyteArray imageData, jint width,
                                        jint height, jint channels) {

    jboolean inputCopy = JNI_FALSE;
    jbyte *in_imageData = env->GetByteArrayElements(imageData, &inputCopy);
    ImageData image;
    image.data = (unsigned char *)in_imageData;
    image.width = width;
    image.height = height;
    image.channels = channels;

    SeetaFaceInfoArray faces = mFaceDetector->detect(image);
    int faceNnm = faces.size;
    int outSize = faceNnm * 4 + 1;
    int *faceInfoArray = new int[outSize];
    faceInfoArray[0] = faceNnm;
    for (int i = 0; i < faceNnm; i ++) {
        faceInfoArray[4 * i + 1] = faces.data[i].pos.x;
        faceInfoArray[4 * i + 2] = faces.data[i].pos.y;
        faceInfoArray[4 * i + 3] = faces.data[i].pos.width;
        faceInfoArray[4 * i + 4] = faces.data[i].pos.height;
    }
    jintArray faceInfoArrayRet = env->NewIntArray(outSize);
    env->SetIntArrayRegion(faceInfoArrayRet, 0, outSize, faceInfoArray);

    delete[] faceInfoArray;
    env->ReleaseByteArrayElements(imageData, in_imageData, 0);

    return faceInfoArrayRet;
}

JNIEXPORT jdoubleArray JNICALL
Java_com_seetatech_seetaface_SeetaFace_native_1LandmarkDetect(JNIEnv *env, jobject instance,
                                                              jbyteArray imageData, jint width,
                                                              jint height, jint channels , jintArray faceRect) {

    jboolean inputCopy = JNI_FALSE;
    jbyte *in_imageData = env->GetByteArrayElements(imageData, &inputCopy);
    jint * in_faceRect = env->GetIntArrayElements(faceRect, &inputCopy);

    SeetaImageData seetaImageData;
    seetaImageData.data = (unsigned char *)in_imageData;
    seetaImageData.width = width;
    seetaImageData.height = height;
    seetaImageData.channels = channels;

    SeetaRect seetaRect;
    seetaRect.x = in_faceRect[0];
    seetaRect.y = in_faceRect[1];
    seetaRect.width = in_faceRect[2];
    seetaRect.height = in_faceRect[3];

    int outSize = mFaceLandmarker->number() * 2 + 1;
    double *landmarkDoubleArray = new double[outSize];
    landmarkDoubleArray[0] = (double)mFaceLandmarker->number();

    std::vector<SeetaPointF> landmarkPointVector = mFaceLandmarker->mark(seetaImageData, seetaRect);
    for (int i = 0; i < mFaceLandmarker->number(); i ++) {
        landmarkDoubleArray[i * 2 + 1] = landmarkPointVector[i].x;
        landmarkDoubleArray[i * 2 + 2] = landmarkPointVector[i].y;
    }

    jdoubleArray seetaPointFRet = env->NewDoubleArray(outSize);
    env->SetDoubleArrayRegion(seetaPointFRet, 0, outSize, landmarkDoubleArray);

    delete[] landmarkDoubleArray;
    env->ReleaseByteArrayElements(imageData, in_imageData, 0);
    env->ReleaseIntArrayElements(faceRect, in_faceRect, 0);

    return seetaPointFRet;
}

JNIEXPORT jfloatArray JNICALL
Java_com_seetatech_seetaface_SeetaFace_native_1GetFaceFeature(JNIEnv *env, jobject instance,
                                                              jbyteArray imageData,
                                                              jint width, jint height,
                                                              jint channels, jdoubleArray seetapointf) {

    jboolean inputCopy = JNI_FALSE;
    jbyte *in_imageData = env->GetByteArrayElements(imageData, &inputCopy);
    jdouble *in_seetapointf = env->GetDoubleArrayElements(seetapointf, &inputCopy);

    SeetaImageData seetaImageData;
    seetaImageData.data = (unsigned char *)in_imageData;
    seetaImageData.width = width;
    seetaImageData.height = height;
    seetaImageData.channels = channels;

    SeetaPointF *seetaPointF = (SeetaPointF *)in_seetapointf;

    int featureSize = mFaceRecognizer->GetExtractFeatureSize();
    int outSize = featureSize + 1;
    float *featureOut = new float[outSize];
    float *features = new float[featureSize];
    bool ret = mFaceRecognizer->Extract(seetaImageData, seetaPointF, features);
    if (!ret) {
        return nullptr;
    }
    featureOut[0] = (float)featureSize;
    memcpy(featureOut+1, features, sizeof(features)*featureSize);
//    std::copy(std::begin(features), std::end(features), )
    jfloatArray featureOutFloatArray = env->NewFloatArray(outSize);
    env->SetFloatArrayRegion(featureOutFloatArray, 0, outSize, featureOut);

    delete[] featureOut;
    delete[] features;
    env->ReleaseByteArrayElements(imageData, in_imageData, 0);
    env->ReleaseDoubleArrayElements(seetapointf, in_seetapointf, 0);

    return featureOutFloatArray;
}

JNIEXPORT jfloat JNICALL
Java_com_seetatech_seetaface_SeetaFace_native_1CalculateSimilarity(JNIEnv *env, jobject instance,
                                        jfloatArray feature1, jfloatArray feature2) {

    jboolean inputCopy = JNI_FALSE;
    jfloat *in_feature1 = env->GetFloatArrayElements(feature1, &inputCopy);
    jfloat *in_feature2 = env->GetFloatArrayElements(feature2, &inputCopy);

    const float *feature1_ = in_feature1;
    const float *feature2_ = in_feature2;

    float similarity = mFaceRecognizer->CalculateSimilarity(feature1_, feature2_);

    env->ReleaseFloatArrayElements(feature1, in_feature1, 0);
    env->ReleaseFloatArrayElements(feature2, in_feature2, 0);

    return similarity;
}

}