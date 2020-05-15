#include "GPUTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GPUTool w;
	w.show();
	return a.exec();
}
