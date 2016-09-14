
package com.ytx.ican.media.player.render;

import android.content.Context;
import android.opengl.GLES20;
import android.util.AttributeSet;

import com.ytx.ican.media.player.YtxLog;


public class VideoGlSurfaceViewFFMPEG extends VideoGlSurfaceView {

    private final static String TAG = "VideoDecoderFFMPEG";

    YUVFilter mYUVFilter;
    Picture mPhoto;
    int mWidth=2;
    int mHeight;
    int mYUVTextures[] = new int[3];;
    volatile boolean mInitialed = false;
  //  H264Decoder mH264Decoder;

    public VideoGlSurfaceViewFFMPEG(Context context){
        this(context,null,null);
    }

    public VideoGlSurfaceViewFFMPEG(Context context, AttributeSet attrs, HardDecodeExceptionCallback callback) {
        super(context, attrs, callback);
    }

    @Override
    protected void initial() {
        super.initial();
      //  mH264Decoder = new H264Decoder();
        mYUVFilter = new YUVFilter(getContext());
        mYUVFilter.initial();
      //  mYUVTextures = new int[3];
        GLES20.glGenTextures(mYUVTextures.length, mYUVTextures, 0);
        mYUVFilter.setYuvTextures(mYUVTextures);
        mInitialed = true;
    }

    @Override
    protected void release() {
        super.release();
        mInitialed = false;
//        if(mH264Decoder!=null) {
//            mH264Decoder.release();
//            mH264Decoder = null;
//        }
        if (mYUVFilter != null) {
            mYUVFilter.release();
            mYUVFilter = null;
            GLES20.glDeleteTextures(mYUVTextures.length, mYUVTextures, 0);
        }
        if (mPhoto != null) {
            mPhoto.clear();
            mPhoto = null;
        }
    }

    @Override
    public void drawFrame() {
        super.drawFrame();
        if (!mInitialed) {
            return;
        }
        VideoFrame frame = mAVFrameQueue.poll();
        if (frame == null || frame.data == null) {
            return;
        }
        long lastTime = System.currentTimeMillis();
        if (frame.width != mWidth || frame.height != mHeight) {
            mWidth = frame.width;
            mHeight = frame.height;
//            if(mH264Decoder!=null) {
//                mH264Decoder.release();
//            }
//            mH264Decoder = new H264Decoder();
        }

        // ByteBuffer buffer = ByteBuffer.wrap(frame.data);
        // buffer.flip();
        // if (H264Decoder.decodeBuffer(buffer, buffer.limit(),
        // frame.timeStamp))// (frame.data,frame.data.length,frame.timeStamp))
//        if (mH264Decoder.decode(frame.data, frame.data.length, frame.timeStamp))
//        {
//            int ret = mH264Decoder.toTexture(mYUVTextures[0], mYUVTextures[1], mYUVTextures[2]);
//            if (ret < 0)
//                return;
//
//            if (mPhoto == null) {
//                mPhoto = Picture.create(mH264Decoder.getWidth(), mH264Decoder.getHeight());
//            } else {
//                mPhoto.updateSize(mH264Decoder.getWidth(), mH264Decoder.getHeight());
//            }
//
//            mYUVFilter.process(null, mPhoto);
//            Picture dst = appFilter(mPhoto);
//            RendererUtils.checkGlError("process");
//            setPicture(dst);
//            RendererUtils.checkGlError("setPhoto");
//        }

        if (mAVFrameQueue.size() > 0) {
            requestRender();
        }

        long decodeOneFrameMilliseconds = System.currentTimeMillis() - lastTime;
        onDecodeTime(decodeOneFrameMilliseconds);

        YtxLog.d(TAG, "decode " + frame.toString() + ", decodeTime:" + decodeOneFrameMilliseconds);

    }

    public void getTextureData(){

    }
}
