/*
 * 定义了表示二叉空间剖分树（bsp tree）的类，
 * 用于光线跟踪的加速
 */

#ifndef __RAYBSPTREE_H__
#define __RAYBSPTREE_H__

#include <vector>
#include "RayObjData.h"

struct ObjFaceIndex
{
	int objIndex;			// 物体编号
	int faceIndex;			// 面片编号
	ObjFaceIndex(int oIndex, int fIndex);
	ObjFaceIndex(const ObjFaceIndex &objFaceIndex);
};

enum Axis
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};

struct RayBSPTree
{
	static const int MAXDEPTH = 10;
	static const int MAXELEMENT = 8;
	static const double ZERO;
	double e1i, e1j, e1k, e2i, e2j, e2k, di, dj, dk, Li, Lj, Lk;
	ObjData *objData;
	RayBSPTree *leftChild, *rightChild;
	std::vector<ObjFaceIndex> faceIndexList;
	std::vector<ObjFaceIndex> sphereIndexList;
	Axis m_nAxis;
	double m_fMinX, m_fMaxX, m_fMinY, m_fMaxY, m_fMinZ, m_fMaxZ;
	RayBSPTree(double xMin, double mMax, double yMin, double yMax, double zMin, double zMax, ObjData *oData);
	~RayBSPTree();
	void subdivide(int currentDepth, Axis currentAxis);
	bool boxIntersectSphere(RayBSPTree *tree, int objIndex, int sphereIndex);
	bool boxIntersectPoly(RayBSPTree *tree, int objIndex, int faceIndex);
	bool insideBox(RayBSPTree *tree, const RayVector &point);
	bool solutionExists();
	double C0();
	double Ci();
	double Cj();
	double Ck();
	double a0();
	double ai();
	double aj();
	double ak();
	double deti();
	double detj();
	double detk();
	double b0();
	double bi();
	double bj();
	double bk();
	//void print(FILE *f, double cc);
	//FILE *fptr;
};

#endif
