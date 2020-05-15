#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GPUTool.h"


class GPUTool : public QMainWindow
{
	Q_OBJECT

public:
	GPUTool(QWidget *parent = Q_NULLPTR);

private slots:
	void onGetGPUInfo();
	void onGetGPUInfo1();

private:
	Ui::GPUToolClass ui;
};
