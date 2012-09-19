#include "RayBSPTree.h"
#include <math.h>

using namespace std;

ObjFaceIndex::ObjFaceIndex(int oIndex, int fIndex)
{
	objIndex = oIndex;
	faceIndex = fIndex;
}

ObjFaceIndex::ObjFaceIndex(const ObjFaceIndex &objFaceIndex)
{
	objIndex = objFaceIndex.objIndex;
	faceIndex = objFaceIndex.faceIndex;
}

const double RayBSPTree::ZERO = 0.0000001;

RayBSPTree::RayBSPTree(double minX, double maxX, double minY, double maxY, double minZ, double maxZ, ObjData *oData)
{
	m_fMinX = minX;
	m_fMaxX = maxX;
	m_fMinY = minY;
	m_fMaxY = maxY;
	m_fMinZ = minZ;
	m_fMaxZ = maxZ;
	objData = oData;
	faceIndexList.clear();
	sphereIndexList.clear();
	//fptr = f;
}

RayBSPTree::~RayBSPTree()
{
	delete leftChild;
	delete rightChild;
	faceIndexList.clear();
	sphereIndexList.clear();
}

/*************** 对空间进行细分 ***************/
void RayBSPTree::subdivide(int currentDepth, Axis currentAxis)
{
	Axis nextAxis = X_AXIS;
	int totalElement = 0;
	totalElement += faceIndexList.size();
	totalElement += sphereIndexList.size();
	if (totalElement > MAXELEMENT && currentDepth < MAXDEPTH)
	{
		m_nAxis = currentAxis;
		leftChild = new RayBSPTree(m_fMinX, m_fMaxX, m_fMinY, m_fMaxY, m_fMinZ, m_fMaxZ, objData);
		rightChild = new RayBSPTree(m_fMinX, m_fMaxX, m_fMinY, m_fMaxY, m_fMinZ, m_fMaxZ, objData);
		if (currentAxis == X_AXIS)
		{
			rightChild->m_fMinX = (m_fMaxX + m_fMinX) * 0.5;
			leftChild->m_fMaxX = rightChild->m_fMinX;
			nextAxis = Y_AXIS;
		}
		else if (currentAxis == Y_AXIS)
		{
			rightChild->m_fMinY = (m_fMaxY + m_fMinY) * 0.5;
			leftChild->m_fMaxY = rightChild->m_fMinY;
			nextAxis = Z_AXIS;
		}
		else if (currentAxis == Z_AXIS)
		{
			rightChild->m_fMinZ = (m_fMaxZ + m_fMinZ) * 0.5;
			leftChild->m_fMaxZ = rightChild->m_fMinZ;
			nextAxis = X_AXIS;
		}
		/************ 往left和right的物体列表中添加物体 *************/
		unsigned int objQuantity = faceIndexList.size();
		for (unsigned int i = 0; i < objQuantity; ++i)
		{
			int objIndex = faceIndexList[i].objIndex;
			int faceIndex = faceIndexList[i].faceIndex;

			int vertexIndex0 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[0];
			int vertexIndex1 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[1];
			int vertexIndex2 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[2];

			RayVector v0 = objData->geoVertexList[vertexIndex0];
			RayVector v1 = objData->geoVertexList[vertexIndex1];
			RayVector v2 = objData->geoVertexList[vertexIndex2];

			/* 三个顶点都位于左孩子中，则一定属于左孩子，一定不属于右孩子 */
			bool result0 = insideBox(leftChild, v0);
			bool result1 = insideBox(leftChild, v1);
			bool result2 = insideBox(leftChild, v2);
			if (result0 && result1 && result2)
			{
				leftChild->faceIndexList.push_back(faceIndexList[i]);
				continue;
			}
			/* 三个顶点都位于右孩子中，则一定属于右孩子，一定不属于左孩子 */
			bool result3 = insideBox(rightChild, v0);
			bool result4 = insideBox(rightChild, v1);
			bool result5 = insideBox(rightChild, v2);
			if (result3 && result4 && result5)
			{
				rightChild->faceIndexList.push_back(faceIndexList[i]);
				continue;
			}

			/* 三个顶点至少有一个位于左孩子中，则一定属于左孩子 */
			if (result0 || result1 || result2)
			{
				leftChild->faceIndexList.push_back(faceIndexList[i]);
			}
			/* 三个顶点均在左孩子外，使用相交算法判断三角形是否与左孩子相交 */
			else if (boxIntersectPoly(leftChild, faceIndexList[i].objIndex, faceIndexList[i].faceIndex))
			{
				leftChild->faceIndexList.push_back(faceIndexList[i]);
			}

			/* 三个顶点至少有一个位于右孩子中，则一定属于右孩子 */
			if (result3 || result4 || result5)
			{
				rightChild->faceIndexList.push_back(faceIndexList[i]);
			}
			/* 三个顶点均在右孩子外，使用相交算法判断三角形是否与右孩子相交 */
			else if (boxIntersectPoly(rightChild, faceIndexList[i].objIndex, faceIndexList[i].faceIndex))
			{
				rightChild->faceIndexList.push_back(faceIndexList[i]);
			}
		}

		objQuantity = sphereIndexList.size();
		for (unsigned int i = 0; i < objQuantity; ++i)
		{
			if (boxIntersectSphere(leftChild, sphereIndexList[i].objIndex, sphereIndexList[i].faceIndex))
				leftChild->sphereIndexList.push_back(sphereIndexList[i]);
			if (boxIntersectSphere(rightChild, sphereIndexList[i].objIndex, sphereIndexList[i].faceIndex))
				rightChild->sphereIndexList.push_back(sphereIndexList[i]);
		}
		leftChild->subdivide(currentDepth + 1, nextAxis);
		rightChild->subdivide(currentDepth + 1, nextAxis);
	}
	else
	{
		leftChild = NULL;
		rightChild = NULL;
		return;
	}
}

