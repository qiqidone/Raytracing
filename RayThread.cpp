#include "RayThread.h"
#include <cmath>

const double RayThread::ZERO = 0.0000001;
const double RayThread::NEGA_ZERO = -0.0000001;
const double RayThread::RAY_INFINITY = 10000000;
const double RayThread::RAY_NEGA_INFINITY = -10000000;
const double RayThread::BOX_COEFFICIENT = 1.1;

RayThread::RayThread(RayCommonData *commonData, int num, int stride, RayVector eyePoint,
					 double scrPlane, int maxDepth, bool useBSP, RayBSPTree *bspTree,
					 ObjData *objData, RayColor ia,
					 double minX, double maxX, double minY, double maxY, double minZ, double maxZ)
:
m_pCommonData(commonData),
m_nNum(num),
m_nStride(stride),
eyePoint(eyePoint),
scrPlane(scrPlane),
maxDepth(maxDepth),
m_bUseBSP(useBSP),
bspTree(bspTree),
objData(objData),
ia(ia),
m_fMinX(minX),
m_fMaxX(maxX),
m_fMinY(minY),
m_fMaxY(maxY),
m_fMinZ(minZ),
m_fMaxZ(maxZ),
m_bIsRunning(false)
{
}

void RayThread::run()
{
	m_bIsRunning = true;
	/* 开始光线跟踪 */
	for (int i = m_nNum; i < m_pCommonData->m_nSceneHeight; i += m_nStride)
	{
		for (int j = 0; j < m_pCommonData->m_nSceneWidth; ++j)
		{
			RayPixel &tempPixel = m_pCommonData->scene[i][j];
			RayVector direction((double)j - m_pCommonData->m_nSceneWidth / 2 - eyePoint.x(),
								(double)(m_pCommonData->m_nSceneHeight) / 2 - i - eyePoint.y(),
								-scrPlane * 1.4 / 0.6);
			RayColor color;
			traceRay(eyePoint, direction, 0, color, false);
			color.normalize();
			tempPixel.setR(color.r());
			tempPixel.setG(color.g());
			tempPixel.setB(color.b());
		}
		m_pCommonData->m_nFinishHeight++;
	}
	m_bIsRunning = false;
}

