#include "specialeffect.h"
SpecialEffect::SpecialEffect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	fileManager = new FileManager();
	videoShowController = new VideoShowController();
	ui.verticalLayout->addWidget(fileManager);
	ui.videoLayout->addWidget(videoShowController);
	setStyleSheet("background-color:gray");
	connect(ui.actionOpen, SIGNAL(triggered(bool)), fileManager, SLOT(openFile(bool)));
	connect(fileManager, SIGNAL(filePath(const QString)), videoShowController, SLOT(startPlayName(const QString)));//Ö±½Ó²¥·Å
}
SpecialEffect::~SpecialEffect()
{
	
}
