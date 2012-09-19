#include "RayView.h"

RayView::RayView(QWidget *parent, RayCommonData *commonData)
:ArthurFrame(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_pCommonData = commonData;
	m_pCommonData->m_nScrWidth = width();
	m_pCommonData->m_nScrHeight = height();
}

void RayView::paint(QPainter *painter)		// ��ͼ����
{
	m_pCommonData->m_nScrWidth = width();
	m_pCommonData->m_nScrHeight = height();
	if (!m_pCommonData->m_bFinished)								// ���û������ģ�ͣ���������л���
		return;

	/* ʹ����������Ļ���� */
	int deltaI = 0, deltaJ = 0, endI = m_pCommonData->m_nSceneHeight, endJ = m_pCommonData->m_nSceneWidth;
	if (m_pCommonData->m_nScrHeight > m_pCommonData->m_nSceneHeight)
		deltaI = (m_pCommonData->m_nScrHeight - m_pCommonData->m_nSceneHeight) / 2;
	else
		endI = m_pCommonData->m_nScrHeight;
	if (m_pCommonData->m_nScrWidth > m_pCommonData->m_nSceneWidth)
		deltaJ = (m_pCommonData->m_nScrWidth - m_pCommonData->m_nSceneWidth) / 2;
	else
		endJ = m_pCommonData->m_nScrWidth;

	/* ���Ѿ����ƺõĳ�����ʾ����Ļ�� */
	for (int i = 0; i < endI; ++i)
	{
		for (int j = 0; j < endJ; ++j)
		{
			QPoint p(j + deltaJ, i + deltaI);
			QColor penColor((int)((m_pCommonData->scene)[i][j].r()),
							(int)((m_pCommonData->scene)[i][j].g()),
							(int)((m_pCommonData->scene)[i][j].b()));
			painter->setPen(QPen(penColor, 1.0));
			painter->drawPoint(p);
		}
	}
}