/*************** 判断BSP树子空间是否与三角片相交 ***************/
bool RayBSPTree::boxIntersectPoly(RayBSPTree *tree, int objIndex, int faceIndex)
{
	int vertexIndex0 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[0];
	int vertexIndex1 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[1];
	int vertexIndex2 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[2];

	RayVector v0 = objData->geoVertexList[vertexIndex0];
	RayVector v1 = objData->geoVertexList[vertexIndex1];
	RayVector v2 = objData->geoVertexList[vertexIndex2];

	RayVector e1 = v1 - v0;
	RayVector e2 = v2 - v0;
	RayVector d = RayVector(tree->m_fMinX, tree->m_fMinY, tree->m_fMinZ) - v0;

	/* 与 x = m_fMinX 的相交情况 */
	if (fabs(e1.x()) < fabs(e2.x()))				// 保证e1i > e2i
	{
		RayVector temp = e1;
		e1 = e2;
		e2 = temp;
	}
	e1i = e1.x();
	e1j = e1.y();
	e1k = e1.z();
	e2i = e2.x();
	e2j = e2.y();
	e2k = e2.z();
	di = d.x();
	dj = d.y();
	dk = d.z();
	Li = tree->m_fMaxX - tree->m_fMinX;
	Lj = tree->m_fMaxY - tree->m_fMinY;
	Lk = tree->m_fMaxZ - tree->m_fMinZ;
	if (solutionExists())
		return true;

	/* 与 x = m_fMaxX 的相交情况 */
	di += Li;
	if (solutionExists())
		return true;

	/* 与 y = m_fMinY 的相交情况 */
	if (fabs(e1.y()) < fabs(e2.y()))				// 保证e1i > e2i
	{
		RayVector temp = e1;
		e1 = e2;
		e2 = temp;
	}
	e1i = e1.y();
	e1j = e1.z();
	e1k = e1.x();
	e2i = e2.y();
	e2j = e2.z();
	e2k = e2.x();
	di = d.y();
	dj = d.z();
	dk = d.x();
	Li = tree->m_fMaxY - tree->m_fMinY;
	Lj = tree->m_fMaxZ - tree->m_fMinZ;
	Lk = tree->m_fMaxX - tree->m_fMinX;
	if (solutionExists())
		return true;

	/* 与 y = m_fMaxY 的相交情况 */
	di += Li;
	if (solutionExists())
		return true;

	/* 与 z = m_fMinZ 的相交情况 */
	if (fabs(e1.z()) < fabs(e2.z()))				// 保证e1i > e2i
	{
		RayVector temp = e1;
		e1 = e2;
		e2 = temp;
	}
	e1i = e1.z();
	e1j = e1.x();
	e1k = e1.y();
	e2i = e2.z();
	e2j = e2.x();
	e2k = e2.y();
	di = d.z();
	dj = d.x();
	dk = d.y();
	Li = tree->m_fMaxZ - tree->m_fMinZ;
	Lj = tree->m_fMaxX - tree->m_fMinX;
	Lk = tree->m_fMaxY - tree->m_fMinY;
	if (solutionExists())
		return true;

	/* 与 z = m_fMaxZ 的相交情况 */
	di += Li;
	if (solutionExists())
		return true;

	return false;
}

