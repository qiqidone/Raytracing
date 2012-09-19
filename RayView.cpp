#include "RayView.h"

RayView::RayView(QWidget *parent, RayCommonData *commonData)
:ArthurFrame(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_pCommonData = commonData;
	m_pCommonData->m_nScrWidth = width();
	m_pCommonData->m_nScrHeight = height();
}

void RayView::paint(QPainter *painter)		// 绘图函数
{
	m_pCommonData->m_nScrWidth = width();
	m_pCommonData->m_nScrHeight = height();
	if (!m_pCommonData->m_bFinished)								// 如果没有载入模型，则无需进行绘制
		return;

	/* 使场景处于屏幕中央 */
	int deltaI = 0, deltaJ = 0, endI = m_pCommonData->m_nSceneHeight, endJ = m_pCommonData->m_nSceneWidth;
	if (m_pCommonData->m_nScrHeight > m_pCommonData->m_nSceneHeight)
		deltaI = (m_pCommonData->m_nScrHeight - m_pCommonData->m_nSceneHeight) / 2;
	else
		endI = m_pCommonData->m_nScrHeight;
	if (m_pCommonData->m_nScrWidth > m_pCommonData->m_nSceneWidth)
		deltaJ = (m_pCommonData->m_nScrWidth - m_pCommonData->m_nSceneWidth) / 2;
	else
		endJ = m_pCommonData->m_nScrWidth;

	/* 将已经绘制好的场景显示到屏幕上 */
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
