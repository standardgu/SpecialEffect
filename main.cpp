#include "specialeffect.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SpecialEffect w;
	w.show();
	return a.exec();
}
