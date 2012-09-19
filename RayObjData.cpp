#include "RayObjData.h"
#include <string.h>
#include <stdio.h>

/**************** Sphere类的成员函数 **************/

Sphere::Sphere(const RayVector center, const double r,
			   const RayColor &kd, const RayColor &ks, const RayColor &ka, const RayColor &kt,
			   const double eta, const int phongN, const bool specular, const bool transparent)
:m_center(center), m_kd(kd), m_ks(ks), m_ka(ka), m_kt(kt)
{
	m_fR = r;
	m_fEta = eta;
	m_nPhongN = phongN;
	m_bSpecular = specular;
	m_bTransparent = transparent;
}

Sphere::Sphere(Sphere const & sphere)
:m_center(sphere.center()), m_kd(sphere.kd()),
m_ks(sphere.ks()), m_ka(sphere.ka()), m_kt(sphere.kt())
{
	m_fR = sphere.m_fR;
	m_fEta = sphere.m_fEta;
	m_nPhongN = sphere.m_nPhongN;
	m_bSpecular = sphere.m_bSpecular;
	m_bTransparent = sphere.m_bTransparent;
}

const RayVector Sphere::center() const
{
	return m_center;
}

double Sphere::r() const
{
	return m_fR;
}

void Sphere::setCenter(const RayVector &center)
{
	m_center = center;
}

void Sphere::setR(const double r)
{
	m_fR = r;
}

const RayColor Sphere::kd() const
{
	return m_kd;
}

const RayColor Sphere::ks() const
{
	return m_ks;
}

const RayColor Sphere::ka() const
{
	return m_ka;
}

const RayColor Sphere::kt() const
{
	return m_kt;
}

double Sphere::eta() const
{
	return m_fEta;
}

int Sphere::phongN() const
{
	return m_nPhongN;
}

bool Sphere::specular() const
{
	return m_bSpecular;
}

bool Sphere::transparent() const
{
	return m_bTransparent;
}


/**************** Face类的成员函数 **************/
Face::Face(const RayColor &kd, const RayColor &ks, const RayColor &ka, const RayColor &kt,
		   const double eta, const int phongN, const bool specular, const bool transparent)
:m_kd(kd), m_ks(ks), m_ka(ka), m_kt(kt)
{
	m_eVertexCase = V_NO_NO;
	m_fEta = eta;
	m_nPhongN = phongN;
	m_bSpecular = specular;
	m_bTransparent = transparent;
}

Face::Face(const Face& face)
:m_kd(face.kd()), m_ks(face.ks()), m_ka(face.ka()), m_kt(face.kt())
{
	m_eVertexCase = face.m_eVertexCase;
	for (int i = 0; i < 3; ++i)
	{
		geoVertexIndex[i] = face.geoVertexIndex[i];
		textureVertexIndex[i] = face.textureVertexIndex[i];
		vertexNormalIndex[i] = face.vertexNormalIndex[i];
	}
	m_fEta = face.m_fEta;
	m_nPhongN = face.m_nPhongN;
	m_bSpecular = face.m_bSpecular;
	m_bTransparent = face.m_bTransparent;
}

const RayColor Face::kd() const
{
	return m_kd;
}

const RayColor Face::ks() const
{
	return m_ks;
}

const RayColor Face::ka() const
{
	return m_ka;
}

const RayColor Face::kt() const
{
	return m_kt;
}

double Face::eta() const
{
	return m_fEta;
}

int Face::phongN() const
{
	return m_nPhongN;
}

bool Face::specular() const
{
	return m_bSpecular;
}

bool Face::transparent() const
{
	return m_bTransparent;
}


/**************** Object类的成员函数 **************/
Object::Object()
{
	/* 保留适当空间，防止多次分配空间，提高性能 */
	faceList.reserve(MAXFACE);
	sphereList.reserve(MAXFACE);
}

Object::Object(const Object& object)
{
	/* 保留适当空间，防止多次分配空间，提高性能 */
	faceList.reserve(MAXFACE);
	sphereList.reserve(MAXFACE);
	faceList = object.faceList;
	sphereList = object.sphereList;
}