/*************** 进行光线跟踪 *******************/
void RayThread::traceRay(RayVector start, RayVector direction, int depth, RayColor &color, bool isInside)
{
	RayVector interPoint, refDirection, completeRefDirection, transDirection;
	RayColor localColor, refColor, completeRefColor, transColor;

	if (depth > maxDepth)				// 递归层数超过限制
	{
		color.setR(0.0);
		color.setG(0.0);
		color.setB(0.0);
	}
	else
	{
		int objIndex, faceIndex;
		int objIndex1, faceIndex1;
		double T, T1;
		bool result = false, result1 = false, isPoly = false;
		RayColor useless;
		if (!m_bUseBSP)				// 不使用BSP加速
		{
			result = findNearestPoly(start, direction, T, objIndex, faceIndex, true, isInside, useless);
			result1 = findNearestSphere(start, direction, T1, objIndex1, faceIndex1, true, isInside, useless);
			isPoly = true;
			if (result && result1)
			{
				if (T1 < T)
				{
					T = T1;
					isPoly = false;
					objIndex = objIndex1;
					faceIndex = faceIndex1;
				}
			}
			else if (!result && result1)
			{
				T = T1;
				isPoly = false;
				objIndex = objIndex1;
				faceIndex = faceIndex1;
			}
			result = result || result1;
		}
		else						// 使用BSP加速
		{
			double tMin = 0.0, tMax = 0.0;
			/* 与最大包围盒相交才需要计算其内部相交情况，否则直接断定无交点 */
			if (RayIntersectBox(start, direction, tMin, tMax))
			{
				result = findNearestPointBSP(start, direction, T, objIndex, faceIndex,
											 tMin, tMax, bspTree, isPoly, true, useless);
			}
			else
			{
				result = false;
			}
		}

		if (!result)					// 无交点
		{
			color.setR(0.0);
			color.setG(0.0);
			color.setB(0.0);
		}
		else
		{
			RayColor ks, kt, ksTrans(0.0, 0.0, 0.0),  transRefColor(0.0, 0.0, 0.0);
			RayVector N;
			bool isSpecular, isTransparent;
			interPoint = start + direction * T;
			shade(objIndex, faceIndex, interPoint, localColor, isPoly, N, ks, isSpecular, isTransparent);

			if (isSpecular)					// 交点所在的表面为镜面
			{
				direction.normalize();
				refDirection = direction - N * (direction * N) * 2.0;
				traceRay(interPoint, refDirection, depth + 1, refColor, false);
			}
			if (isTransparent)				// 交点所在表面为透明面
			{
				double eta;
				if (isPoly)
				{
					eta = objData->objectList[objIndex].faceList[faceIndex].eta();
					kt = objData->objectList[objIndex].faceList[faceIndex].kt();
				}
				else
				{
					eta = objData->objectList[objIndex].sphereList[faceIndex].eta();
					kt = objData->objectList[objIndex].sphereList[faceIndex].kt();
				}
				double cosTheta1, cosTheta2;
				RayVector L, T;
				L = -direction;
				L.normalize();
				cosTheta1 = N * L;
				if (cosTheta1 > 0.0)		// 光线从空气射向物体
				{
					cosTheta2 = sqrt(1 - (1 - cosTheta1 * cosTheta1) / (eta * eta));
					transDirection = -L / eta - N * (cosTheta2 - cosTheta1 / eta);
					traceRay(interPoint, transDirection, depth + 1, transColor, true);
				}
				else						// 光线从物体射向空气
				{
					cosTheta1 *= -1.0;
					double sinTheta1 = sqrt(1 - cosTheta1 * cosTheta1);
					double sinThetaMax = 1.0 / eta;				// 临界角，入射角大于这个角会发生全反射
					ksTrans.setR(sinTheta1 / sinThetaMax);
					ksTrans.setG(sinTheta1 / sinThetaMax);
					ksTrans.setB(sinTheta1 / sinThetaMax);
					//ksTrans = RayColor(0.5, 0.5, 0.5);
					RayVector transRefDirection = -N * (L * -N) * 2.0 - L;
					traceRay(interPoint, transRefDirection, depth + 1, transRefColor, true);
					if ((1.0 - cosTheta1 * cosTheta1) * eta * eta < 1.0)		// 不发生全反射，此时需要计算折射
					{
						cosTheta2 = sqrt(1.0 - (1.0 - cosTheta1 * cosTheta1) * eta * eta);
						transDirection = -L * eta - -N * (cosTheta2 - cosTheta1 * eta);
						traceRay(interPoint, transDirection, depth + 1, transColor, false);
					}
				}
			}
			color = localColor + ks * refColor + kt * transColor + ksTrans * transRefColor;
		}
	}
}

/******* 计算光线和最大的包围盒的相交情况 *******/
bool RayThread::RayIntersectBox(const RayVector &start, const RayVector &direction, double &tMin, double &tMax)
{
	double t0, t1;
	double minX = m_fMinX * BOX_COEFFICIENT;
	double maxX = m_fMaxX * BOX_COEFFICIENT;
	double minY = m_fMinY * BOX_COEFFICIENT;
	double maxY = m_fMaxY * BOX_COEFFICIENT;
	double minZ = m_fMinZ * BOX_COEFFICIENT;
	double maxZ = m_fMaxZ * BOX_COEFFICIENT;
	/* 首先计算与YZ平面的相交情况 */
	if (fabs(direction.x()) < ZERO)
		if (start.x() < minX || start.x() > maxX)
			return false;
	t0 = (minX - start.x()) / direction.x();
	t1 = (maxX - start.x()) / direction.x();

	if (t0 > t1)
	{
		double temp = t0;
		t0 = t1;
		t1 = temp;
	}

	tMin = t0;
	tMax = t1;

	if (tMin > tMax)
		return false;
	if (tMax < 0.0)
		return false;

	/* 其次计算与XZ平面的相交情况 */
	if (fabs(direction.y()) < ZERO)
		if (start.y() < minY || start.y() > maxY)
			return false;
	t0 = (minY - start.y()) / direction.y();
	t1 = (maxY - start.y()) / direction.y();

	if (t0 > t1)
	{
		double temp = t0;
		t0 = t1;
		t1 = temp;
	}
	
	if (t0 > tMin)
		tMin = t0;
	if (t1 < tMax)
		tMax = t1;

	if (tMin > tMax)
		return false;
	if (tMax < 0.0)
		return false;

	/* 最后计算与XY平面的相交情况 */
	if (fabs(direction.z()) < ZERO)
		if (start.z() < minZ || start.z() > maxZ)
			return false;
	t0 = (minZ - start.z()) / direction.z();
	t1 = (maxZ - start.z()) / direction.z();

	if (t0 > t1)
	{
		double temp = t0;
		t0 = t1;
		t1 = temp;
	}
	
	if (t0 > tMin)
		tMin = t0;
	if (t1 < tMax)
		tMax = t1;

	if (tMin > tMax)
		return false;
	if (tMax < 0.0)
		return false;

	return true;
}

