#include "VideoDecodeThread.h"
VideoDecodeThread::VideoDecodeThread(QString filePlay):QThread()
{
	fileName = filePlay;
	notPlay = false;
	videoStream = -1;
	epsZero = 0.000025;
	first_frame_number = -1;
	valid = false;
	picture_pts = AV_NOPTS_VALUE;
	frame_number = 0;
	isSeek = false;
	nowTime = 0.0;
}
VideoDecodeThread::VideoDecodeThread() :QThread()
{
	notPlay = false;
	videoStream = -1;
	epsZero = 0.000025;
	first_frame_number = -1;
	valid = false;
	picture_pts = AV_NOPTS_VALUE;
	frame_number = 0;
	isSeek = false;
	nowTime = 0.0;
}
void VideoDecodeThread::setPlayName(const QString filePlay)
{
	fileName = filePlay;
	start();
}
void VideoDecodeThread::startorPause(bool flag)
{
	if (notPlay)
		notPlay = false;
	else
		notPlay = true;
	isSeek = false;
}
double VideoDecodeThread::r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}
double VideoDecodeThread::get_duration_sec()
{
	double sec = (double)pFormatCtx->duration / (double)AV_TIME_BASE;

	if (sec < epsZero)
	{
		sec = (double)pFormatCtx->streams[videoStream]->duration * r2d(pFormatCtx->streams[videoStream]->time_base);
	}
	return sec;
}
int VideoDecodeThread::get_bitrate()
{
	return pFormatCtx->bit_rate;
}
double VideoDecodeThread::get_fps()
{
	double fps = r2d(pFormatCtx->streams[videoStream]->r_frame_rate);
	if (fps < epsZero)
	{
		fps = r2d(pFormatCtx->streams[videoStream]->avg_frame_rate);
	}
	return fps;
}
int64_t VideoDecodeThread::get_total_frames()
{
	int64_t nbf = pFormatCtx->streams[videoStream]->nb_frames;
	if (nbf == 0)
	{
		nbf = (int64_t)floor(get_duration_sec() * get_fps() + 0.5);
	}
	return nbf;
}
int64_t VideoDecodeThread::dts_to_frame_number(int64_t dts)
{
	double sec = dts_to_sec(dts);
	return (int64_t)(get_fps() * sec + 0.5);
}
double VideoDecodeThread::dts_to_sec(int64_t dts)
{
	return (double)(dts - pFormatCtx->streams[videoStream]->start_time) *
		r2d(pFormatCtx->streams[videoStream]->time_base);
}

void VideoDecodeThread::seek(double sec)
{
	seek((int64_t)(sec * get_fps() + 0.5));
}
bool VideoDecodeThread::setProperty(int property_id, double value)
{
	if (!pFormatCtx->streams[videoStream]) 
		return false;
	switch (property_id)
	{
	case CV_FFMPEG_CAP_PROP_POS_MSEC:
	case CV_FFMPEG_CAP_PROP_POS_FRAMES:
	case CV_FFMPEG_CAP_PROP_POS_AVI_RATIO:
	{
       switch (property_id)
	   {
	   case CV_FFMPEG_CAP_PROP_POS_FRAMES:
		   seek((int64_t)value);
		   break;

	   case CV_FFMPEG_CAP_PROP_POS_MSEC:
		   seek(value / 1000.0);
		   break;

	   case CV_FFMPEG_CAP_PROP_POS_AVI_RATIO:
		   seek((int64_t)(value*pFormatCtx->duration));
		   break;
	   }
	   picture_pts = (int64_t)value;
	}
		break;
	default:
		return false;
	}
	return true;
}
double VideoDecodeThread::getProperty(int property_id)
{
	if (!pFormatCtx->streams[videoStream]) return 0;

	switch (property_id)
	{
	case CV_FFMPEG_CAP_PROP_POS_MSEC:
		return 1000.0*(double)frame_number/get_fps();
	case CV_FFMPEG_CAP_PROP_POS_FRAMES:
		return (double)frame_number;
	case CV_FFMPEG_CAP_PROP_POS_AVI_RATIO:
		return r2d(pFormatCtx->streams[videoStream]->time_base);
	default:
		return 0;
	}
}
void VideoDecodeThread::preSecond(bool flag)//回到前一秒
{
	if (nowTime - 1000.0 > 0.0000025)
		setProperty(CV_FFMPEG_CAP_PROP_POS_MSEC, nowTime - 1000);
	else
		setProperty(CV_FFMPEG_CAP_PROP_POS_MSEC, 0.0);
	
}
void VideoDecodeThread::seek(int64_t _frame_number)
{
	_frame_number = std::min(_frame_number, get_total_frames());
	int delta = 16;//偏移量
	//// if we have not grabbed a single frame before first seek, let's read the first frame
	//// and get some valuable information during the process
	int64_t _frame_number_temp = std::max(_frame_number - delta, (int64_t)0);
	double sec = (double)_frame_number_temp / get_fps();
	time_stamp = pFormatCtx->streams[videoStream]->start_time;
	double  time_base = r2d(pFormatCtx->streams[videoStream]->time_base);
	time_stamp += (int64_t)(sec / time_base + 0.5);
	if (get_total_frames()> 1) 
	frame_number = dts_to_frame_number(picture_pts) - first_frame_number;
	notPlay = false;
	isSeek = true;
	
}

