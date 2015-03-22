#ifndef VIDEODECODETHREAD
#define VIDEODECODETHREAD
#include <iostream>
#include <QImage>
#include <QThread>
#include <QMutex>
#include <QString>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
using namespace std;
const int  CV_FFMPEG_CAP_PROP_POS_MSEC = 0;
const int CV_FFMPEG_CAP_PROP_POS_FRAMES = 1;
const int CV_FFMPEG_CAP_PROP_POS_AVI_RATIO = 2;
class VideoDecodeThread:public QThread
{
	Q_OBJECT
public:
	VideoDecodeThread(QString);
	VideoDecodeThread();
	~VideoDecodeThread();
signals:
	void sendImage(QImage *imgShow);
protected:
	void run();
public slots:
  void setPlayName(const QString);
   void startorPause(bool);//控制播放或者暂停
   double get_duration_sec();//获取视频时长
   bool setProperty(int property_id, double value);//进行设置播放位置
   double getProperty(int property_id);
   void preSecond(bool flag);//回到前一秒
private:
	QImage *img;
	QMutex imgMutex;
	bool  notPlay;
	unsigned int videoStream ;
	AVCodecContext *pCodecCtx;
	AVFormatContext *pFormatCtx;
	AVCodec *pCodec;
	AVFrame *pFrame, *pFrameRGB;
	struct SwsContext *pSwsCtx;
	AVPacket packet;
	int frameFinished;
	int PictureSize;
	uint8_t *buf;
	QString fileName;
	int64_t time_stamp;
	bool isSeek;
	double epsZero;
	bool valid;
	double nowTime;
	int64_t     picture_pts;
	double r2d(AVRational r);//基转换
	int get_bitrate();
	double get_fps();
	int64_t frame_number, first_frame_number;
	int64_t get_total_frames();
	int64_t dts_to_frame_number(int64_t dts);
	double dts_to_sec(int64_t dts);
	void seek(int64_t _frame_number);
	void seek(double sec);
};
#endif