/******* 用局部光照模型计算交点interPoint处的局部光亮度localColor ***********/
void RayThread::shade(const int objIndex, const int faceIndex, RayVector interPoint, RayColor& localColor,
						bool isPoly, RayVector &N, RayColor &ks, bool &isSpecular, bool &isTransparent)
{
	RayColor kd, ka;
	int phongN;

	if (isPoly)			// 交点在一个多边形面片上
	{
		double x, y, z;
		x = y = z = 0.0;

		N.setX(0.0);
		N.setY(0.0);
		N.setZ(0.0);
		for (unsigned int i = 0; i < 3; ++i)
		{
			int normalIndex = objData->objectList[objIndex].faceList[faceIndex].vertexNormalIndex[i];
			N += objData->vertexNormalList[normalIndex];
		}
		int normalIndex0 = objData->objectList[objIndex].faceList[faceIndex].vertexNormalIndex[0];
		int normalIndex1 = objData->objectList[objIndex].faceList[faceIndex].vertexNormalIndex[1];
		int normalIndex2 = objData->objectList[objIndex].faceList[faceIndex].vertexNormalIndex[2];
		/* 如果有两点的法向量编号不同，则插值求出这点的法向量 */
		if (normalIndex0 != normalIndex1 || normalIndex1 != normalIndex2 || normalIndex2 != normalIndex0)
			normalInterpolate(N, interPoint, objIndex, faceIndex);
		kd = objData->objectList[objIndex].faceList[faceIndex].kd();
		ks = objData->objectList[objIndex].faceList[faceIndex].ks();
		ka = objData->objectList[objIndex].faceList[faceIndex].ka();

		phongN = objData->objectList[objIndex].faceList[faceIndex].phongN();
		isSpecular = objData->objectList[objIndex].faceList[faceIndex].specular();
		isTransparent = objData->objectList[objIndex].faceList[faceIndex].transparent();
	}
	else				// 交点在一个球面上
	{
		RayVector center;
		center = objData->objectList[objIndex].sphereList[faceIndex].center();
		N = interPoint - center;

		kd = objData->objectList[objIndex].sphereList[faceIndex].kd();
		ks = objData->objectList[objIndex].sphereList[faceIndex].ks();
		ka = objData->objectList[objIndex].sphereList[faceIndex].ka();

		phongN = objData->objectList[objIndex].sphereList[faceIndex].phongN();
		isSpecular = objData->objectList[objIndex].sphereList[faceIndex].specular();
		isTransparent = objData->objectList[objIndex].sphereList[faceIndex].transparent();
	}
	N.normalize();
	localColor = ka * ia;

	RayVector V, L, H;
	V = eyePoint - interPoint;
	V.normalize();

	unsigned int size = objData->lightList.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		L = objData->lightList[i].position - interPoint;
		L.normalize();
		H = V + L;
		H.normalize();

		double useless;
		bool result0 = false, result1 = false;
		RayColor ktFac0(1.0, 1.0, 1.0), ktFac1(1.0, 1.0, 1.0);
		int uselessObjIndex = objIndex, uselessFaceIndex = faceIndex;
		if (!m_bUseBSP)				// 不使用BSP加速
		{
			result0 = findNearestPoly(interPoint, objData->lightList[i].position - interPoint, useless,
									  uselessObjIndex, uselessFaceIndex, false, false, ktFac0);
			result1 = findNearestSphere(interPoint, objData->lightList[i].position - interPoint, useless,
										uselessObjIndex, uselessFaceIndex, false, false, ktFac1);
			result0 = result0 || result1;
			ktFac0 *= ktFac1;
		}
		else						// 使用BSP加速
		{
			result0 = findNearestPointBSP(interPoint, objData->lightList[i].position - interPoint, useless,
										  uselessObjIndex, uselessFaceIndex, 0.0, 1.0, bspTree, isPoly,
										  false, ktFac0);
		}
		if (!result0)
		{
			double factor0 = N * L;
			double factor1 = N * H;
			if (factor0 < 0.0)
				factor0 = 0.0;
			if (factor1 < 0.0)
				factor1 = 0.0;
			localColor += objData->lightList[i].color * ktFac0 * (kd * factor0 + ks * pow(factor1, phongN));
		}
	}
}