/**************** ReadObj类的成员函数 **************/
ObjData::ObjData()
:m_ia(0.5, 0.5, 0.5), m_kd(0.5, 0.5, 0.5), m_ks(0.5, 0.5, 0.5),
m_ka(0.5, 0.5, 0.5), m_kt(0.5, 0.5, 0.5)
{
	/* 保留适当空间，防止多次分配空间，提高性能 */
	geoVertexList.reserve(MAXVERTEX);
	textureVertexList.reserve(MAXVERTEX);
	vertexNormalList.reserve(MAXVERTEX);
	lightList.reserve(MAXLIGHT);
	objectList.reserve(MAXOBJECT);
	m_nMaxDepth = 3;
	m_fEta = 0.0;
	m_nPhongN = 30;
	m_bSpecular = false;
	m_bTransparent = false;
}

void ObjData::initialize()			// 初始化相关参数
{
	geoVertexList.clear();
	textureVertexList.clear();
	vertexNormalList.clear();
	lightList.clear();
	objectList.clear();
	m_ia = RayColor(0.5, 0.5, 0.5);
	m_kd = RayColor(0.5, 0.5, 0.5);
	m_ks = RayColor(0.5, 0.5, 0.5);
	m_ka = RayColor(0.5, 0.5, 0.5);
	m_kt = RayColor(0.5, 0.5, 0.5);
	m_nMaxDepth = 3;
	m_fEta = 0.0;
	m_nPhongN = 30;
	m_bSpecular = false;
	m_bTransparent = false;
}

int ObjData::maxDepth() const
{
	return m_nMaxDepth;
}

double ObjData::eyeX() const
{
	return m_fEyeX;
}

double ObjData::eyeY() const
{
	return m_fEyeY;
}

const RayColor ObjData::ia() const
{
	return m_ia;
}

int ObjData::getVertexNum()						// 返回顶点数量
{
	return geoVertexList.size();
}

int ObjData::getTextureNum()					// 返回纹理数量
{
	return textureVertexList.size();
}

int ObjData::getNormalNum()						// 返回法向数量
{
	return vertexNormalList.size();
}

int ObjData::getFaceNum()						// 返回面数量
{
	int total = 0;
	for (unsigned int i = 0; i < objectList.size(); ++i)
	{
		total += objectList[i].faceList.size();
		total += objectList[i].sphereList.size();
	}
	return total;
}

int ObjData::getObjNum()						// 返回物体数量
{
	return objectList.size();
}

void ObjData::parseVertex(const char *str)		// 解析顶点坐标
{
	char useless[1024];
	double x, y, z;

	sscanf(str, "%s%lf%lf%lf", useless, &x, &y, &z);
	RayVector v(x, y, z);
	geoVertexList.push_back(v);
}

void ObjData::parseTexture(const char *str)		// 解析纹理坐标
{
	char useless[1024];
	double u, v, w;

	sscanf(str, "%s%lf%lf%lf", useless, &u, &v, &w);
	RayVector vt(u, v, w);
	textureVertexList.push_back(vt);
}

void ObjData::parseNormal(const char *str)		// 解析法向坐标
{
	char useless[1024];
	double i, j, k;

	sscanf(str, "%s%lf%lf%lf", useless, &i, &j, &k);
	RayVector vn(i, j, k);
	vertexNormalList.push_back(vn);
}

void ObjData::parseLight(const char *str)		// 解析点光源
{
	char useless[1024];
	double x, y, z, r, g, b;

	sscanf(str, "%s%lf%lf%lf%lf%lf%lf", useless, &x, &y, &z, &r, &g, &b);
	RaySpotLight light(x, y, z, r, g, b);
	lightList.push_back(light);
}

void ObjData::parseMaxDepth(const char *str)
{
	char useless[1024];

	sscanf(str, "%s%d", useless, &m_nMaxDepth);
}

void ObjData::parseEye(const char *str)			// 解析视点
{
	char useless[1024];

	sscanf(str, "%s%lf%lf", useless, &m_fEyeX, &m_fEyeY);
}

void ObjData::parseIa(const char *str)			// 解析环境光
{
	char useless[1024];
	double r, g, b;

	sscanf(str, "%s%lf%lf%lf", useless, &r, &g, &b);
	m_ia.setR(r);
	m_ia.setG(g);
	m_ia.setB(b);
}

void ObjData::parseKd(const char *str)			// 解析漫反射系数
{
	char useless[1024];
	double r, g, b;

	sscanf(str, "%s%lf%lf%lf", useless, &r, &g, &b);
	m_kd.setR(r);
	m_kd.setG(g);
	m_kd.setB(b);
}

