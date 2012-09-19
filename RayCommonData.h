/*
 * 将控制类、绘制类、显示类
 * 公用的数据放在此类中
 */

#ifndef __RAYCOMMONDATA_H__
#define __RAYCOMMONDATA_H__

#include <vector>
#include "RayPixel.h"

struct RayCommonData
{
	std::vector<std::vector<RayPixel> > scene;		// 以像素形式保存已经绘制好的场景
	int m_nScrWidth, m_nScrHeight;			// 屏幕宽度和高度
	int m_nSceneWidth, m_nSceneHeight;		// 场景宽度和高度
	int m_nThreadNum;						// 绘制的线程总数
	volatile int m_nFinishHeight;			// 绘制完成的行数
	bool m_bFinished;						// 模型是否已经绘制完成
	RayCommonData();
};

#endif