/* 使用交点在三角面片的重心坐标对其法向量进行插值求解 */
void RayThread::normalInterpolate(RayVector &N, const RayVector &interPoint, const int objIndex, const int faceIndex)
{
	int flag = 0;
	double length = fabs(N.x());
	double x, y, x1, y1, x2, y2, x3, y3;

	int vertexIndex1 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[0];
	int vertexIndex2 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[1];
	int vertexIndex3 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[2];

	/* 将这个三角片和交点都投影到与三角片法向量的x,y,z三个分量中绝对值最大的那个分量所垂直的平面上 */
	if (fabs(N.y()) > length)
	{
		length = fabs(N.y());
		flag = 1;
	}
	if (fabs(N.z()) > length)
	{
		flag = 2;
	}
	if (flag == 0)
	{
		x = interPoint.y();
		y = interPoint.z();
		x1 = objData->geoVertexList[vertexIndex1].y();
		y1 = objData->geoVertexList[vertexIndex1].z();
		x2 = objData->geoVertexList[vertexIndex2].y();
		y2 = objData->geoVertexList[vertexIndex2].z();
		x3 = objData->geoVertexList[vertexIndex3].y();
		y3 = objData->geoVertexList[vertexIndex3].z();
	}
	else if (flag == 1)
	{
		x = interPoint.z();
		y = interPoint.x();
		x1 = objData->geoVertexList[vertexIndex1].z();
		y1 = objData->geoVertexList[vertexIndex1].x();
		x2 = objData->geoVertexList[vertexIndex2].z();
		y2 = objData->geoVertexList[vertexIndex2].x();
		x3 = objData->geoVertexList[vertexIndex3].z();
		y3 = objData->geoVertexList[vertexIndex3].x();
	}
	else
	{
		x = interPoint.x();
		y = interPoint.y();
		x1 = objData->geoVertexList[vertexIndex1].x();
		y1 = objData->geoVertexList[vertexIndex1].y();
		x2 = objData->geoVertexList[vertexIndex2].x();
		y2 = objData->geoVertexList[vertexIndex2].y();
		x3 = objData->geoVertexList[vertexIndex3].x();
		y3 = objData->geoVertexList[vertexIndex3].y();
	}
	/* 求出投影后交点的重心坐标 */
	double denominator = x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2;
	double u = (x * y2 + x2 * y3 + x3 * y - x * y3 - x2 * y - x3 * y2) / denominator;
	double v = (x1 * y + x * y3 + x3 * y1 - x1 * y3 - x * y1 - x3 * y) / denominator;
	double w = (x1 * y2 + x2 * y + x * y1 - x1 * y - x2 * y1 - x * y2) / denominator;

	int normalIndex1 = objData->objectList[objIndex].faceList[faceIndex].vertexNormalIndex[0];
	int normalIndex2 = objData->objectList[objIndex].faceList[faceIndex].vertexNormalIndex[1];
	int normalIndex3 = objData->objectList[objIndex].faceList[faceIndex].vertexNormalIndex[2];

	RayVector N1 = objData->vertexNormalList[normalIndex1];
	RayVector N2 = objData->vertexNormalList[normalIndex2];
	RayVector N3 = objData->vertexNormalList[normalIndex3];

	/* 根据重心坐标(u, v, w)插值求出N, N为N1, N2, N3的凸线性组合 */
	N = N1 * u + N2 * v + N3 * w;
}