void ObjData::parseKs(const char *str)			// 解析镜面反射系数
{
	char useless[1024];
	double r, g, b;

	sscanf(str, "%s%lf%lf%lf", useless, &r, &g, &b);
	m_ks.setR(r);
	m_ks.setG(g);
	m_ks.setB(b);
}

void ObjData::parseKa(const char *str)			// 解析环境反射系数
{
	char useless[1024];
	double r, g, b;

	sscanf(str, "%s%lf%lf%lf", useless, &r, &g, &b);
	m_ka.setR(r);
	m_ka.setG(g);
	m_ka.setB(b);
}

void ObjData::parseKt(const char *str)			// 解析透射系数
{
	char useless[1024];
	double r, g, b;

	sscanf(str, "%s%lf%lf%lf%lf", useless, &r, &g, &b, &m_fEta);
	m_bTransparent = true;
	m_kt.setR(r);
	m_kt.setG(g);
	m_kt.setB(b);
}

void ObjData::parsePhongN(const char *str)		// 解析镜面高光系数
{
	char useless[1024];

	sscanf(str, "%s%d", useless, &m_nPhongN);
}

void ObjData::parseSphere(const char *str, int objectIndex)		// 解析球面信息
{
	char useless[1024];
	double x, y, z, r;

	sscanf(str, "%s%lf%lf%lf%lf", useless, &x, &y, &z, &r);
	Sphere sphere(RayVector(x, y, z), r,
				  m_kd, m_ks, m_ka, m_kt,
				  m_fEta, m_nPhongN,
				  m_bSpecular, m_bTransparent);
	objectList[objectIndex].sphereList.push_back(sphere);
}