void VideoDecodeThread::run()
{
	setPriority(QThread::HighestPriority);
	//注册编解码器
	av_register_all();
	//打开视频文件
	pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&pFormatCtx, fileName.toUtf8().data(), NULL, NULL) != 0)
	{

		printf("av open input file failed!\n");
		exit(1);
	}
	//获取流信息
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("av find stream info failed!\n");
		exit(1);
	}
	//获取视频流
	int i;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
	if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		videoStream = i;
		break;
	}

	if (videoStream == -1)
	{
		printf("find video stream failed!\n");
		exit(1);
	}

	pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

	if (pCodec == NULL)
	{
		printf("avcode find decoder failed!\n");
		exit(1);
	}
	//打开解码器
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("avcode open failed!\n");
		exit(1);
	}
	//为每帧图像分配内存
	pFrame = avcodec_alloc_frame();
	pFrameRGB = avcodec_alloc_frame();
	if ((pFrame == NULL) || (pFrameRGB == NULL))
	{
		printf("avcodec alloc frame failed!\n");
		exit(1);
	}
	PictureSize = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	buf = (uint8_t*)av_malloc(PictureSize);
	if (buf == NULL)
	{
		printf("av malloc failed!\n");
		exit(1);
	}
	int dst_fmt = PIX_FMT_RGB24;
	int dst_w = pCodecCtx->width;
	int dst_h = pCodecCtx->height;
	avpicture_fill((AVPicture *)pFrameRGB, buf, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	//设置图像转换上下文
	pSwsCtx = sws_getContext(pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		PIX_FMT_RGB24,
		SWS_BICUBIC,
		NULL, NULL, NULL);
	img = new QImage(dst_w, dst_h, QImage::Format_RGB32);
	while (true)
	{
		
		while (notPlay)
		{
			usleep(100);
		}
		if (isSeek)
		{
			av_seek_frame(pFormatCtx, videoStream, time_stamp, AVSEEK_FLAG_BACKWARD);
			avcodec_flush_buffers(pFormatCtx->streams[videoStream]->codec);
			isSeek = false;
		}
		if(av_read_frame(pFormatCtx, &packet) >= 0)
		{
		if (packet.stream_index == videoStream)
		{
			/*avcodec_decode_video(pCodecCtx, pFrame, &frameFinished,
			packet.data, packet.size);*/
			packet.data = (unsigned char*)packet.data;
			printf("packet.dts=%lld\n", packet.dts);
			//printf("frameNumber=%lld\n", dts_to_frame_number(packet.dts));
			//printf("nowTime=%lf\n",dts_to_sec(packet.dts));
			imgMutex.lock();
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			if (frameFinished)
			{
				//反转图像 ，否则生成的图像是上下调到的
				/*pFrame->data[0] += pFrame->linesize[0] * (pCodecCtx->height - 1);
				pFrame->linesize[0] *= -1;
				pFrame->data[1] += pFrame->linesize[1] * (pCodecCtx->height / 2 - 1);
				pFrame->linesize[1] *= -1;
				pFrame->data[2] += pFrame->linesize[2] * (pCodecCtx->height / 2 - 1);
				pFrame->linesize[2] *= -1;*/
				//转换图像格式，将解压出来的YUV420P的图像转换为BRG24的图像
				if (picture_pts == AV_NOPTS_VALUE)
				{
					picture_pts = packet.pts != AV_NOPTS_VALUE&& packet.pts != 0 ? packet.pts : packet.dts;
				}
				if (first_frame_number < 0)//初始化第一个frame编号
				{
					first_frame_number = dts_to_frame_number(picture_pts);
				}
				nowTime = getProperty(CV_FFMPEG_CAP_PROP_POS_MSEC);
				printf("nowTime=%llf\n",nowTime);//可以获得当前的时间
				frame_number++;
				sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
				uint8_t *src = (uint8_t *)(pFrameRGB->data[0]);
				for (int y = 0; y < dst_h; y++)
				{
					QRgb *scanLine = (QRgb *)img->scanLine(y);
					for (int x = 0; x < dst_w; x = x + 1)
					{
						scanLine[x] = qRgb(src[3 * x], src[3 * x + 1], src[3 * x + 2]);
					}
					src += pFrameRGB->linesize[0];
				}
				emit sendImage(img);
			}
			imgMutex.unlock();
		}
		av_free_packet(&packet);
	}
}
	sws_freeContext(pSwsCtx);
	av_free(pFrame);
	av_free(pFrameRGB);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
}
VideoDecodeThread::~VideoDecodeThread()
{

}
