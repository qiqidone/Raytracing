#include "RayWidget.h"
#include "arthurstyle.h"
#include "arthurwidgets.h"

extern void draw_round_rect(QPainter *p, const QRect &bounds, int radius);

RayWidget::RayWidget()
{
	setWindowTitle(tr("光线跟踪算法"));
	m_commonData.m_bFinished = false;
	m_rayView = new RayView(this, &m_commonData);
	m_rayRenderer = new RayRenderer(&m_commonData);

	QPalette pal = palette();
	setPalette(pal);

	/******************* 布局 ********************/
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(m_rayView);

	QGroupBox *ctrlGroupBox = new QGroupBox(this);
	ctrlGroupBox->setTitle(tr("控制面板"));
	//threadNumSpinBox->setFixedWidth(threadNumSpinBox->sizeHint().width());
	ctrlGroupBox->setFixedWidth(ctrlGroupBox->minimumSize().width());

	loadButton = new QPushButton(ctrlGroupBox);
	loadButton->setText(tr("打开文件"));

	QGroupBox *statusGroupBox = new QGroupBox(ctrlGroupBox);
	statusGroupBox->setTitle(tr("模型统计信息"));
	QLabel *vertexLabel = new QLabel(tr("顶点数量:"));
	QLabel *textureLabel = new QLabel(tr("纹理数量:"));
	QLabel *normalLabel = new QLabel(tr("法向数量:"));
	QLabel *faceLabel = new QLabel(tr("面片数量:"));
	QLabel *objLabel = new QLabel(tr("物体数量"));

	vertexLCDNumber = new QLCDNumber(6, statusGroupBox);
	textureLCDNumber = new QLCDNumber(6, statusGroupBox);
	normalLCDNumber = new QLCDNumber(6, statusGroupBox);
	faceLCDNumber = new QLCDNumber(6, statusGroupBox);
	objLCDNumber = new QLCDNumber(6, statusGroupBox);

	QGroupBox *renderModeBox = new QGroupBox(ctrlGroupBox);
	renderModeBox->setTitle(tr("是否加速"));
	QRadioButton *BSPRButton = new QRadioButton(renderModeBox);
    QRadioButton *noBSPRButton = new QRadioButton(renderModeBox);
    BSPRButton->setText(tr("使用BSP加速"));
    noBSPRButton->setText(tr("不加速"));
	BSPRButton->setChecked(true);

	QGroupBox *timeBox = new QGroupBox(ctrlGroupBox);
	timeBox->setTitle(tr("进度"));
	QLabel *renderTimeLabel = new QLabel(tr("绘制时间(秒):"));
	QLabel *progressLabel = new QLabel(tr("绘制进度"));
	renderTimeLCDNumber = new QLCDNumber(6, timeBox);
	progressLCDNumber = new QLCDNumber(3, timeBox);

	QGroupBox *threadBox = new QGroupBox(ctrlGroupBox);
	threadBox->setTitle(tr("CPU线程数"));
	threadNumSlider = new QSlider(Qt::Horizontal);
	threadNumSlider->setRange(1, 32);
	threadNumSlider->setValue(m_commonData.m_nThreadNum);

	threadNumSpinBox = new QSpinBox;
	threadNumSpinBox->setRange(1, 32);
	threadNumSpinBox->setValue(m_commonData.m_nThreadNum);

	QVBoxLayout *statusLayout = new QVBoxLayout(statusGroupBox);
	statusLayout->addWidget(vertexLabel);
	statusLayout->addWidget(vertexLCDNumber);
	statusLayout->addWidget(textureLabel);
	statusLayout->addWidget(textureLCDNumber);
	statusLayout->addWidget(normalLabel);
	statusLayout->addWidget(normalLCDNumber);
	statusLayout->addWidget(faceLabel);
	statusLayout->addWidget(faceLCDNumber);
	statusLayout->addWidget(objLabel);
	statusLayout->addWidget(objLCDNumber);

	QVBoxLayout *renderModeLayout = new QVBoxLayout(renderModeBox);
	renderModeLayout->addWidget(BSPRButton);
	renderModeLayout->addWidget(noBSPRButton);
	
	QVBoxLayout *timeLayout = new QVBoxLayout(timeBox);
	timeLayout->addWidget(renderTimeLabel);
	timeLayout->addWidget(renderTimeLCDNumber);
	timeLayout->addWidget(progressLabel);
	timeLayout->addWidget(progressLCDNumber);

	QHBoxLayout *threadLayout = new QHBoxLayout(threadBox);
	threadLayout->addWidget(threadNumSlider);
	threadLayout->addWidget(threadNumSpinBox);

	QVBoxLayout *ctrlPanelLayout = new QVBoxLayout(ctrlGroupBox);
	ctrlPanelLayout->addWidget(loadButton);
	ctrlPanelLayout->addWidget(statusGroupBox);
	ctrlPanelLayout->addWidget(renderModeBox);
	ctrlPanelLayout->addWidget(timeBox);
	ctrlPanelLayout->addWidget(threadBox);

	QVBoxLayout *rightLayout = new QVBoxLayout;
	rightLayout->addWidget(ctrlGroupBox);
	rightLayout->addStretch();

	mainLayout->addLayout(rightLayout);

	ctrlGroupBox->setFixedWidth(ctrlGroupBox->sizeHint().width());
	ctrlGroupBox->setFixedHeight(ctrlGroupBox->sizeHint().height());

	updateTimer = new QTimer(this);
	m_bUseBSP = true;
	/******************* 连接 ********************/
	connect(loadButton, SIGNAL(clicked()), this, SLOT(open()));
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateRenderTime()));
	connect(BSPRButton, SIGNAL(clicked()), this, SLOT(setUseBSP()));
	connect(noBSPRButton, SIGNAL(clicked()), this, SLOT(setNotUseBSP()));

	connect(threadNumSlider, SIGNAL(valueChanged(int)), threadNumSpinBox, SLOT(setValue(int)));
	connect(threadNumSpinBox, SIGNAL(valueChanged(int)), threadNumSlider, SLOT(setValue(int)));
	connect(threadNumSlider, SIGNAL(valueChanged(int)), this, SLOT(setThreadNum(int)));
	connect(threadNumSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setThreadNum(int)));
}

