#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QTableWidget>
#include <QFileInfo>
class FileManager :public QTableWidget
{
	Q_OBJECT
public:
	FileManager(int rows = 1, int columns = 3, QWidget * parent = 0);
	~FileManager();
signals:
	void filePath(const QString);
private:
	QStringList fileList;
public slots :
	void addFile(QStringList fileName);
	void addFile(QString fileName);
	void openFile(bool open);
	void sendFilePath(const QModelIndex & index);
};

#endif
