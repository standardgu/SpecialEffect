#include "FileManager.h"
#include<QHeaderView>
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QTextCodec>
#include <QMessageBox>
#include <qdebug.h>
FileManager::FileManager(int rows, int columns, QWidget * parent) :QTableWidget(rows, columns, parent)
{
	setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
	setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中的方式
	setSelectionMode(QAbstractItemView::ExtendedSelection);//设置为可以选中多个目标
	//QTableWidgetItem *columnHeaderItem0 = horizontalHeaderItem(0);//获得水平方向表头的Item对象
	verticalHeader()->setVisible(false);//隐藏列表头
	horizontalHeader()->setVisible(false);
	resizeColumnToContents(0);//根据内容调整列宽
	setItem(0, 1, new QTableWidgetItem("Name"));//第一行第二列的内容设置为abc
	setItem(0, 2, new QTableWidgetItem("Frame Rate"));//第一行第一列的内容设置为abc
	setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
	connect(this,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(sendFilePath(const QModelIndex & )));
	fileList << "Name";
	setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px;");
	
}
void FileManager::addFile(QStringList filePath)
{
	fileList << filePath;
	int fileLength = filePath.size();
	for (int i = 0; i < fileLength; i++)
	{
		CvCapture *capture = cvCreateFileCapture(filePath.at(i).toStdString().c_str());
		double fps = (double)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);//帧速率
		cvReleaseCapture(&capture);
		QFileInfo file(filePath.at(i));
		insertRow(rowCount());
		setItem(rowCount() - 1, 1, new QTableWidgetItem(file.baseName()));//第i行第二列的内容设置为abc
		setItem(rowCount() - 1, 2, new QTableWidgetItem(QString::number(fps) + "fps"));//第i行第三列的内容设置为abc
	}
}
void FileManager::addFile(QString filePath)
{
	fileList << filePath;
	CvCapture *capture = cvCreateFileCapture(filePath.toStdString().c_str());
	double fps = (double)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);//帧速率
	cvReleaseCapture(&capture);
	QFileInfo file(filePath);
	insertRow(rowCount());
	setItem(rowCount() - 1, 1, new QTableWidgetItem(file.baseName()));//第i行第二列的内容设置为abc
	setItem(rowCount() - 1, 2, new QTableWidgetItem(QString::number(fps) + "fps"));//第i行第三列的内容设置为abc
}
void FileManager::openFile(bool open)
{
	/*QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);*/
	QStringList files = QFileDialog::getOpenFileNames(
		this,
		"Select one or more files to open",
		"/",
		"Video (*.mp4 *.mov)");
	qDebug() << files << endl;
	addFile(files);
	
}
void FileManager::sendFilePath(const QModelIndex & index)//发送文件路径
{
	qDebug() << "indexRow=" << QString::number(index.row()) << endl;
	if (index.row()>0)
	emit filePath(fileList.at(index.row()));
	qDebug() << fileList.at(index.row());
}
FileManager::~FileManager()
{
}
