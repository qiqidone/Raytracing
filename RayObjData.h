/*
 * 定义了读取和存储obj文件的相关类
 */

#ifndef __RAYOBJDATA_H__
#define __RAYOBJDATA_H__

#include <vector>
#include <cstdlib>
#include <cstdio>
#include "RayVectorColor.h"

class Sphere
{
	RayVector m_center;
	double m_fR;
	RayColor m_kd, m_ks, m_ka, m_kt;
	double m_fEta;
	int m_nPhongN;								// 镜面高光指数
	bool m_bSpecular;							// 是否为镜面
	bool m_bTransparent;						// 是否为透明面
public:
	Sphere(const RayVector center, const double r,
		   const RayColor &kd, const RayColor &ks, const RayColor &ka, const RayColor &kt,
		   const double eta, const int phongN, const bool specular, const bool transparent);
	Sphere(Sphere const &);
	const RayVector center() const;
	double r() const;
	void setCenter(const RayVector &center);
	void setR(const double r);
	const RayColor kd() const;
	const RayColor ks() const;
	const RayColor ka() const;
	const RayColor kt() const;
	double eta() const;
	int phongN() const;
	bool specular() const;
	bool transparent() const;
};

enum VertexCase						// 这个面中顶点数据的类型
{
	V_NO_NO,						// 只有几何顶点
	V_NO_VN,						// 有几何顶点和顶点法向
	V_VT_VN							// 有几何顶点、顶点纹理和顶点法向
};

class Face								// 存放面信息
{
	RayColor m_kd, m_ks, m_ka, m_kt;
	double m_fEta;
	int m_nPhongN;								// 镜面高光指数
	bool m_bSpecular;							// 是否为镜面
	bool m_bTransparent;						// 是否为透明面
public:
	int geoVertexIndex[3];
	int textureVertexIndex[3];
	int vertexNormalIndex[3];
	VertexCase m_eVertexCase;
	Face(const RayColor &kd, const RayColor &ks, const RayColor &ka, const RayColor &kt,
		 const double eta, const int phongN, const bool specular, const bool transparent);
	Face(const Face& face);
	const RayColor kd() const;
	const RayColor ks() const;
	const RayColor ka() const;
	const RayColor kt() const;
	double eta() const;
	int phongN() const;
	bool specular() const;
	bool transparent() const;
};

class Object
{
	static const int MAXFACE = 1000;
public:
	std::vector<Face> faceList;
	std::vector<Sphere> sphereList;
	Object();
	Object(const Object& object);
};

class ObjData							// 存放obj文件内数据
{
	static const int MAXOBJECT = 100;
	static const int MAXVERTEX = 10000;
	static const int MAXLIGHT = 100;
	int m_nMaxDepth;
	RayColor m_ia, m_kd, m_ks, m_ka, m_kt;
	double m_fEta;
	double m_fEyeX, m_fEyeY;					// 视点坐标
	int m_nPhongN;								// 最近的镜面高光指数
	bool m_bSpecular;							// 最近一次设置是否为镜面的值
	bool m_bTransparent;						// 最近一次设置是否为透明面
public:
	std::vector<RayVector> geoVertexList;			// 存放顶点坐标
	std::vector<RayVector> textureVertexList;		// 存放纹理坐标
	std::vector<RayVector> vertexNormalList;			// 存放法向坐标
	std::vector<Object> objectList;
	std::vector<RaySpotLight> lightList;
	FILE *m_pFile;
	ObjData();
	void initialize();					// 对相应数据进行初始化
	void readObj(const char *fileName);	// 读取obj文件数据并存放
	int maxDepth() const;
	double eyeX() const;
	double eyeY() const;
	double eyeZ() const;
	const RayColor ia() const;
	int getVertexNum();					// 返回顶点数量
	int getTextureNum();				// 返回纹理数量
	int getNormalNum();					// 返回法向数量
	int getFaceNum();					// 返回面数量
	int getObjNum();					// 返回物体数量
	void parseVertex(const char *str);	// 解析顶点坐标
	void parseTexture(const char *str);	// 解析纹理坐标
	void parseNormal(const char *str);	// 解析法向坐标
	void parseLight(const char *str);	// 解析点光源
	void parseMaxDepth(const char *str);// 解析最大跟踪深度
	void parseEye(const char* str);		// 解析视点坐标
	void parseIa(const char* str);		// 解析环境光
	void parseKd(const char* str);		// 解析漫反射系数
	void parseKs(const char* str);		// 解析镜面反射系数
	void parseKa(const char* str);		// 解析环境反射系数
	void parseKt(const char* str);		// 解析透射系数
	void parsePhongN(const char* str);	// 解析镜面高光系数
	void parseSphere(const char* str, int objectIndex);	// 解析球面信息
	void parseFace(char *str, int objectIndex);			// 解析平面信息
};

#endif
