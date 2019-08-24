package com.seetatech.seetaface;

import android.graphics.Bitmap;
import android.util.Log;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.nio.ByteBuffer;
import java.util.ArrayList;

public class SeetaFace {
    static {
        System.loadLibrary("seetaface");
        System.loadLibrary("opencv_java3");
    }

    String MODEL_DIR = "/sdcard/seetaface";

    public boolean FaceModelInit(String modelDir, int minFazeSize) {
        if (modelDir.substring(modelDir.length()-1).equals("/")) {
            modelDir = modelDir.substring(0, modelDir.length()-1);
        }
        return native_FaceModelInit(modelDir, minFazeSize);
    }

    public ArrayList<SeetaRect> FaceDetect(SeetaImageData image) {
        ArrayList<SeetaRect> seetaRectArrayList = new ArrayList<>();
        int[] retFaceDetect =  native_FaceDetect(image.data, image.width, image.height, image.channels);
        int faceNum = retFaceDetect[0];
        for (int i = 0; i < faceNum; i ++) {
            int x = retFaceDetect[i * 4 + 1];
            int y = retFaceDetect[i * 4 + 2];
            int width = retFaceDetect[i * 4 + 3];
            int height = retFaceDetect[i * 4 + 4];
            seetaRectArrayList.add(new SeetaRect(x, y, width, height));
        }
        return seetaRectArrayList;
    }

    public double[] LandmarkDetect(SeetaImageData image, SeetaRect seetaRect) {
        int[] rect = new int[]{seetaRect.x, seetaRect.y, seetaRect.width, seetaRect.height};
        double[] result =  native_LandmarkDetect(image.data, image.width, image.height, image.channels, rect);
        double[] landmarks = new double[result.length - 1];
        System.arraycopy(result, 1, landmarks, 0, result.length - 1);
        return landmarks;
    }

    public float[] GetFaceFeature(SeetaImageData image, double[] landmarks) {
        float[] result =  native_GetFaceFeature(image.data, image.width, image.height, image.channels, landmarks);
        float[] feature = new float[result.length - 1];
        System.arraycopy(result, 1, feature, 0, result.length - 1);
        return feature;
    }

    public float CalculateSimilarity(float[] feature1, float[] feature2) {
        return native_CalculateSimilarity(feature1, feature2);
    }

    public SeetaImageData convertBitmap2SeetaImageData(Bitmap bitmap) {
        if (bitmap == null) {
            return null;
        }
        Mat imageMat = new Mat(bitmap.getHeight(), bitmap.getWidth(), CvType.CV_8UC4);
        Utils.bitmapToMat(bitmap, imageMat, true);
        Mat newImagtMat = new Mat();
        Imgproc.cvtColor(imageMat, newImagtMat, Imgproc.COLOR_BGRA2BGR);
        byte[] imageByte = new byte[(int)newImagtMat.total() * newImagtMat.channels()];
        newImagtMat.get(0, 0, imageByte);
        SeetaImageData image = new SeetaImageData();
        image.data = imageByte;
        image.width = newImagtMat.width();
        image.height = newImagtMat.height();
        image.channels = newImagtMat.channels();
        return image;
    }

    private byte[] convertBitmapToByteArray(Bitmap image) {
        int bytes = image.getByteCount();
        ByteBuffer buffer = ByteBuffer.allocate(bytes);
        image.copyPixelsToBuffer(buffer);
        byte[] temp = buffer.array();

        return temp;
    }

    //================================ jni ================================

    public native boolean native_FaceModelInit(String modelDir, int minFazeSize);

    public native int[] native_FaceDetect(byte[] imageData, int width, int height, int channels);

    public native double[] native_LandmarkDetect(byte[] imageData, int width, int height, int channels, int[] faceRect);

    public native float[] native_GetFaceFeature(byte[] imageData, int width, int height, int channels, double[] landmarks);

    public native float native_CalculateSimilarity(float[] feature1, float[] feature2);
}
