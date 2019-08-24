package com.seetatech.seetaface;

public class SeetaImageData {
    public byte[] data;
    public int width;
    public int height;
    public int channels;

    public SeetaImageData() {}

    public SeetaImageData(byte[] data, int width, int height, int channels) {
        this.data = data;
        this.width = width;
        this.height = height;
        this.channels = channels;
    }
}

