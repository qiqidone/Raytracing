/*
 * 这个类表示计算光线跟踪的线程
 * 用于Qt多线程编程
 */

#ifndef __RAYTHREAD_H__
#define __RAYTHREAD_H__

#include <QThread>
#include "RayCommonData.h"
#include "RayVectorColor.h"
#include "RayBSPTree.h"

class RayThread : public QThread
{
	Q_OBJECT
private:
	static const double ZERO;
	static const double NEGA_ZERO;
	static const double RAY_INFINITY;
	static const double RAY_NEGA_INFINITY;
	static const double BOX_COEFFICIENT;

	RayCommonData *m_pCommonData;
	int m_nNum, m_nStride;
	RayVector eyePoint;
	double scrPlane;
	int maxDepth;
	bool m_bUseBSP;
	RayBSPTree *bspTree;
	ObjData *objData;
	RayColor ia;
	double m_fMinX, m_fMaxX, m_fMinY, m_fMaxY, m_fMinZ, m_fMaxZ;
	volatile bool m_bIsRunning;

	void traceRay(RayVector start, RayVector direction, int depth, RayColor &color, bool isInside);
	void shade(const int objIndex, const int faceIndex, RayVector interPoint, RayColor& localColor, bool isPoly,
			   RayVector &N, RayColor &ks, bool &isSpecular, bool &isTransparent);
	void normalInterpolate(RayVector &N, const RayVector &interPoint, const int objIndex, const int faceIndex);
	bool findNearestPoly(const RayVector &start, const RayVector &direction, double &T, int &objIndex,
						 int &faceIndex, bool exact, bool isInside, RayColor &ktFac);
	bool findInterPointPoly(int objIndex, int faceIndex, const RayVector &start,
							const RayVector &direction, double &t);
	bool findNearestSphere(const RayVector &start, const RayVector &direction, double &T, int &objIndex,
						   int &faceIndex, bool exact, bool isInside, RayColor &ktFac);
	int findInterPointSphere(int objIndex, int faceIndex, const RayVector &start,
							 const RayVector &direction, double &t0, double &t1);
	bool findNearestPointBSP(const RayVector &start, const RayVector &direction, double &T, int &objIndex,
							 int &faceIndex, double tMin, double tMax, RayBSPTree *node, bool &isPoly,
							 bool exact, RayColor& ktFac);
	bool RayIntersectBox(const RayVector &start, const RayVector &direction, double &tMin, double &tMax);
public:
	RayThread(RayCommonData *caller, int num, int stride, RayVector eyePoint,
			  double scrPlane, int m_nMaxDepth, bool useBSP, RayBSPTree *bspTree,
			  ObjData *objData, RayColor ia,
			  double minX, double maxX, double minY, double maxY, double minZ, double maxZ);
	bool isRunning() const {return m_bIsRunning;}
protected:
	void run();
};

#endif
