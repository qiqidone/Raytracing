#include "RayPixel.h"

RayPixel::RayPixel(unsigned short r, unsigned short g, unsigned short b)
{
	m_usR = r;
	m_usG = g;
	m_usB = b;
}

RayPixel::RayPixel(const RayPixel &pixel)
{
	m_usR = pixel.m_usR;
	m_usG = pixel.m_usG;
	m_usB = pixel.m_usB;
}

unsigned short RayPixel::r() const
{
	return m_usR;
}

unsigned short RayPixel::g() const
{
	return m_usG;
}

unsigned short RayPixel::b() const
{
	return m_usB;
}

void RayPixel::setR(double r)
{
	m_usR = (unsigned short)(r * 255.0);
}

void RayPixel::setG(double g)
{
	m_usG = (unsigned short)(g * 255.0);
}

void RayPixel::setB(double b)
{
	m_usB = (unsigned short)(b * 255.0);
}
