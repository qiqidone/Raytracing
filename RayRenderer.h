/*
 * ������һ��RayRenderer�࣬������һ��ָ��ObjDada��
 * ���ݵ�ָ��objData��ͨ��������ʵ����һ��ObjData����
 * �Ӷ��ﵽ���ʶ���������ݵ�Ŀ�ġ�
 * ��Ҫ������ʹ�ù��߸�������㷨�������ÿ�����ص�
 * ��ɫֵ��������洢�������У���RayView�����
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
