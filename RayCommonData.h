/*
 * �������ࡢ�����ࡢ��ʾ��
 * ���õ����ݷ��ڴ�����
 */

#ifndef __RAYCOMMONDATA_H__
#define __RAYCOMMONDATA_H__

#include <vector>
#include "RayPixel.h"

struct RayCommonData
{
	std::vector<std::vector<RayPixel> > scene;		// ��������ʽ�����Ѿ����ƺõĳ���
	int m_nScrWidth, m_nScrHeight;			// ��Ļ��Ⱥ͸߶�
	int m_nSceneWidth, m_nSceneHeight;		// ������Ⱥ͸߶�
	int m_nThreadNum;						// ���Ƶ��߳�����
	volatile int m_nFinishHeight;			// ������ɵ�����
	bool m_bFinished;						// ģ���Ƿ��Ѿ��������
	RayCommonData();
};

#endif
