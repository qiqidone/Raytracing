/*
 * 这个类的直观表示是界面左边很大的绘图框，
 * 主要作用是将缓存中的结果绘制到屏幕上。
 */

#ifndef __RAYVIEW_H__
#define __RAYVIEW_H__

#include "arthurwidgets.h"
#include "RayCommonData.h"
#include <QtGui>

using namespace std;

class RayView: public ArthurFrame
{
	Q_OBJECT;
private:
	RayCommonData *m_pCommonData;
public:
	RayView(QWidget *parent, RayCommonData *commonData);
	QSize sizeHint() const {return QSize(500, 500);}
	void paint(QPainter *painter);
};

#endif
