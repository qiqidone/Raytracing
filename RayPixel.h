/*
 * 定义了一个用来存储像素rgb分量的类
 */

#ifndef __RAYPIXEL_H__
#define __RAYPIXEL_H__

class RayPixel
{
	unsigned short m_usR, m_usG, m_usB;
public:
	RayPixel(unsigned short r = 0, unsigned short g = 0, unsigned short b = 0);
	RayPixel(const RayPixel &pixel);
	unsigned short r() const;
	unsigned short g() const;
	unsigned short b() const;
	void setR(double r);
	void setG(double g);
	void setB(double b);
};

#endif
