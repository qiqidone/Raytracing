#include "RayWidget.h"
#include "arthurstyle.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));

	RayWidget rayWidget;
	QStyle *arthurStyle = new ArthurStyle();
	rayWidget.setStyle(arthurStyle);

	QList<QWidget *> widgets = qFindChildren<QWidget *>(&rayWidget);
	foreach (QWidget *w, widgets)
		w->setStyle(arthurStyle);

	rayWidget.show();

	return app.exec();
}
