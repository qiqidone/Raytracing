/*
 * 定义了一个RayWidget类，用来创建程序的
 * 主界面以及各个组件，例如按钮、标签等。
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

	QPushButton *loadButton;				// 打开文件的按钮
	QLCDNumber *vertexLCDNumber;			// 显示顶点数量的液晶屏
	QLCDNumber *textureLCDNumber;			// 显示纹理数量的液晶屏
	QLCDNumber *normalLCDNumber;			// 显示法向数量的液晶屏
	QLCDNumber *faceLCDNumber;				// 显示面数量的液晶屏
	QLCDNumber *objLCDNumber;				// 显示物体数量的液晶屏
	QLCDNumber *renderTimeLCDNumber;		// 显示已用的绘制时间
	QLCDNumber *progressLCDNumber;			// 显示绘制完成进度
	QTimer *updateTimer;
	QSlider *threadNumSlider;
	QSpinBox *threadNumSpinBox;

	int m_nVertexNum;						// 顶点数量
	int m_nTextureNum;						// 纹理数量
	int m_nNormalNum;						// 法向数量
	int m_nFaceNum;							// 面数量
	int m_nObjNum;							// 物体数量
	int m_nRenderTime;						// 绘制场景所用时间
	bool m_bUseBSP;
private slots:
	void open();							// 打开文件
	void updateRenderTime();
	void setUseBSP();
	void setNotUseBSP();
	void setThreadNum(int threadNum);
};

#endif