/*********** 找出射线和所有三角片的最近的交点 ***********/
bool RayThread::findNearestPoly(const RayVector &start, const RayVector &direction, double &T,
								  int &objIndex, int &faceIndex, bool exact, bool isInside, RayColor &ktFac)
{
	double t;
	bool found = false;
	T = 0.0;

	int size = (int)objData->objectList.size();
	for (int i = 0; i < size; ++i)
	{
		int totalFace = objData->objectList[i].faceList.size();
		for (int j = 0; j < totalFace; ++j)
		{
			t = 1.1;
			if (!findInterPointPoly(i, j, start, direction, t))
				continue;
			if (!exact)						// 计算光源和绘制点之间有无遮挡
			{
				if (t > 1.0)
					continue;
				if (!objData->objectList[i].faceList[j].transparent())	// 遮挡物体不透明
				{
					ktFac.setR(0.0);
					ktFac.setG(0.0);
					ktFac.setB(0.0);
					return true;
				}
				else						// 遮挡物体透明
				{
					ktFac *= objData->objectList[i].faceList[j].kt();
					/* 到目前为止已经将此光源所有的光全部遮住了 */
					if (ktFac.r() <= ZERO && ktFac.g() <= ZERO && ktFac.b() <= ZERO)
						return true;
				}
			}
			else							// 要求出最近的交点，而且要得出它的确切值
			{
				if (found && t >= T)
					continue;
				found = true;
				T = t;
				objIndex = i;
				faceIndex = j;
			}	
		}
	}
	return found;
}

/*********** 求出射线和三角片的交点 ***********/
bool RayThread::findInterPointPoly(int objIndex, int faceIndex, const RayVector &start,
									 const RayVector &direction, double &t)
{
	RayVector e1, e2, p, s, q;
	double u, v, temp;

	int vertexIndex0 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[0];
	int vertexIndex1 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[1];
	int vertexIndex2 = objData->objectList[objIndex].faceList[faceIndex].geoVertexIndex[2];

	RayVector v0 = objData->geoVertexList[vertexIndex2];
	RayVector v1 = objData->geoVertexList[vertexIndex0];
	RayVector v2 = objData->geoVertexList[vertexIndex1];

	e1 = v1 - v0;
	e2 = v2 - v0;
	p = direction.cross(e2);
	temp = p * e1;
	if (fabs(temp) < ZERO)
		return false;

	temp = 1.0 / temp;
	s = start - v0;

	u = s * p * temp;
	if (u < 0.0 || u > 1.0)
		return false;

	q = s.cross(e1);
	v = direction * q * temp;

	if (v < 0.0 || v > 1.0)
		return false;

	if (u + v > 1.0)
		return false;

	t = e2 * q * temp;
	if (t < ZERO)
		return false;
	return true;
}

