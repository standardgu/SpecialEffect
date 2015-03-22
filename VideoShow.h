#ifndef VIDEOSHOW_H
#define VIDEOSHOW_H
#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QMouseEvent>
#include <QString>
#include <opencv2\imgproc\types_c.h>
#include <QtCore\qobject.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv\cv.h>
#include <QtGui\qwindow.h>
#include "ui_VideoShowWidget.h"
class  TimeLabel:public QLabel
{
public:
	TimeLabel(QWidget *parent = 0);
	~TimeLabel();
private:

};
class TimeSlider :public QSlider
{
	Q_OBJECT
public:
	TimeSlider();
	~TimeSlider();
signals:
	void seekPosition(double position);
protected:
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
private:
	TimeLabel *timeLabel;//显示当前时间
	QTimer *timer;
	int fps,frameNumber;//存储帧率
	int position;
	int hour, min, sec;
private slots:
       void hideTime();//隐藏时间
public slots:
     void setTime(QString Time);//设置显示时间
	 void setFps(int videoFps,int frameNumber);//设置帧速率
	 QString changToTimeString(int nowFrameNumber);

	

};
class VideoShow : public QLabel
{
	Q_OBJECT

public:
	VideoShow(QWidget *parent = 0);
	~VideoShow();
public slots:
	void setImage(IplImage *pIplImage);
	void setImage(QImage *img);
	void paintEvent(QPaintEvent *);
private:
	QImage *img;
	bool flag;
	uchar *qImageBuffer;
};
class VideoShowController :public QWidget
{
	Q_OBJECT
public:
    VideoShowController();
	~VideoShowController();
private:
	Ui::Form ui;
	VideoShow *videoShow;
	CvCapture *videoCapture;
	TimeSlider *timeSlider;
	QTimer *timer;
	IplImage * frame;//声明视频帧
	int frameNumber;//视频总数
	int nowFrameNumber;//现在所在的帧数
	int fps;//视频帧速率
	int hour, min, sec;//记录时间
signals:
	void sendImage(IplImage *pIplImage);
public slots:
	void startPlayName(const QString); 
private slots:
    void nextFrame();
	void setPosition(double position);
	void setPosition();
	QString changToTimeString(int nowFrameNumber);
	void setPlayState(bool state);
};
#endif // VIDEOSHOW_H
