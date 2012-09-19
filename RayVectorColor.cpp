#include "RayVectorColor.h"
#include <math.h>

const double RayVector::ZERO = 0.0000001;

RayVector::RayVector(const double x, const double y, const double z)
{
	m_fX = x;
	m_fY = y;
	m_fZ = z;
}

RayVector::RayVector(const RayVector& vector)
{
	m_fX = vector.m_fX;
	m_fY = vector.m_fY;
	m_fZ = vector.m_fZ;
}

void RayVector::setX(const double x)
{
	m_fX = x;
}

void RayVector::setY(const double y)
{
	m_fY = y;
}

void RayVector::setZ(const double z)
{
	m_fZ = z;
}

double RayVector::x() const
{
	return m_fX;
}

double RayVector::y() const
{
	return m_fY;
}

double RayVector::z() const
{
	return m_fZ;
}

const RayVector RayVector::operator+(const RayVector& vector) const
{
	return RayVector(m_fX + vector.m_fX, m_fY + vector.m_fY, m_fZ + vector.m_fZ);
}

const RayVector RayVector::operator-(const RayVector& vector) const
{
	return RayVector(m_fX - vector.m_fX, m_fY - vector.m_fY, m_fZ - vector.m_fZ);
}

const RayVector RayVector::operator-() const
{
	return RayVector(-m_fX, -m_fY, -m_fZ);
}

double RayVector::operator*(const RayVector& vector) const
{
	double result = 0.0;
	result += m_fX * vector.m_fX;
	result += m_fY * vector.m_fY;
	result += m_fZ * vector.m_fZ;

	return result;
}

const RayVector RayVector::cross(const RayVector& vector) const
{
	double x1 = m_fX;
	double y1 = m_fY;
	double z1 = m_fZ;

	double x2 = vector.m_fX;
	double y2 = vector.m_fY;
	double z2 = vector.m_fZ;

	return RayVector(y1 * z2 - y2 * z1,
					 x2 * z1 - x1 * z2,
					 x1 * y2 - x2 * y1);
}

const RayVector RayVector::operator*(double factor) const
{
	return RayVector(m_fX * factor, m_fY * factor, m_fZ * factor);
}

const RayVector RayVector::operator/(double factor) const
{
	if (factor < ZERO)
		return RayVector(m_fX, m_fY, m_fZ);
	else
		return RayVector(m_fX / factor, m_fY / factor, m_fZ / factor);
}

RayVector& RayVector::operator=(const RayVector& vector)
{
	if (this == &vector)
		return *this;
	m_fX = vector.m_fX;
	m_fY = vector.m_fY;
	m_fZ = vector.m_fZ;
	return *this;
}

RayVector& RayVector::operator+=(const RayVector& vector)
{
	if (this == &vector)
	{
		m_fX *= 2;
		m_fY *= 2;
		m_fZ *= 2;
		return *this;
	}
	m_fX += vector.m_fX;
	m_fY += vector.m_fY;
	m_fZ += vector.m_fZ;
	return *this;
}

void RayVector::normalize()
{
	double length = sqrt(m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ);
	if (length < ZERO)
		return;
	m_fX /= length;
	m_fY /= length;
	m_fZ /= length;
}

/**********************************************************************/

RayColor::RayColor(const double r, const double g, const double b)
{
	m_fR = r;
	m_fG = g;
	m_fB = b;
}

RayColor::RayColor(const RayColor& color)
{
	m_fR = color.m_fR;
	m_fG = color.m_fG;
	m_fB = color.m_fB;
}

void RayColor::setR(const double r)
{
	m_fR = r;
}

void RayColor::setG(const double g)
{
	m_fG = g;
}

void RayColor::setB(const double b)
{
	m_fB = b;
}

double RayColor::r() const
{
	return m_fR;
}

double RayColor::g() const
{
	return m_fG;
}

double RayColor::b() const
{
	return m_fB;
}

const RayColor RayColor::operator+(const RayColor& color) const
{
	return RayColor(m_fR + color.m_fR, m_fG + color.m_fG, m_fB + color.m_fB);
}

const RayColor RayColor::operator*(const RayColor& factor) const
{
	return RayColor(m_fR * factor.m_fR, m_fG * factor.m_fG, m_fB * factor.m_fB);
}

const RayColor RayColor::operator*(double reflex) const
{
	return RayColor(m_fR * reflex, m_fG * reflex, m_fB * reflex);
}

RayColor& RayColor::operator=(const RayColor& color)
{
	if (this == &color)
		return *this;
	m_fR = color.m_fR;
	m_fG = color.m_fG;
	m_fB = color.m_fB;
	return *this;
}

RayColor& RayColor::operator+=(const RayColor& color)
{
	if (this == &color)
	{
		m_fR *= 2;
		m_fG *= 2;
		m_fB *= 2;
		return *this;
	}
	m_fR += color.m_fR;
	m_fG += color.m_fG;
	m_fB += color.m_fB;
	return *this;
}

RayColor& RayColor::operator*=(const RayColor& color)
{
	if (this == &color)
	{
		m_fR *= m_fR;
		m_fG *= m_fG;
		m_fB *= m_fB;
		return *this;
	}
	m_fR *= color.m_fR;
	m_fG *= color.m_fG;
	m_fB *= color.m_fB;
	return *this;
}

void RayColor::normalize()
{
	if (m_fR > 1.0)
		m_fR = 1.0;
	else if (m_fR < 0.0)
		m_fR = 0.0;
	if (m_fG > 1.0)
		m_fG = 1.0;
	else if (m_fG < 0.0)
		m_fG = 0.0;
	if (m_fB > 1.0)
		m_fB = 1.0;
	else if (m_fB < 0.0)
		m_fB = 0.0;
}

/**********************************************************************/

RaySpotLight::RaySpotLight(const double x, const double y, const double z,
						   const double r, const double g, const double b)
:position(x, y, z), color(r, g, b){}

RaySpotLight::RaySpotLight(const RaySpotLight& light)
:position(light.position.x(),
		  light.position.y(),
		  light.position.z())
,color	 (light.color.r(),
		  light.color.g(),
		  light.color.b()){}
