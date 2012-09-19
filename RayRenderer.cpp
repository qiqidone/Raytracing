#include "RayRenderer.h"
#include <math.h>

using namespace std;

const double RayRenderer::BOX_COEFFICIENT = 1.1;

RayRenderer::RayRenderer(RayCommonData *commonData)
{
	objData = new ObjData();
	m_pCommonData = commonData;
	bspTree = NULL;
}

RayRenderer::~RayRenderer()
{
	if (bspTree != NULL)
		delete bspTree;
}

void RayRenderer::readObj(const char *fileName)			// 调用相应函数读取obj文件
{
	objData->initialize();
	objData->readObj(fileName);

	m_fMinX = m_fMaxX = m_fMinY = m_fMaxY = m_fMinZ = m_fMaxZ = 0.0;
	bool begin = false;
	/* 记录原始坐标中x,y,z的最大、最小值 */
	unsigned int totalVertex = objData->geoVertexList.size();
	for (unsigned int i = 0; i < totalVertex; ++i)
	{
		double x = objData->geoVertexList[i].x();
		double y = objData->geoVertexList[i].y();
		double z = objData->geoVertexList[i].z();
		if (i == 0)
		{
			begin = true;
			m_fMinX = m_fMaxX = x;
			m_fMinY = m_fMaxY = y;
			m_fMinZ = m_fMaxZ = z;
		}
		if (x < m_fMinX)
			m_fMinX = x;
		if (x > m_fMaxX)
			m_fMaxX = x;
		if (y < m_fMinY)
			m_fMinY = y;
		if (y > m_fMaxY)
			m_fMaxY = y;
		if (z < m_fMinZ)
			m_fMinZ = z;
		if (z > m_fMaxZ)
			m_fMaxZ = z;
	}
	unsigned int objQuantity = objData->objectList.size();
	for (unsigned int i = 0; i < objQuantity; ++i)
	{
		unsigned int sphereQuantity = objData->objectList[i].sphereList.size();
		for (unsigned int j = 0; j < sphereQuantity; ++j)
		{
			double r = objData->objectList[i].sphereList[j].r();
			RayVector center = objData->objectList[i].sphereList[j].center();
			if (!begin)
			{
				begin = true;
				m_fMinX = center.x() - r;
				m_fMaxX = center.x() + r;
				m_fMinY = center.y() - r;
				m_fMaxX = center.y() + r;
				m_fMinZ = center.z() - r;
				m_fMaxZ = center.z() + r;
			}
			else
			{
				if ((center.x() - r) < m_fMinX)
					m_fMinX = center.x() - r;
				if ((center.x() + r) > m_fMaxX)
					m_fMaxX = center.x() + r;
				if ((center.y() - r) < m_fMinY)
					m_fMinY = center.y() - r;
				if ((center.y() - r) > m_fMaxY)
					m_fMaxX = center.y() + r;
				if ((center.z() - r) < m_fMinZ)
					m_fMinZ = center.z() - r;
				if ((center.z() + r) > m_fMaxZ)
					m_fMaxZ = center.z() + r;
			}
		}
	}
	double deltaX = m_fMaxX - m_fMinX;
	double deltaY = m_fMaxY - m_fMinY;
	double nx = (double)(m_pCommonData->m_nSceneWidth - 20) / deltaX;
	double ny = (double)(m_pCommonData->m_nSceneHeight - 20) / deltaY;
	if (nx <= ny)
		n = nx;
	else
		n = ny;
	bx = -m_fMinX * n;
	by = -m_fMinY * n;
	bz = -m_fMinZ * n;

	m_fMaxX = n * m_fMaxX + bx;
	m_fMinX = n * m_fMinX + bx;
	double delta = (m_fMinX - m_fMaxX) / 2;
	bx += delta;

	m_fMinX += delta;
	m_fMaxX += delta;

	m_fMaxY = n * m_fMaxY + by;
	m_fMinY = n * m_fMinY + by;
	delta = (m_fMinY - m_fMaxY) / 2;
	by += delta;

	m_fMinY += delta;
	m_fMaxY += delta;

	m_fMaxZ = n * m_fMaxZ + bz;
	m_fMinZ = n * m_fMinZ + bz;
	delta = (m_fMinZ - m_fMaxZ) / 2;
	bz += delta;

	scrPlane = (m_fMaxZ + delta) * 0.6 / 0.5;

	m_fMinZ += delta;
	m_fMaxZ += delta;

	/* 将顶点调整到适合初始窗口的大小 */
	for (unsigned int i = 0; i < objData->geoVertexList.size(); ++i)
	{
		double x = objData->geoVertexList[i].x();
		double y = objData->geoVertexList[i].y();
		double z = objData->geoVertexList[i].z();

		x = n * x + bx;
		y = n * y + by;
		z = n * z + bz;

		objData->geoVertexList[i].setX(x);
		objData->geoVertexList[i].setY(y);
		objData->geoVertexList[i].setZ(z);
	}

	/* 将点光源位置调整到适合初始窗口的大小 */
	for (unsigned int i = 0; i < objData->lightList.size(); ++i)
	{
		double x = objData->lightList[i].position.x();
		double y = objData->lightList[i].position.y();
		double z = objData->lightList[i].position.z();

		x = n * x + bx;
		y = n * y + by;
		z = n * z + bz;
		
		objData->lightList[i].position.setX(x);
		objData->lightList[i].position.setY(y);
		objData->lightList[i].position.setZ(z);
	}

	/* 将球面调整到适合初始窗口的大小 */
	for (unsigned int i = 0; i < objData->objectList.size(); ++i)
	{
		for (unsigned int j = 0; j < objData->objectList[i].sphereList.size(); ++j)
		{
			double x = objData->objectList[i].sphereList[j].center().x();
			double y = objData->objectList[i].sphereList[j].center().y();
			double z = objData->objectList[i].sphereList[j].center().z();
			double r = objData->objectList[i].sphereList[j].r();
			x = n * x + bx;
			y = n * y + by;
			z = n * z + bz;
			r = n * r;
			RayVector tempVector(x, y, z);
			objData->objectList[i].sphereList[j].setCenter(tempVector);
			objData->objectList[i].sphereList[j].setR(r);
		}
	}
	/* 得到最大跟踪深度 */
	maxDepth = objData->maxDepth();

	/* 将视点调整到适合初始窗口的大小 */
	double x = objData->eyeX();
	double y = objData->eyeY();

	x = n * x + bx;
	y = n * y + by;

	eyePoint = RayVector(x, y, scrPlane * 2.0 / 0.6);

	/* 得到环境光 */
	ia = objData->ia();

	qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void RayRenderer::renderScene(bool useBSP)		// 计算场景中每一点的颜色值并存储到缓存中
{
	m_bUseBSP = useBSP;
	//m_pCommonData->scene.clear();
	if (bspTree != NULL)
	{
		delete bspTree;
		bspTree = NULL;
	}
	if (m_bUseBSP)								// 如果要用BSP加速，先构建此场景的BSP树
	{
		bspTree = new RayBSPTree(m_fMinX * BOX_COEFFICIENT, m_fMaxX * BOX_COEFFICIENT,
								 m_fMinY * BOX_COEFFICIENT, m_fMaxY * BOX_COEFFICIENT,
								 m_fMinZ * BOX_COEFFICIENT, m_fMaxZ * BOX_COEFFICIENT, objData);
		unsigned int objQuantity = objData->objectList.size();
		for (unsigned int i = 0; i < objQuantity; ++i)
		{
			unsigned int faceQuantity = objData->objectList[i].faceList.size();
			for (unsigned int j = 0; j < faceQuantity; ++j)
			{
				ObjFaceIndex objFaceIndex(i, j);
				bspTree->faceIndexList.push_back(objFaceIndex);
			}
			unsigned int sphereQuantity = objData->objectList[i].sphereList.size();
			for (unsigned int j = 0; j < sphereQuantity; ++j)
			{
				ObjFaceIndex objFaceIndex(i, j);
				bspTree->sphereIndexList.push_back(objFaceIndex);
			}
		}
		bspTree->subdivide(0, X_AXIS);
		//bspTree->print(f, 1.0);
	}

	/* 开始光线跟踪 */
	for (int i = 0; i < m_pCommonData->m_nThreadNum; ++i)
	{
		RayThread *pThread = new RayThread(m_pCommonData, i, m_pCommonData->m_nThreadNum, eyePoint,
						 scrPlane, maxDepth, m_bUseBSP, bspTree,
						 objData, ia,
						 m_fMinX, m_fMaxX, m_fMinY, m_fMaxY, m_fMinZ, m_fMaxZ);
		threadList.push_back(pThread);
	}

	for (vector<RayThread *>::size_type i = 0; i < threadList.size(); ++i)
		threadList[i]->start();

	bool allFinished = false;
	while(!allFinished)
	{
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
		allFinished = true;
		for (vector<RayThread *>::size_type i = 0; i < threadList.size(); ++i)
		{
			if (threadList[i]->isRunning())
			{
				allFinished = false;
			}
		}
	}

	for (vector<RayThread *>::size_type i = 0; i < threadList.size(); ++i)
		delete threadList[i];
	threadList.clear();
}


int RayRenderer::getVertexNum()			// 调用相应函数，返回ObjData类中的顶点数量
{
	return objData->getVertexNum();
}

int RayRenderer::getTextureNum()		// 调用相应函数，返回ObjData类中的纹理数量
{
	return objData->getTextureNum();
}

int RayRenderer::getNormalNum()			// 调用相应函数，返回ObjData类中的法向数量
{
	return objData->getNormalNum();
}

int RayRenderer::getFaceNum()			// 调用相应函数，返回ObjData类中的面数量
{
	return objData->getFaceNum();
}

int RayRenderer::getObjNum()			// 调用相应函数，返回ObjData类中的物体数量
{
	return objData->getObjNum();
}
