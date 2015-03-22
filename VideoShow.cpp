#include "videoshow.h"
#include <iostream>
#include <cstring>
#include <opencv2\highgui\highgui.hpp>
#include <opencv\cv.h>
#include <QtGui\qpainter.h>
#include <QVector>
#include <QBoxLayout>
#include <QTimer>
#include <qdebug.h>
using std::string;
using std::iostream;
TimeLabel::TimeLabel(QWidget *parent) :QLabel(parent)
{
	setStyleSheet("background:transparent;color:red;");
}
TimeLabel::~TimeLabel()
{

}
TimeSlider::TimeSlider()
{
//设置样式
	setStyleSheet(QLatin1String("QSlider::handle:horizontal {\n"
		"\n"
		"    /* background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);*/\n"
		"\n"
		"     /*border: 0px solid #5c5c5c;*/\n"
		"     width: 1px;\n"
		"     margin: 0px 0; /* handle is placed by default on the contents rect of the groove. Expand outside the groove */\n"
		"   /* border-radius: 1px;*/\n"
		"   color:green;\n"
		"   background:red;\n"
		" }\n"
		"QSlider::groove:horizontal {\n"
		"     border: 1px solid #999999;\n"
		"     height:12px; /* the groove expands to the size of the slider by default. by giving it a height, it has a fixed size */\n"
		"     background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);\n"
		"     margin: 2px 0;\n"
		" border-radius: 3px;\n"
		"\n"
		" }"));
    setOrientation(Qt::Horizontal);
	timeLabel = new TimeLabel(this);
	timeLabel->setFixedSize(60, 20);
    setMouseTracking(true);
	fps = -1;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()),this, SLOT(hideTime()));
}
void TimeSlider::hideTime()
{
	timer->stop();
	timeLabel->hide();
}
void TimeSlider::setTime(QString time)//设置显示时间
{
	timeLabel->setText(time);
}
void TimeSlider::setFps(int videoFps,int f)
{
	fps = videoFps;
	frameNumber = f;
}
void TimeSlider::mouseMoveEvent(QMouseEvent * event)
{
	if (fps != -1)
	{
		
		if(event->pos().x() - timeLabel->size().width()/2<0)
			timeLabel->move(0, pos().y());
		else 
			timeLabel->move(event->pos().x()- timeLabel->size().width() / 2, pos().y());
		printf("event->pos().x() - timeLabel->size().width() / 2=%ld\n", event->pos().x() - timeLabel->size().width() / 2);
		position = (event->localPos().x()*1.0 / size().rwidth())*frameNumber;
		timeLabel->setText(changToTimeString(int(position)));
		timeLabel->show();
		timer->start(100);
		//emit seekPosition(event->localPos().x()*1.0 / size().rwidth());
	}
}
void TimeSlider::mousePressEvent(QMouseEvent * event)
{
	if (fps != -1)
	{
		timeLabel->move(event->pos().x() - timeLabel->size().width() / 2, pos().y());
		position = value();
		timeLabel->show();
		timeLabel->setText(changToTimeString(int(position)));
		timer->start(500);
		emit seekPosition(event->localPos().x()*1.0 / size().rwidth());
	}
}
QString TimeSlider::changToTimeString(int nowFrameNumber)
{
	QString timeString;
	if (fps!=-1)
		sec = nowFrameNumber / fps;
	min = sec / 60;
	sec = sec % 60;
	hour = min / 60;
	min = min % 60;
	if (min < 10)
	{
		timeString += "0";
		timeString += QString::number(hour);
	}
	else
	{
		timeString += QString::number(hour);

	}
	timeString += ":";
	if (min < 10)
	{
		timeString += "0";
		timeString += QString::number(min);
	}
	else
	{
		timeString += QString::number(min);

	}
	timeString += ":";
	if (sec < 10)
	{
		timeString += "0";
		timeString += QString::number(sec);
	}
	else
	{
		timeString += QString::number(sec);

	}
	return timeString;

}
TimeSlider::~TimeSlider()
{
	delete timeLabel;
}
VideoShow::VideoShow(QWidget *parent)
: QLabel(parent)
{
	flag = false;
	resize(1024, 1024);
	setWindowTitle(tr("Paint Demo"));
	img = NULL;
	qImageBuffer = NULL;

}
void VideoShow::setImage(QImage *imgShow)
{
	img = imgShow;
	flag = true;
	update();
}
void VideoShow::setImage(IplImage *frame)
{
	/*int height = iplImage->height;
	int width = iplImage->width;
	if (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 3) {

	const uchar *qImageBuffer = (const uchar*)iplImage->imageData;

	img=new QImage(qImageBuffer, width, height, QImage::Format_RGB888);

	img->rgbSwapped();

	}
	else if (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 1){

	const uchar *qImageBuffer = (const uchar*)iplImage->imageData;

	img=new QImage(qImageBuffer, width, height, QImage::Format_Indexed8);

	QVector<QRgb> colorTable; for (int i = 0; i < 256; i++){

	colorTable.push_back(QColor(i, i, i).rgb());

	}
	img->setColorTable(colorTable);
	}
	*/

	/*qImageBuffer=(uchar *)malloc(frame->width*frame->height*sizeof(uchar));
	if (img != NULL)
	{
	flag = false;
	free(img);
	}
	img=IplImageToQImage(frame,&qImageBuffer);
	update();*/
	//cvxCopyIplImage(frame, *img);
	//free(qImageBuffer);
	/*if (!frame)
	return ;
	QImage* desImage = new QImage(frame->width, frame->height, QImage::Format_RGB32);

	for (int i = 0; i < frame->height; i++)
	{
	for (int j = 0; j < frame->width; j++)
	{
	int r, g, b;

	if (3 == frame->nChannels)
	{
	b = (int)CV_IMAGE_ELEM(frame, uchar, i, j * 3 + 0);
	g = (int)CV_IMAGE_ELEM(frame, uchar, i, j * 3 + 1);
	r = (int)CV_IMAGE_ELEM(frame, uchar, i, j * 3 + 2);

	}
	else if (1 == frame->nChannels)
	{
	b = (int)CV_IMAGE_ELEM(frame, uchar, i, j);
	g = b;
	r = b;
	}

	desImage->setPixel(j, i, qRgb(r, g, b));
	}
	}
	img = desImage;*/
	cvCvtColor(frame, frame, CV_BGR2RGB);
	img = new QImage((uchar *)frame->imageData, frame->width, frame->height, QImage::Format_RGB888);
	img->rgbSwapped();
	flag = true;
	update();
}
void VideoShow::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (flag)
		painter.drawImage(rect(), *img);
}
VideoShow::~VideoShow()
{

}
VideoShowController::VideoShowController()
{
	ui.setupUi(this);
	videoShow = new VideoShow();
	timeSlider = new TimeSlider();
	ui.VideoLayout->addWidget(videoShow);
	ui.ControlToolLayout->addWidget(timeSlider);
	videoShow->show();
	connect(this, SIGNAL(sendImage(IplImage *)), videoShow, SLOT(setImage(IplImage *)));
	connect(timeSlider, SIGNAL(seekPosition(double)), this, SLOT(setPosition(double)));
	connect(ui.startorPauseButton, SIGNAL(clicked(bool)), this, SLOT(setPlayState(bool)));
	frameNumber = 0;
	timeSlider->setRange(0, 0);
}
void VideoShowController::startPlayName(const QString fileName)
{
	videoCapture = cvCreateFileCapture(fileName.toStdString().c_str());//打开文件结构
	if (!videoCapture)
		printf("file open failed\n"); 
	frameNumber = (int)cvGetCaptureProperty(videoCapture, CV_CAP_PROP_FRAME_COUNT);//返回视频帧的总数 
	timeSlider->setRange(0, frameNumber);
    fps = (int)cvGetCaptureProperty(videoCapture, CV_CAP_PROP_FPS);
	timeSlider->setFps(fps,frameNumber);
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
	//timer->start(1000 /fps);
	timer->start(2);
	frame = cvQueryFrame(videoCapture);  //获取下一帧图像  
	if (!frame)
	{
		printf("cannot get nextFrame\n");
	}
	else
		emit sendImage(frame);
}
void VideoShowController::setPlayState(bool state)
{
	if (timer->isActive())
	{
		timer->stop();
	}
	else
	{
		timer->start();
	}
	
}
QString VideoShowController::changToTimeString(int nowFrameNumber)
{
	QString timeString;
	if (fps)
		sec = nowFrameNumber / fps;
	min = sec / 60;
	sec = sec % 60;
	hour = min / 60;
	min = min % 60;
	if (min < 10)
	{
		timeString += "0";
		timeString += QString::number(hour);
	}
	else
	{
		timeString += QString::number(hour);

	}
	timeString += ":";
	if (min < 10)
	{
		timeString += "0";
		timeString += QString::number(min);
	}
	else
	{
		timeString += QString::number(min);

	}
	timeString += ":";
	if (sec < 10)
	{
		timeString += "0";
		timeString += QString::number(sec);
	}
	else
	{
		timeString += QString::number(sec);

	}
	return timeString;
	
}
void VideoShowController::nextFrame()
{
	frame = cvQueryFrame(videoCapture);  //获取下一帧图像  
	if (!frame)
	{
		printf("cannot get nextFrame\n");
	}
	nowFrameNumber = (int)cvGetCaptureProperty(videoCapture,CV_CAP_PROP_POS_FRAMES);
	timeSlider->setValue(nowFrameNumber);
	emit sendImage(frame);
}
void VideoShowController::setPosition()
{

	cvSetCaptureProperty(videoCapture, CV_CAP_PROP_POS_FRAMES, timeSlider->value());
}
void VideoShowController::setPosition(double position)
{
	
	printf("position*frameNumber=%d\n",(int)( position*frameNumber));
	printf("position=%lf\n", position);
	printf("frameNumber=%d\n", frameNumber);
	if (frameNumber> 0)//才可以寻找
	{
		cvSetCaptureProperty(videoCapture, CV_CAP_PROP_POS_FRAMES, (int)(position*frameNumber));
		nextFrame();
	}
	timeSlider->setValue((int)(position*frameNumber));
	timeSlider->setTime(changToTimeString((int)(position*frameNumber)));
	
	
}
VideoShowController::~VideoShowController()
{



}