void ObjData::parseFace(char *str, int objectIndex)				// 解析面信息
{
	VertexCase vertexCase = V_NO_NO;
	for (unsigned int i = 0; i < strlen(str); ++i)
	{
		if (str[i] == '/')
		{
			if (str[i + 1] == '/')
			{
				vertexCase = V_NO_VN;			// 两个'/'连续，说明是形如"3//4"的顶点表达
			}
			else
			{
				vertexCase = V_VT_VN;			// 两个'/'不连续，说明是形如"1/2/3"的顶点表达
			}
			break;
		}
	}
	
	for (unsigned int i = 0; i < strlen(str); ++i)
	{
		if (str[i] == 'f')						// 将前面的'f'改成空格，方便后续操作
		{
			str[i] = ' ';
			break;
		}
	}
	int vIndex, vtIndex, vnIndex;
	vIndex = 0;
	vtIndex = 0;
	vnIndex = 0;
	char *cPtr = str;							// 用这个指针遍历字符串

	int firstVIndex = 0, secondVIndex = 0, lastVIndex = 0;
	int firstVNIndex = 0, secondVNIndex = 0, lastVNIndex = 0;
	int firstVTIndex = 0, secondVTIndex = 0, lastVTIndex = 0;
	switch (vertexCase)
	{
		case V_NO_NO:							// 顶点信息中不包含纹理坐标和法向坐标
			for (int i = 0; sscanf(cPtr, "%d", &vIndex) != EOF; ++i)		// 将这个字符串中的顶点索引以整数方式读出
			{
				if (vIndex > 0)								// 索引为正
					vIndex--;								// 索引从1开始，数组从零开始，需要调整一下
				else
					vIndex += geoVertexList.size();			// 负数索引需要往上数相应的行
				if (i == 0)
				{
					firstVIndex = vIndex;
				}
				else if (i == 1)
				{
					secondVIndex = vIndex;
				}
				else if (i == 2)				// 第一个面片形成（至少有一个）	
				{
					Face face(m_kd, m_ks, m_ka, m_kt, m_fEta,
							  m_nPhongN, m_bSpecular, m_bTransparent);
					face.m_eVertexCase = vertexCase;
					lastVIndex = vIndex;
					face.geoVertexIndex[0] = firstVIndex;
					face.geoVertexIndex[1] = secondVIndex;
					face.geoVertexIndex[2] = lastVIndex;
					objectList[objectIndex].faceList.push_back(face);	
				}
				else							// 这种情况下每读进一个顶点都会形成一个新面片
				{
					Face face(m_kd, m_ks, m_ka, m_kt, m_fEta,
							  m_nPhongN, m_bSpecular, m_bTransparent);
					face.m_eVertexCase = vertexCase;
					face.geoVertexIndex[0] = firstVIndex;
					face.geoVertexIndex[1] = lastVIndex;
					face.geoVertexIndex[2] = vIndex;
					lastVIndex = vIndex;
					objectList[objectIndex].faceList.push_back(face);
				}
				while(*cPtr != '-' && (*cPtr > '9' ||  *cPtr < '0'))		// 指针不断前移，直到读到数字为止
				{
					cPtr++;
				}
				while(*cPtr != ' ' && *cPtr != '\t'  && *cPtr != '\n')		// 指针不断前移，直到读到空白字符为止
				{
					cPtr++;
				}
			}
			break;
		case V_NO_VN:							// 顶点信息中不包含纹理坐标，包含法向坐标
			for (int i = 0; sscanf(cPtr, "%d//%d", &vIndex, &vnIndex) != EOF; ++i)
			{
				if (vIndex > 0)
					vIndex--;
				else
					vIndex += geoVertexList.size();
				if (vnIndex > 0)
					vnIndex--;
				else
					vnIndex += vertexNormalList.size();
				if (i == 0)
				{
					firstVIndex = vIndex;
					firstVNIndex = vnIndex;
				}
				else if (i == 1)
				{
					secondVIndex = vIndex;
					secondVNIndex = vnIndex;
				}
				else if (i == 2)				// 第一个面片形成（至少有一个）
				{
					Face face(m_kd, m_ks, m_ka, m_kt, m_fEta,
							  m_nPhongN, m_bSpecular, m_bTransparent);
					face.m_eVertexCase = vertexCase;
					lastVIndex = vIndex;
					lastVNIndex = vnIndex;
					face.geoVertexIndex[0] = firstVIndex;
					face.geoVertexIndex[1] = secondVIndex;
					face.geoVertexIndex[2] = lastVIndex;
					face.vertexNormalIndex[0] = firstVNIndex;
					face.vertexNormalIndex[1] = secondVNIndex;
					face.vertexNormalIndex[2] = lastVNIndex;
					objectList[objectIndex].faceList.push_back(face);
				}
				else						// 这种情况下每读进一个顶点都会形成一个新面片
				{
					Face face(m_kd, m_ks, m_ka, m_kt, m_fEta,
							  m_nPhongN, m_bSpecular, m_bTransparent);
					face.m_eVertexCase = vertexCase;
					face.geoVertexIndex[0] = firstVIndex;
					face.geoVertexIndex[1] = lastVIndex;
					face.geoVertexIndex[2] = vIndex;
					lastVIndex = vIndex;
					face.vertexNormalIndex[0] = firstVNIndex;
					face.vertexNormalIndex[1] = lastVNIndex;
					face.vertexNormalIndex[2] = vnIndex;
					lastVNIndex = vnIndex;
					objectList[objectIndex].faceList.push_back(face);
				}
				while(*cPtr != '/')
				{
					cPtr++;
				}
				while(*cPtr != ' ' && *cPtr != '\t'  && *cPtr != '\n')
				{
					cPtr++;
				}
			}
			break;		
		case V_VT_VN:							// 顶点信息中包含纹理坐标和法向坐标
			for (int i = 0; sscanf(cPtr, "%d/%d/%d", &vIndex, &vtIndex, &vnIndex) != EOF; ++i)
			{
				if (vIndex > 0)
					vIndex--;
				else
					vIndex += geoVertexList.size();
				if (vtIndex > 0)
					vtIndex--;
				else
					vtIndex += textureVertexList.size();
				if (vnIndex > 0)
					vnIndex--;
				else
					vnIndex += vertexNormalList.size();
				if (i == 0)
				{
					firstVIndex = vIndex;
					firstVTIndex = vtIndex;
					firstVNIndex = vnIndex;
				}
				else if (i == 1)
				{
					secondVIndex = vIndex;
					secondVTIndex = vtIndex;
					secondVNIndex = vnIndex;
				}
				else if (i == 2)				// 第一个面片形成（至少有一个）
				{
					Face face(m_kd, m_ks, m_ka, m_kt, m_fEta,
							  m_nPhongN, m_bSpecular, m_bTransparent);
					face.m_eVertexCase = vertexCase;
					lastVIndex = vIndex;
					lastVTIndex = vtIndex;
					lastVNIndex = vnIndex;
					face.geoVertexIndex[0] = firstVIndex;
					face.geoVertexIndex[1] = secondVIndex;
					face.geoVertexIndex[2] = lastVIndex;
					face.textureVertexIndex[0] = firstVTIndex;
					face.textureVertexIndex[1] = secondVTIndex;
					face.textureVertexIndex[2] = lastVTIndex;
					face.vertexNormalIndex[0] = firstVNIndex;
					face.vertexNormalIndex[1] = secondVNIndex;
					face.vertexNormalIndex[2] = lastVNIndex;
					objectList[objectIndex].faceList.push_back(face);
				}
				else						// 这种情况下每读进一个顶点都会形成一个新面片
				{
					Face face(m_kd, m_ks, m_ka, m_kt, m_fEta,
							  m_nPhongN, m_bSpecular, m_bTransparent);
					face.m_eVertexCase = vertexCase;
					face.geoVertexIndex[0] = firstVIndex;
					face.geoVertexIndex[1] = lastVIndex;
					face.geoVertexIndex[2] = vIndex;
					lastVIndex = vIndex;
					face.textureVertexIndex[0] = firstVTIndex;
					face.textureVertexIndex[1] = lastVTIndex;
					face.textureVertexIndex[2] = vtIndex;
					lastVTIndex = vtIndex;
					face.vertexNormalIndex[0] = firstVNIndex;
					face.vertexNormalIndex[1] = lastVNIndex;
					face.vertexNormalIndex[2] = vnIndex;
					lastVNIndex = vnIndex;
					objectList[objectIndex].faceList.push_back(face);
				}
				while(*cPtr != '/')
				{
					cPtr++;
				}
				while(*cPtr != ' ' && *cPtr != '\t'  && *cPtr != '\n')
				{
					cPtr++;
				}
			}
			break;
		default:
			break;
	}
}

