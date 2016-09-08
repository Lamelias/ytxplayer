//
// Created by Administrator on 2016/9/7.
//
#include <android/log.h>
#include "decoder_video.h"
#include "YtxMediaPlayer.h"

#define TAG "FFMpegVideoDecoder"

static uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

DecoderVideo::DecoderVideo(InputStream* stream) : IDecoder(stream)
{
    stream->dec_ctx->get_buffer2 = getBuffer;
  //  mStream->codec->
  //  mStream->codec->release_buffer = releaseBuffer;
}

DecoderVideo::~DecoderVideo()
{
}

bool DecoderVideo::prepare()
{
    ALOGI("ytxhao DecoderVideo::prepare");
    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return false;
    }
    return true;
}

double DecoderVideo::synchronize(AVFrame *src_frame, double pts) {

    double frame_delay;

    if (pts != 0) {
        /* if we have pts, set video clock to it */
        mVideoClock = pts;
    } else {
        /* if we aren't given a pts, set it to the clock */
        pts = mVideoClock;
    }
    /* update the video clock */
    frame_delay = av_q2d(mStream->dec_ctx->time_base);
    /* if we are repeating a frame, adjust clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    mVideoClock += frame_delay;
    return pts;
}

bool DecoderVideo::process(AVPacket *packet)
{
    int	completed;
    int pts = 0;

    // Decode video frame
    __android_log_print(ANDROID_LOG_INFO, TAG, "process mStream->dec_ctx=%d,mStream=%d",mStream->dec_ctx,mStream);
    avcodec_decode_video2(mStream->dec_ctx,
                         mFrame,
                         &completed,
                         packet);

    /*
    if (packet->dts == AV_NOPTS_VALUE && mFrame->opaque
        && *(uint64_t*) mFrame->opaque != AV_NOPTS_VALUE) {
        pts = *(uint64_t *) mFrame->opaque;
    } else if (packet->dts != AV_NOPTS_VALUE) {
        pts = packet->dts;
    } else {
        pts = 0;
    }
    pts *= av_q2d(mStream->time_base);
*/



    if (completed) {
        __android_log_print(ANDROID_LOG_INFO, TAG, "process mFrame=%d,mFrame->data=%d,mFrame->data[0]=%d",mFrame,mFrame->data,mFrame->data[0]);
        pts = synchronize(mFrame, pts);

        onDecode(mFrame, pts);

        return true;
    }
    return false;
}

bool DecoderVideo::decode(void* ptr)
{
    AVPacket        pPacket;

    __android_log_print(ANDROID_LOG_INFO, TAG, "decoding video");

    while(mRunning)
    {

        __android_log_print(ANDROID_LOG_INFO, TAG, "1 decoding video pPacket.buf=%d,pPacket.data=%d,pPacket.size=%d",pPacket.buf,pPacket.data,pPacket.size);
        if(mQueue->get(&pPacket, true) < 0)
        {
            mRunning = false;
            return false;
        }
        __android_log_print(ANDROID_LOG_INFO, TAG, "2 decoding video pPacket.buf=%d,pPacket.data=%d,pPacket.size=%d",pPacket.buf,pPacket.data,pPacket.size);
        if(!process(&pPacket))
        {
            mRunning = false;
            return false;
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&pPacket);
    }

    __android_log_print(ANDROID_LOG_INFO, TAG, "decoding video ended");

    // Free the RGB image
    av_free(mFrame);

    return true;
}

/* These are called whenever we allocate a frame
 * buffer. We use this to store the global_pts in
 * a frame at the time it is allocated.
 */
int DecoderVideo::getBuffer(struct AVCodecContext *c, AVFrame *pic,int flags) {
    int ret = avcodec_default_get_buffer2(c, pic,flags);
    uint64_t *pts = (uint64_t *)av_malloc(sizeof(uint64_t));
    *pts = global_video_pkt_pts;
    pic->opaque = pts;
    return ret;
}
void DecoderVideo::releaseBuffer(struct AVCodecContext *c, AVFrame *pic) {
    if (pic)
        av_freep(&pic->opaque);
   // avcodec_default_release_buffer(c, pic);
}