/*********** 找出射线和所有球面的最近的交点 ***********/
bool RayThread::findNearestSphere(const RayVector &start, const RayVector &direction, double &T,
									int &objIndex, int &faceIndex, bool exact, bool isInside, RayColor &ktFac)
{
	double t0, t1;
	bool found = false;

	int objAmount = (int)objData->objectList.size();
	for (int i = 0; i < objAmount; ++i)
	{
		int sphereAmount = (int)objData->objectList[i].sphereList.size();
		for (int j = 0; j < sphereAmount; ++j)
		{
			int rootQuantity = findInterPointSphere(i, j, start, direction, t0, t1);
			if (rootQuantity == 2)				// 有两个大于ZERO的实根
			{
				if (exact)
				{
					if (found && t0 >= T)
						continue;
					found = true;
					T = t0;
					objIndex = i;
					faceIndex = j;
				}
				else							// 只需确定有无交点
				{
					if (t0 >= 1.0)
						continue;
					if (!objData->objectList[i].sphereList[j].transparent())	// 遮挡物体不透明
					{
						ktFac.setR(0.0);
						ktFac.setG(0.0);
						ktFac.setB(0.0);
						return true;
					}
					else						// 遮挡物体透明
					{
						ktFac *= objData->objectList[i].sphereList[j].kt();
						if (ktFac.r() <= ZERO && ktFac.g() <= ZERO && ktFac.b() <= ZERO)
							return true;
					}	
				}
			}
			else if (rootQuantity == 1)			// 有一个大于ZERO的实根（重根也算在这种情况中）
			{
				if (exact)
				{
					if (found && t0 >= T)
						continue;
					found = true;
					T = t0;
					objIndex = i;
					faceIndex = j;
				}
				else
				{
					if (t0 >= 1.0)
						continue;
					if (!objData->objectList[i].sphereList[j].transparent())	// 遮挡物体不透明
					{
						ktFac.setR(0.0);
						ktFac.setG(0.0);
						ktFac.setB(0.0);
						return true;
					}
					else						// 遮挡物体透明
					{
						ktFac *= objData->objectList[i].sphereList[j].kt();
						if (ktFac.r() <= ZERO && ktFac.g() <= ZERO && ktFac.b() <= ZERO)
							return true;
					}	
				}
			}
		}
	}
	return found;
}

/*********** 求出射线和球面的交点 ***********/
int RayThread::findInterPointSphere(int objIndex, int faceIndex, const RayVector &start,
									  const RayVector &direction, double &t0, double &t1)
{
	double a, b, c, r, discrm;
	RayVector sphereCenter, pMinusC;

	r = objData->objectList[objIndex].sphereList[faceIndex].r();
	sphereCenter =objData->objectList[objIndex].sphereList[faceIndex].center();
	pMinusC = start - sphereCenter;
	a = direction * direction;
	b = direction * pMinusC * 2.0;
	c = pMinusC * pMinusC - r * r;
	discrm = b * b - a * c * 4.0;
	if (discrm > 0.0)					// 有两个不同的实根
	{
		t0 = (-b - sqrt(discrm)) / (a * 2.0);
		t1 = (-b + sqrt(discrm)) / (a * 2.0);
		if (t0 >= ZERO && t1 >= ZERO)		// 二者均符合条件
			return 2;
		else if (t0 < ZERO && t1 >= ZERO)	// 只有较大的t1符合条件
		{
			t0 = t1;
			return 1;
		}
		else if (t0 < ZERO && t1 < ZERO)	// 二者均不符合条件
			return 0;
	}
	else if (fabs(discrm) < ZERO)		// 有两个相同实根
	{
		t0 = -b / (a * 2.0);
		if (t0 >= ZERO)
			return 1;
		else
			return 0;
	}
	return 0;
}

