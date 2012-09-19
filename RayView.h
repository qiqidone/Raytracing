/*
 * ������ֱ�۱�ʾ�ǽ�����ߺܴ�Ļ�ͼ��
 * ��Ҫ�����ǽ������еĽ�����Ƶ���Ļ�ϡ�
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
