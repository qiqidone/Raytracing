/*
 * 定义了光线跟踪中最常用的两类数据结构：
 * 向量（包括点）和颜色（包括反射颜色系数）。
 * 同时还定义了表示点光源类
 */

#ifndef __RAYVECTORCOLOR_H__
#define __RAYVECTORCOLOR_H__

class RayVector
{
	double m_fX, m_fY, m_fZ;
	static const double ZERO;
public:
	void setX(const double x);
	void setY(const double y);
	void setZ(const double z);
	double x() const;
	double y() const;
	double z() const;

	RayVector(const double x = 0.0, const double y = 0.0, const double z = 0.0);
	RayVector(const RayVector& vector);
	const RayVector operator+(const RayVector& vector) const;
	const RayVector operator-(const RayVector& vector) const;
	const RayVector operator-() const;
	double operator*(const RayVector& vector) const;
	const RayVector cross(const RayVector& vector) const;
	const RayVector operator*(double factor) const;
	const RayVector operator/(double factor) const;
	RayVector& operator=(const RayVector& vector);
	RayVector& operator+=(const RayVector& color);
	void normalize();
};

class RayColor
{
	double m_fR, m_fG, m_fB;
public:
	void setR(const double r);
	void setG(const double g);
	void setB(const double b);
	double r() const;
	double g() const;
	double b() const;

	RayColor(const double r = 0.0, const double g = 0.0, const double b = 0.0);
	RayColor(const RayColor& color);
	const RayColor operator+(const RayColor& color) const;
	const RayColor operator*(const RayColor& color) const;
	const RayColor operator*(double angle) const;
	RayColor& operator=(const RayColor& color);
	RayColor& operator+=(const RayColor& color);
	RayColor& operator*=(const RayColor& color);
	void normalize();
};

class RaySpotLight
{
public:
	RayVector position;
	RayColor color;
	RaySpotLight(const double x = 0.0, const double y = 0.0, const double z = 0.0,
				 const double r = 0.0, const double g = 0.0, const double b = 0.0);
	RaySpotLight(const RaySpotLight& light);
};

#endif