/*************** 判断一特定不等式组是否有解 ***************/
bool RayBSPTree::solutionExists()
{
	double min = 0.0, max = 1.0;
	double tempMin, tempMax;

	/* 求解第一个不等式 */
	if (fabs(e2i) > ZERO)				// e2i != 0
	{
		tempMax = C0();
		if (a0() > 0.0)
		{
			if (tempMax < min)
				return false;
			if (tempMax < max)
				max = tempMax;
		}
		else
		{
			tempMin = tempMax;
			if (tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
		}
	}
	else								// 异常情况1，e2i == 0
	{
		if (di / e1i < 0.0)
			return false;
	}

	/* 求解第二个不等式 */
	if (fabs(deti()) > ZERO)			// deti != 0
	{
		tempMax = Ci() + ai();
		if (ai() > 0.0)
		{
			if (tempMax < min)
				return false;
			if (tempMax < max)
				max = tempMax;
		}
		else
		{
			tempMin = tempMax;
			if (tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
		}
	}
	else								// 异常情况2，deti == 0
	{
		if (e1i > 0.0)
		{
			if (e1i - di < 0.0)
				return false;
		}
		else
		{
			if (e1i - di > 0.0)
				return false;
		}
	}

	/* 求解第三个不等式 */
	if (fabs(detj()) > ZERO)			// detj != 0
	{
		tempMin = Cj();
		tempMax = tempMin + aj() * Lj;
		if (aj() > 0.0)
		{
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
		else
		{
			double temp = tempMin;
			tempMin = tempMax;
			tempMax = temp;
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
	}
	else								// 异常情况3，detj == 0
	{
		if (e1i > 0.0)
		{
			if (dj * e1i - e1j * di > 0.0)
				return false;
			if (dj * e1i - e1j * di + Lj * e1i < 0.0)
				return false;
		}
		else
		{
			if (dj * e1i - e1j * di < 0.0)
				return false;
			if (dj * e1i - e1j * di + Lj * e1i > 0.0)
				return false;
		}
	}

	/* 求解第四个不等式 */
	if (fabs(detk()) > ZERO)			// detk != 0
	{
		tempMin = Ck();
		tempMax = tempMin + ak() * Lk;
		if (ak() > 0.0)
		{
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
		else
		{
			double temp = tempMin;
			tempMin = tempMax;
			tempMax = temp;
			if (tempMax < min || tempMin > max)
				return false;
			if (tempMin > min)
				min = tempMin;
			if (tempMax < max)
				max = tempMax;
		}
	}
	else								// 异常情况4，detk == 0
	{
		if (e1i > 0)
		{
			if (dk * e1i - e1k * di > 0.0)
				return false;
			if (dk * e1i - e1k * di + Lk * e1i < 0.0)
				return false;
		}
		else
		{
			if (dk * e1i - e1k * di < 0.0)
				return false;
			if (dk * e1i - e1k * di + Lk * e1i > 0.0)
				return false;
		}
	}

	return true;
}

/*************** 解不等式组要用到的一些量 ***************/
double RayBSPTree::C0()
{
	return b0() * di;
}

double RayBSPTree::Ci()
{
	return -bi() * di;
}

double RayBSPTree::Cj()
{
	return aj() * dj - bj() * di;
}

double RayBSPTree::Ck()
{
	return ak() * dk - bk() * di;
}

double RayBSPTree::a0()
{
	return e1i / e2i;
}

double RayBSPTree::ai()
{
	return e1i / deti();
}

double RayBSPTree::aj()
{
	return e1i / detj();
}

double RayBSPTree::ak()
{
	return e1i / detk();
}

double RayBSPTree::deti()
{
	return e1i - e2i;
}

double RayBSPTree::detj()
{
	return e1i * e2j - e2i * e1j;
}

double RayBSPTree::detk()
{
	return e1i * e2k - e2i * e1k;
}

double RayBSPTree::b0()
{
	return 1.0 / e2i;
}

double RayBSPTree::bi()
{
	return 1.0 / deti();
}

double RayBSPTree::bj()
{
	return e1j / detj();
}

double RayBSPTree::bk()
{
	return e1k / detk();
}

/*************** 判断点是否在一BSP树子空间内部 ***************/
bool RayBSPTree::insideBox(RayBSPTree *tree, const RayVector &point)
{
	if (point.x() < tree->m_fMinX || point.x() > tree->m_fMaxX)
		return false;
	if (point.y() < tree->m_fMinY || point.y() > tree->m_fMaxY)
		return false;
	if (point.z() < tree->m_fMinZ || point.z() > tree->m_fMaxZ)
		return false;
	return true;
}

/*************** 判断BSP树子空间是否与一球体相交 ***************/
bool RayBSPTree::boxIntersectSphere(RayBSPTree *tree, int objIndex, int sphereIndex)
{
	double distSquared = 0.0;
	double r = objData->objectList[objIndex].sphereList[sphereIndex].r();
	RayVector center = objData->objectList[objIndex].sphereList[sphereIndex].center();

	if (center.x() < tree->m_fMinX)
		distSquared += (center.x() - tree->m_fMinX) * (center.x() - tree->m_fMinX);
	else if (center.x() > tree->m_fMaxX)
		distSquared += (center.x() - tree->m_fMaxX) * (center.x() - tree->m_fMaxX);
	if (center.y() < tree->m_fMinY)
		distSquared += (center.y() - tree->m_fMinY) * (center.y() - tree->m_fMinY);
	else if (center.y() > tree->m_fMaxY)
		distSquared += (center.y() - tree->m_fMaxY) * (center.y() - tree->m_fMaxY);
	if (center.z() < tree->m_fMinZ)
		distSquared += (center.z() - tree->m_fMinZ) * (center.z() - tree->m_fMinZ);
	else if (center.z() > tree->m_fMaxZ)
		distSquared += (center.z() - tree->m_fMaxZ) * (center.z() - tree->m_fMaxZ);

	if (distSquared <= r * r)
		return true;
	else
		return false;
}

/*void RayBSPTree::print(FILE *f, double cc)
{
	fprintf(f, "%.0lf:\n", cc);
	fprintf(f, "minx = %.3lf, maxx = %.3lf\n", m_fMinX, m_fMaxX);
	fprintf(f, "miny = %.3lf, maxy = %.3lf\n", m_fMinY, m_fMaxY);
	fprintf(f, "minz = %.3lf, maxz = %.3lf\n", m_fMinZ, m_fMaxZ);
	fprintf(f, "三角片:\n");
	for (unsigned int i = 0; i < faceIndexList.size(); ++i)
		fprintf(f, "\t%d %d\n", faceIndexList[i].objIndex, faceIndexList[i].faceIndex);
	fprintf(f, "球面:\n");
	for (unsigned int i = 0; i < sphereIndexList.size(); ++i)
		fprintf(f, "\t%d %d\n", sphereIndexList[i].objIndex, sphereIndexList[i].faceIndex);
	fprintf(f, "----------------------------------\n");
	if (leftChild != NULL)
		leftChild->print(f, cc * 10);
	if (rightChild != NULL)
		rightChild->print(f, cc * 10 + 1);
}*/