void RayWidget::open()					// 打开文件
{
	QString fileName = QFileDialog::getOpenFileName(this,
								tr("打开obj文件"), "../OBJ",
								tr("obj文件 (*.obj)"));
	if (!fileName.isEmpty())
	{
		m_commonData.m_bFinished = false;
		m_commonData.m_nFinishHeight = 0;
		progressLCDNumber->display(0);
		m_nRenderTime = 0;
		renderTimeLCDNumber->display(0);
		/* 将QString类型数据转化为char *类型数据 */
		QByteArray ba = fileName.toLocal8Bit();
		const char *c_str = ba.data();
		m_commonData.m_nSceneWidth = m_commonData.m_nScrWidth;
		m_commonData.m_nSceneHeight = m_commonData.m_nScrHeight;
		updateTimer->start(1000);
		m_rayRenderer->readObj(c_str);
		/*progressLCDNumber->display(100);
		updateTimer->stop();*/

		m_nVertexNum = m_rayRenderer->getVertexNum();			// 得到顶点数量
		m_nTextureNum = m_rayRenderer->getTextureNum();			// 得到纹理数量
		m_nNormalNum = m_rayRenderer->getNormalNum();			// 得到法向数量
		m_nFaceNum = m_rayRenderer->getFaceNum();				// 得到面数量
		m_nObjNum = m_rayRenderer->getObjNum();					// 得到物体数量
		vertexLCDNumber->display(m_nVertexNum);					// 在液晶屏上显示顶点数量
		textureLCDNumber->display(m_nTextureNum);				// 在液晶屏上显示纹理数量
		normalLCDNumber->display(m_nNormalNum);					// 在液晶屏上显示法向数量
		faceLCDNumber->display(m_nFaceNum);						// 在液晶屏上显示面数量
		objLCDNumber->display(m_nObjNum);						// 在液晶屏上显示物体数量
		m_rayRenderer->renderScene(m_bUseBSP);
		progressLCDNumber->display(100);
		updateTimer->stop();
		m_commonData.m_bFinished = true;
		m_rayView->update();
	}
}

void RayWidget::updateRenderTime()
{
	renderTimeLCDNumber->display(++m_nRenderTime);
	int temp = m_commonData.m_nFinishHeight * 100 / m_commonData.m_nSceneHeight;
	progressLCDNumber->display(temp);
}

void RayWidget::setUseBSP()			// 以平面方式绘制面片
{
	m_bUseBSP = true;
}

void RayWidget::setNotUseBSP()		// 以双线性插值方式绘制面片
{
	m_bUseBSP = false;
}

void RayWidget::setThreadNum(int threadNum)
{
	m_commonData.m_nThreadNum = threadNum;
}
