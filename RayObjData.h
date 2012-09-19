/*
 * �����˶�ȡ�ʹ洢obj�ļ��������
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
	int m_nPhongN;								// ����߹�ָ��
	bool m_bSpecular;							// �Ƿ�Ϊ����
	bool m_bTransparent;						// �Ƿ�Ϊ͸����
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

enum VertexCase						// ������ж������ݵ�����
{
	V_NO_NO,						// ֻ�м��ζ���
	V_NO_VN,						// �м��ζ���Ͷ��㷨��
	V_VT_VN							// �м��ζ��㡢��������Ͷ��㷨��
};

class Face								// �������Ϣ
{
	RayColor m_kd, m_ks, m_ka, m_kt;
	double m_fEta;
	int m_nPhongN;								// ����߹�ָ��
	bool m_bSpecular;							// �Ƿ�Ϊ����
	bool m_bTransparent;						// �Ƿ�Ϊ͸����
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

class ObjData							// ���obj�ļ�������
{
	static const int MAXOBJECT = 100;
	static const int MAXVERTEX = 10000;
	static const int MAXLIGHT = 100;
	int m_nMaxDepth;
	RayColor m_ia, m_kd, m_ks, m_ka, m_kt;
	double m_fEta;
	double m_fEyeX, m_fEyeY;					// �ӵ�����
	int m_nPhongN;								// ����ľ���߹�ָ��
	bool m_bSpecular;							// ���һ�������Ƿ�Ϊ�����ֵ
	bool m_bTransparent;						// ���һ�������Ƿ�Ϊ͸����
public:
	std::vector<RayVector> geoVertexList;			// ��Ŷ�������
	std::vector<RayVector> textureVertexList;		// �����������
	std::vector<RayVector> vertexNormalList;			// ��ŷ�������
	std::vector<Object> objectList;
	std::vector<RaySpotLight> lightList;
	FILE *m_pFile;
	ObjData();
	void initialize();					// ����Ӧ���ݽ��г�ʼ��
	void readObj(const char *fileName);	// ��ȡobj�ļ����ݲ����
	int maxDepth() const;
	double eyeX() const;
	double eyeY() const;
	double eyeZ() const;
	const RayColor ia() const;
	int getVertexNum();					// ���ض�������
	int getTextureNum();				// ������������
	int getNormalNum();					// ���ط�������
	int getFaceNum();					// ����������
	int getObjNum();					// ������������
	void parseVertex(const char *str);	// ������������
	void parseTexture(const char *str);	// ������������
	void parseNormal(const char *str);	// ������������
	void parseLight(const char *str);	// �������Դ
	void parseMaxDepth(const char *str);// �������������
	void parseEye(const char* str);		// �����ӵ�����
	void parseIa(const char* str);		// ����������
	void parseKd(const char* str);		// ����������ϵ��
	void parseKs(const char* str);		// �������淴��ϵ��
	void parseKa(const char* str);		// ������������ϵ��
	void parseKt(const char* str);		// ����͸��ϵ��
	void parsePhongN(const char* str);	// ��������߹�ϵ��
	void parseSphere(const char* str, int objectIndex);	// ����������Ϣ
	void parseFace(char *str, int objectIndex);			// ����ƽ����Ϣ
};

#endif
