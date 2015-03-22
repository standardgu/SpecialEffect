#ifndef SPECIALEFFECT_H
#define SPECIALEFFECT_H

#include <QtWidgets/QMainWindow>
#include "ui_specialeffect.h"
#include "FileManager.h"
#include "VideoShow.h"

class SpecialEffect : public QMainWindow
{
	Q_OBJECT

public:
	SpecialEffect(QWidget *parent = 0);
	~SpecialEffect();

private:
	Ui::SpecialEffectClass ui;
	VideoShowController *videoShowController;
	FileManager *fileManager;
};

#endif // SPECIALEFFECT_H