void ObjData::readObj(const char *fileName)					// 读取obj文件中的数据
{
	char tempString[1024], tempString2[10];
	m_pFile = fopen(fileName, "r");
	int objectIndex = -1;

	while(fgets(tempString, 1000, m_pFile) != NULL)			// 逐行读取
	{
		sscanf(tempString, "%s", tempString2);				// 将每行中最前面的一个字符串提取到tempString2中
		if (strcmp(tempString2, "v") == 0)					// 这一行是顶点坐标
		{
			parseVertex(tempString);
		}
		else if (strcmp(tempString2, "vt") == 0)			// 这一行是纹理坐标
		{
			parseTexture(tempString);
		}
		else if (strcmp(tempString2, "vn") == 0)			// 这一行是法向坐标
		{
			parseNormal(tempString);
		}
		else if (strcmp(tempString2, "light") == 0)			// 这是一个点光源
		{
			parseLight(tempString);
		}
		else if (strcmp(tempString2, "depth") == 0)			// 定义了最大跟踪深度
		{
			parseMaxDepth(tempString);
		}
		else if (strcmp(tempString2, "eye") == 0)			// 视点坐标
		{
			parseEye(tempString);
		}
		else if (strcmp(tempString2, "ia") == 0)			// 环境光
		{
			parseIa(tempString);
		}
		else if (strcmp(tempString2, "kd") == 0)			// 漫反射系数
		{
			parseKd(tempString);
		}
		else if (strcmp(tempString2, "ks") == 0)			// 镜面反射系数
		{
			parseKs(tempString);
		}
		else if (strcmp(tempString2, "ka") == 0)			// 环境反射系数
		{
			parseKa(tempString);
		}
		else if (strcmp(tempString2, "kt") == 0)			// 透射系数
		{
			parseKt(tempString);
		}
		else if (strcmp(tempString2, "!kt") == 0)			// 取消透明属性
		{
			m_bTransparent = false;
		}
		else if (strcmp(tempString2, "pn") == 0)			// 镜面高光系数
		{
			parsePhongN(tempString);
		}
		else if (strcmp(tempString2, "spe") == 0)			// 设置为镜面属性
		{
			m_bSpecular = true;
		}
		else if (strcmp(tempString2, "!spe") == 0)			// 取消镜面属性
		{
			m_bSpecular = false;
		}
		else if (strcmp(tempString2, "f") == 0)				// 平面信息
		{
			parseFace(tempString, objectIndex);
		}
		else if (strcmp(tempString2, "fs") == 0)			// 球面信息
		{
			parseSphere(tempString, objectIndex);
		}
		else if (strcmp(tempString2, "o") == 0)				// 新物体
		{
			++objectIndex;
			Object obj;
			objectList.push_back(obj);
		}
		for (int i = 0; i < 10; ++i)						// 清空tempString2，防止影响下一次的判断
		{
			tempString2[i] = 0;
		}
	}
	fclose(m_pFile);
}
