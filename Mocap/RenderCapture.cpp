#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#include "RenderCapture.h"
#include <vector>
#include <time.h>
#include <glad\glad.h>

extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

struct capImpl
{
	std::vector<unsigned int> pbos;
	size_t reservedFrames, freeFrame;
	clock_t lastCap, capDelay;
	unsigned int width, height, fps;
	unsigned int fWidth, fHeight;
};

RenderCapture::RenderCapture(unsigned int width, unsigned int height, unsigned int outWidth, unsigned int outHeight, unsigned int fps)
{
	pimpl = std::make_unique<capImpl>();
	pimpl->capDelay = round((double)CLOCKS_PER_SEC / fps);
	pimpl->lastCap = 0;
	pimpl->reservedFrames = fps * 10;
	pimpl->pbos.resize(pimpl->reservedFrames);
	pimpl->freeFrame = 0;
	pimpl->width = width;
	pimpl->height = height;
	pimpl->fps = fps;
	pimpl->fWidth = outWidth == ~0 ? width : outWidth;
	pimpl->fHeight = outHeight == ~0 ? height : outHeight;
	glGenBuffers(pimpl->reservedFrames, &pimpl->pbos[0]);
	for (unsigned int pbo : pimpl->pbos) {
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
		glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, NULL, GL_STREAM_READ);
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}


RenderCapture::~RenderCapture()
{
	glDeleteBuffers(pimpl->reservedFrames, &pimpl->pbos[0]);
}

void RenderCapture::capture()
{
	if (clock() - pimpl->lastCap >= pimpl->capDelay) {
		if (pimpl->freeFrame == pimpl->reservedFrames) {
			unsigned int inc = pimpl->reservedFrames;
			pimpl->reservedFrames *= 2;
			pimpl->pbos.resize(pimpl->reservedFrames);
			glGenBuffers(inc, &pimpl->pbos[pimpl->freeFrame]);
			for (size_t i = pimpl->freeFrame; i < pimpl->reservedFrames; ++i) {
				glBindBuffer(GL_PIXEL_PACK_BUFFER, pimpl->pbos[i]);
				glBufferData(GL_PIXEL_PACK_BUFFER, pimpl->width * pimpl->height * 4, NULL, GL_STREAM_READ);
			}
		}
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pimpl->pbos[pimpl->freeFrame++]);
		glReadPixels(0, 0, pimpl->width, pimpl->height, GL_BGRA, GL_UNSIGNED_BYTE, 0);
		int err = glGetError();
		if (err != 0) printf("Gl error: %d\n");
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		pimpl->lastCap = clock();
	}
}

void RenderCapture::setFrameRate(unsigned int fps)
{
	pimpl->capDelay = round((double)CLOCKS_PER_SEC / fps);
}

void RenderCapture::saveCapture(const char * filename)
{
	int success;
	av_register_all();
	SwsContext * convert = sws_getContext(pimpl->width, pimpl->height, AV_PIX_FMT_BGRA, pimpl->fWidth, pimpl->fHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	AVOutputFormat * fmt = av_guess_format(NULL, filename, NULL);
	AVCodecContext * ctx;
	AVCodec * codec = avcodec_find_encoder(fmt->video_codec);
	if (!codec) printf("Codec not found!\n");
//	avformat_alloc_output_context2(&ctx, NULL, NULL, filename);
	ctx = avcodec_alloc_context3(codec);
	ctx->bit_rate = 400000;
	ctx->width = pimpl->fWidth;
	ctx->height = pimpl->fHeight;
	ctx->time_base = { 1, (int)pimpl->fps };
	ctx->framerate = { (int)pimpl->fps, 1 };
	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	if (avcodec_open2(ctx, codec, NULL) < 0) printf("Could not open codec\n");
	AVFrame * frame = av_frame_alloc();
	frame->width = pimpl->fWidth;
	frame->height = pimpl->fHeight;
	frame->format = AV_PIX_FMT_YUV420P;
//	if(av_image_alloc(frame->data, frame->linesize, ctx->width, ctx->height, ctx->pix_fmt, 32) < 0) printf("Could not alloc frame!\n");
	if (av_frame_get_buffer(frame, 32) < 0) printf("Could not alloc frame!\n");
	FILE * f = fopen(filename, "wb");
	AVPacket packet;
	for (unsigned int i = 0; i < pimpl->freeFrame; ++i) {
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pimpl->pbos[i]);
		GLubyte * data = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		auto flippedData = std::make_unique<uint8_t[]>(pimpl->width * pimpl->height * 4);
		for (unsigned int i = 0; i < pimpl->height; ++i) {
			memcpy(&flippedData[i * 4 * pimpl->width], &data[(pimpl->height - i - 1) * 4 * pimpl->width], pimpl->width * 4);
		}
		int rgbLinesize = pimpl->width * 4;
		uint8_t * flippedDataPtr = flippedData.get();
		sws_scale(convert, &flippedDataPtr, &rgbLinesize, 0, pimpl->width, frame->data, frame->linesize);
		frame->pts = i;
		av_init_packet(&packet);
		if(av_frame_make_writable(frame) < 0) printf("Frame not writeable!\n");
		avcodec_encode_video2(ctx, &packet, frame, &success);
		if (success < 0) printf("Error encoding frame!\n");
		else {
			fwrite(packet.data, 1, packet.size, f);
			av_free_packet(&packet);
		}
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}
	success = 1;
	while (success) {
		int r = avcodec_encode_video2(ctx, &packet, NULL, &success);
		if (r < 0) printf("error encoded delayed frames!\n");
		else if (success) {
			fwrite(packet.data, 1, packet.size, f);
			av_free_packet(&packet);
		}
	}
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	fwrite(endcode, 1, sizeof(endcode), f);
	fclose(f);
	sws_freeContext(convert);
	avcodec_close(ctx);
	avcodec_free_context(&ctx);
	av_frame_free(&frame);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glDeleteBuffers(pimpl->reservedFrames - 10, &pimpl->pbos[10]);
	pimpl->reservedFrames = 10;
	pimpl->pbos.resize(10);
	pimpl->freeFrame = 0;

}
