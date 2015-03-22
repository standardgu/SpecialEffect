#include "FileManager.h"
#include<QHeaderView>
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QTextCodec>
#include <QMessageBox>
#include <qdebug.h>
FileManager::FileManager(int rows, int columns, QWidget * parent) :QTableWidget(rows, columns, parent)
{
	setEditTriggers(QAbstractItemView::NoEditTriggers);//��ֹ�༭
	setSelectionBehavior(QAbstractItemView::SelectRows);//����ѡ�еķ�ʽ
	setSelectionMode(QAbstractItemView::ExtendedSelection);//����Ϊ����ѡ�ж��Ŀ��
	//QTableWidgetItem *columnHeaderItem0 = horizontalHeaderItem(0);//���ˮƽ�����ͷ��Item����
	verticalHeader()->setVisible(false);//�����б�ͷ
	horizontalHeader()->setVisible(false);
	resizeColumnToContents(0);//�������ݵ����п�
	setItem(0, 1, new QTableWidgetItem("Name"));//��һ�еڶ��е���������Ϊabc
	setItem(0, 2, new QTableWidgetItem("Frame Rate"));//��һ�е�һ�е���������Ϊabc
	setStyleSheet("selection-background-color:lightblue;"); //����ѡ�б���ɫ
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
		double fps = (double)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);//֡����
		cvReleaseCapture(&capture);
		QFileInfo file(filePath.at(i));
		insertRow(rowCount());
		setItem(rowCount() - 1, 1, new QTableWidgetItem(file.baseName()));//��i�еڶ��е���������Ϊabc
		setItem(rowCount() - 1, 2, new QTableWidgetItem(QString::number(fps) + "fps"));//��i�е����е���������Ϊabc
	}
}
void FileManager::addFile(QString filePath)
{
	fileList << filePath;
	CvCapture *capture = cvCreateFileCapture(filePath.toStdString().c_str());
	double fps = (double)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);//֡����
	cvReleaseCapture(&capture);
	QFileInfo file(filePath);
	insertRow(rowCount());
	setItem(rowCount() - 1, 1, new QTableWidgetItem(file.baseName()));//��i�еڶ��е���������Ϊabc
	setItem(rowCount() - 1, 2, new QTableWidgetItem(QString::number(fps) + "fps"));//��i�е����е���������Ϊabc
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
void FileManager::sendFilePath(const QModelIndex & index)//�����ļ�·��
{
	qDebug() << "indexRow=" << QString::number(index.row()) << endl;
	if (index.row()>0)
	emit filePath(fileList.at(index.row()));
	qDebug() << fileList.at(index.row());
}
FileManager::~FileManager()
{
}
