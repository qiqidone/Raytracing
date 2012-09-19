/*
 * 定义了一个RayRenderer类，其中有一个指向ObjDada类
 * 数据的指针objData，通过它可以实例化一个ObjData对象，
 * 从而达到访问顶点和面数据的目的。
 * 主要所用是使用光线跟踪相关算法，计算出每个象素的
 * 颜色值，并将其存储到缓存中，供RayView类绘制
 */

#ifndef __RAYRENDERER_H__
#define __RAYRENDERER_H__

#include "RayVectorColor.h"
#include "RayObjData.h"
#include "RayCommonData.h"
#include "RayBSPTree.h"
#include "RayThread.h"
#include <QtGui>
#include <vector>

class RayRenderer
{
private:
	static const double BOX_COEFFICIENT;

	ObjData *objData;
	double n, bx, by, bz, scrPlane;
	double m_fMinX, m_fMaxX, m_fMinY, m_fMaxY, m_fMinZ, m_fMaxZ;
	int maxDepth;
	bool m_bUseBSP;
	RayVector eyePoint;
	RayColor ia;
	RayBSPTree *bspTree;
	RayCommonData *m_pCommonData;
	std::vector<RayThread *> threadList;
public:
	RayRenderer(RayCommonData *commonData);
	~RayRenderer();
	void readObj(const char *fileName);
	void renderScene(bool useBSP);
	int getVertexNum();
	int getTextureNum();
	int getNormalNum();
	int getFaceNum();
	int getObjNum();
};

#endif
