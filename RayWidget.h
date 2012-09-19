/*
 * ������һ��RayWidget�࣬�������������
 * �������Լ�������������簴ť����ǩ�ȡ�
 */

#ifndef __RAYWIDGET_H__
#define __RAYWIDGET_H__

#include "arthurwidgets.h"
#include "RayRenderer.h"
#include "RayView.h"
#include "RayCommonData.h"
#include <QtGui>

class RayWidget: public QWidget
{
	Q_OBJECT
public:
	RayWidget();
private:
	RayRenderer *m_rayRenderer;
	RayView *m_rayView;
	RayCommonData m_commonData;

	QPushButton *loadButton;				// ���ļ��İ�ť
	QLCDNumber *vertexLCDNumber;			// ��ʾ����������Һ����
	QLCDNumber *textureLCDNumber;			// ��ʾ����������Һ����
	QLCDNumber *normalLCDNumber;			// ��ʾ����������Һ����
	QLCDNumber *faceLCDNumber;				// ��ʾ��������Һ����
	QLCDNumber *objLCDNumber;				// ��ʾ����������Һ����
	QLCDNumber *renderTimeLCDNumber;		// ��ʾ���õĻ���ʱ��
	QLCDNumber *progressLCDNumber;			// ��ʾ������ɽ���
	QTimer *updateTimer;
	QSlider *threadNumSlider;
	QSpinBox *threadNumSpinBox;

	int m_nVertexNum;						// ��������
	int m_nTextureNum;						// ��������
	int m_nNormalNum;						// ��������
	int m_nFaceNum;							// ������
	int m_nObjNum;							// ��������
	int m_nRenderTime;						// ���Ƴ�������ʱ��
	bool m_bUseBSP;
private slots:
	void open();							// ���ļ�
	void updateRenderTime();
	void setUseBSP();
	void setNotUseBSP();
	void setThreadNum(int threadNum);
};

#endif