/*********** 使用BSP树求交，同时在三角片列表和球面列表中求，找出最近的交点 ***********/
bool RayThread::findNearestPointBSP(const RayVector &start, const RayVector &direction, double &T, int &objIndex,
									  int &faceIndex, double tMin, double tMax, RayBSPTree *BSPNode, bool &isPoly,
									  bool exact, RayColor& ktFac)
{
	bool found = false;
	if (BSPNode->faceIndexList.size() == 0 && BSPNode->sphereIndexList.size() == 0)
	{
		return false;
	}
	if (BSPNode->leftChild == NULL && BSPNode->rightChild == NULL)
	{
		double t0, t1;
		unsigned int size = BSPNode->faceIndexList.size();				// 和三角面片求交
		for (unsigned int i = 0; i < size; ++i)
		{
			int oIndex = BSPNode->faceIndexList[i].objIndex;
			int fIndex = BSPNode->faceIndexList[i].faceIndex;
			if (!findInterPointPoly(oIndex, fIndex, start, direction, t0))
				continue;
			if (!exact)						// 计算光源和绘制点之间有无遮挡
			{
				if (t0 > 1.0)
					continue;
				if (t0 > tMax)
					continue;
				if (!objData->objectList[oIndex].faceList[fIndex].transparent())	// 遮挡物体不透明
				{
					ktFac.setR(0.0);
					ktFac.setG(0.0);
					ktFac.setB(0.0);
					return true;
				}
				else						// 遮挡物体透明
				{
					ktFac *= objData->objectList[oIndex].faceList[fIndex].kt();
					/* 到目前为止已经将此光源所有的光全部遮住了 */
					if (ktFac.r() <= ZERO && ktFac.g() <= ZERO && ktFac.b() <= ZERO)
						return true;
				}
			}
			else
			{
				if (found && t0 >= T)
					continue;
				if (t0 - tMax > ZERO)
				{
					continue;
				}
				found = true;
				T = t0;
				objIndex = oIndex;
				faceIndex = fIndex;
				isPoly = true;
			}
		}
		size = BSPNode->sphereIndexList.size();
		for (unsigned int i = 0; i < size; ++i)				// 和球面求交
		{
			int oIndex = BSPNode->sphereIndexList[i].objIndex;
			int fIndex = BSPNode->sphereIndexList[i].faceIndex;
			int rootQuantity = findInterPointSphere(oIndex, fIndex, start, direction, t0, t1);
			if (rootQuantity == 2)				// 有两个大于ZERO的实根
			{
				if (exact)
				{
					if (found && t0 >= T)
						continue;
					if (t0 > tMax)
						continue;
					found = true;
					T = t0;
					objIndex = oIndex;
					faceIndex = fIndex;
					isPoly = false;
				}
				else							// 只需确定有无交点
				{
					if (t0 > tMax)
						continue;
					if (t0 >= 1.0)
						continue;
					if (!objData->objectList[oIndex].sphereList[fIndex].transparent())	// 遮挡物体不透明
					{
						ktFac.setR(0.0);
						ktFac.setG(0.0);
						ktFac.setB(0.0);
						return true;
					}
					else						// 遮挡物体透明
					{
						ktFac *= objData->objectList[oIndex].sphereList[fIndex].kt();
						if (ktFac.r() <= ZERO && ktFac.g() <= ZERO && ktFac.b() <= ZERO)
							return true;
					}
				}
			}
			else if (rootQuantity == 1)			// 有一个大于ZERO的实根（重根也算在这种情况中）
			{
				if (exact)
				{
					if (found && t0 >= T)
						continue;
					if (t0 > tMax)
						continue;	
					found = true;
					T = t0;
					objIndex = oIndex;
					faceIndex = fIndex;
					isPoly = false;
				}
				else
				{
					if (t0 > tMax)
						continue;
					if (t0 >= 1.0)
						continue;
					if (!objData->objectList[oIndex].sphereList[fIndex].transparent())	// 遮挡物体不透明
					{
						ktFac.setR(0.0);
						ktFac.setG(0.0);
						ktFac.setB(0.0);
						return true;
					}
					else						// 遮挡物体透明
					{
						ktFac *= objData->objectList[oIndex].sphereList[fIndex].kt();
						if (ktFac.r() <= ZERO && ktFac.g() <= ZERO && ktFac.b() <= ZERO)
							return true;
					}
				}
			}
		}
		return found;
	}

	Axis axis = BSPNode->m_nAxis;
	double tDist = 0.0, cutFace = 0.0;
	RayBSPTree *nearNode, *farNode;

	if (axis == X_AXIS)			// 这个空间是以YZ平面作为剖分面的
	{
		cutFace = (BSPNode->m_fMinX + BSPNode->m_fMaxX) * 0.5;
		/* 求出光线与剖分面交点的参数tDist */
		if (fabs(cutFace - start.x()) < ZERO)
			tDist = 0.0;
		else if (direction.x() < ZERO && direction.x() >= 0.0)
			tDist = RAY_INFINITY;
		else if (direction.x() >NEGA_ZERO && direction.x() < 0.0)
			tDist = RAY_NEGA_INFINITY;
		else
			tDist = (cutFace - start.x()) / direction.x();
		if (fabs(cutFace - start.x()) < ZERO)			// 光线起点几乎位于剖分面上
		{
			double tempX = start.x() + direction.x();
			if (tempX < cutFace)
			{
				nearNode = BSPNode->rightChild;
				farNode = BSPNode->leftChild;
			}
			else
			{
				nearNode = BSPNode->leftChild;
				farNode = BSPNode->rightChild;
			}
		}
		else											// 光线起点与剖分面较远
		{
			if (start.x() < cutFace)
			{
				nearNode = BSPNode->leftChild;
				farNode = BSPNode->rightChild;
			}
			else
			{
				nearNode = BSPNode->rightChild;
				farNode = BSPNode->leftChild;
			}
		}
	}
	else if (axis == Y_AXIS)	// 这个空间是以XZ平面作为剖分面的
	{
		cutFace = (BSPNode->m_fMinY + BSPNode->m_fMaxY) * 0.5;
		/* 求出光线与剖分面交点的参数tDist */
		if (fabs(cutFace - start.y()) < ZERO)
			tDist = 0.0;
		else if (direction.y() < ZERO && direction.y() >= 0.0)
			tDist = RAY_INFINITY;
		else if (direction.y() >NEGA_ZERO && direction.y() < 0.0)
			tDist = RAY_NEGA_INFINITY;
		else
			tDist = (cutFace - start.y()) / direction.y();
		if (fabs(cutFace - start.y()) < ZERO)			// 光线起点几乎位于剖分面上
		{
			double tempY = start.y() + direction.y();
			if (tempY < cutFace)
			{
				nearNode = BSPNode->rightChild;
				farNode = BSPNode->leftChild;
			}
			else
			{
				nearNode = BSPNode->leftChild;
				farNode = BSPNode->rightChild;
			}
		}
		else											// 光线起点与剖分面较远
		{
			if (start.y() < cutFace)
			{
				nearNode = BSPNode->leftChild;
				farNode = BSPNode->rightChild;
			}
			else
			{
				nearNode = BSPNode->rightChild;
				farNode = BSPNode->leftChild;
			}
		}
	}
	else						// 这个空间是以XY平面作为剖分面的
	{
		cutFace = (BSPNode->m_fMinZ + BSPNode->m_fMaxZ) * 0.5;
		/* 求出光线与剖分面交点的参数tDist */
		if (fabs(cutFace - start.z()) < ZERO)
			tDist = 0.0;
		else if (direction.z() < ZERO && direction.z() >= 0.0)
			tDist = RAY_INFINITY;
		else if (direction.z() >NEGA_ZERO && direction.z() < 0.0)
			tDist = RAY_NEGA_INFINITY;
		else
			tDist = (cutFace - start.z()) / direction.z();
		if (fabs(cutFace - start.z()) < ZERO)			// 光线起点几乎位于剖分面上
		{
			double tempZ = start.z() + direction.z();
			if (tempZ < cutFace)
			{
				nearNode = BSPNode->rightChild;
				farNode = BSPNode->leftChild;
			}
			else
			{
				nearNode = BSPNode->leftChild;
				farNode = BSPNode->rightChild;
			}
		}
		else											// 光线起点与剖分面较远
		{
			if (start.z() < cutFace)
			{
				nearNode = BSPNode->leftChild;
				farNode = BSPNode->rightChild;
			}
			else
			{
				nearNode = BSPNode->rightChild;
				farNode = BSPNode->leftChild;
			}
		}
	}
	if (tDist > tMax || tDist < 0.0)
	{
		found = findNearestPointBSP(start, direction, T, objIndex, faceIndex,
									tMin, tMax, nearNode, isPoly, exact, ktFac);
	}
	else if (tDist < tMin)
	{
		found = findNearestPointBSP(start, direction, T, objIndex, faceIndex,
									tMin, tMax, farNode, isPoly, exact, ktFac);
	}
	else
	{
		found = findNearestPointBSP(start, direction, T, objIndex, faceIndex,
									tMin, tDist, nearNode, isPoly, exact, ktFac);
		if (found && T <= tMax && T >= tMin)
			return found;
		found = findNearestPointBSP(start, direction, T, objIndex, faceIndex,
									tDist, tMax, farNode, isPoly, exact, ktFac);
	}
	return found;
}