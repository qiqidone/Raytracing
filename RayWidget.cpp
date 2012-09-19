#include "RayWidget.h"
#include "arthurstyle.h"
#include "arthurwidgets.h"

extern void draw_round_rect(QPainter *p, const QRect &bounds, int radius);

RayWidget::RayWidget()
{
	setWindowTitle(tr("���߸����㷨"));
	m_commonData.m_bFinished = false;
	m_rayView = new RayView(this, &m_commonData);
	m_rayRenderer = new RayRenderer(&m_commonData);

	QPalette pal = palette();
	setPalette(pal);

	/******************* ���� ********************/
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(m_rayView);

	QGroupBox *ctrlGroupBox = new QGroupBox(this);
	ctrlGroupBox->setTitle(tr("�������"));
	//threadNumSpinBox->setFixedWidth(threadNumSpinBox->sizeHint().width());
	ctrlGroupBox->setFixedWidth(ctrlGroupBox->minimumSize().width());

	loadButton = new QPushButton(ctrlGroupBox);
	loadButton->setText(tr("���ļ�"));

	QGroupBox *statusGroupBox = new QGroupBox(ctrlGroupBox);
	statusGroupBox->setTitle(tr("ģ��ͳ����Ϣ"));
	QLabel *vertexLabel = new QLabel(tr("��������:"));
	QLabel *textureLabel = new QLabel(tr("��������:"));
	QLabel *normalLabel = new QLabel(tr("��������:"));
	QLabel *faceLabel = new QLabel(tr("��Ƭ����:"));
	QLabel *objLabel = new QLabel(tr("��������"));

	vertexLCDNumber = new QLCDNumber(6, statusGroupBox);
	textureLCDNumber = new QLCDNumber(6, statusGroupBox);
	normalLCDNumber = new QLCDNumber(6, statusGroupBox);
	faceLCDNumber = new QLCDNumber(6, statusGroupBox);
	objLCDNumber = new QLCDNumber(6, statusGroupBox);

	QGroupBox *renderModeBox = new QGroupBox(ctrlGroupBox);
	renderModeBox->setTitle(tr("�Ƿ����"));
	QRadioButton *BSPRButton = new QRadioButton(renderModeBox);
    QRadioButton *noBSPRButton = new QRadioButton(renderModeBox);
    BSPRButton->setText(tr("ʹ��BSP����"));
    noBSPRButton->setText(tr("������"));
	BSPRButton->setChecked(true);

	QGroupBox *timeBox = new QGroupBox(ctrlGroupBox);
	timeBox->setTitle(tr("����"));
	QLabel *renderTimeLabel = new QLabel(tr("����ʱ��(��):"));
	QLabel *progressLabel = new QLabel(tr("���ƽ���"));
	renderTimeLCDNumber = new QLCDNumber(6, timeBox);
	progressLCDNumber = new QLCDNumber(3, timeBox);

	QGroupBox *threadBox = new QGroupBox(ctrlGroupBox);
	threadBox->setTitle(tr("CPU�߳���"));
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
	/******************* ���� ********************/
	connect(loadButton, SIGNAL(clicked()), this, SLOT(open()));
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateRenderTime()));
	connect(BSPRButton, SIGNAL(clicked()), this, SLOT(setUseBSP()));
	connect(noBSPRButton, SIGNAL(clicked()), this, SLOT(setNotUseBSP()));

	connect(threadNumSlider, SIGNAL(valueChanged(int)), threadNumSpinBox, SLOT(setValue(int)));
	connect(threadNumSpinBox, SIGNAL(valueChanged(int)), threadNumSlider, SLOT(setValue(int)));
	connect(threadNumSlider, SIGNAL(valueChanged(int)), this, SLOT(setThreadNum(int)));
	connect(threadNumSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setThreadNum(int)));
}

void RayWidget::open()					// ���ļ�
{
	QString fileName = QFileDialog::getOpenFileName(this,
								tr("��obj�ļ�"), "../OBJ",
								tr("obj�ļ� (*.obj)"));
	if (!fileName.isEmpty())
	{
		m_commonData.m_bFinished = false;
		m_commonData.m_nFinishHeight = 0;
		progressLCDNumber->display(0);
		m_nRenderTime = 0;
		renderTimeLCDNumber->display(0);
		/* ��QString��������ת��Ϊchar *�������� */
		QByteArray ba = fileName.toLocal8Bit();
		const char *c_str = ba.data();
		m_commonData.m_nSceneWidth = m_commonData.m_nScrWidth;
		m_commonData.m_nSceneHeight = m_commonData.m_nScrHeight;
		updateTimer->start(1000);
		m_rayRenderer->readObj(c_str);
		/*progressLCDNumber->display(100);
		updateTimer->stop();*/

		m_nVertexNum = m_rayRenderer->getVertexNum();			// �õ���������
		m_nTextureNum = m_rayRenderer->getTextureNum();			// �õ���������
		m_nNormalNum = m_rayRenderer->getNormalNum();			// �õ���������
		m_nFaceNum = m_rayRenderer->getFaceNum();				// �õ�������
		m_nObjNum = m_rayRenderer->getObjNum();					// �õ���������
		vertexLCDNumber->display(m_nVertexNum);					// ��Һ��������ʾ��������
		textureLCDNumber->display(m_nTextureNum);				// ��Һ��������ʾ��������
		normalLCDNumber->display(m_nNormalNum);					// ��Һ��������ʾ��������
		faceLCDNumber->display(m_nFaceNum);						// ��Һ��������ʾ������
		objLCDNumber->display(m_nObjNum);						// ��Һ��������ʾ��������
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

void RayWidget::setUseBSP()			// ��ƽ�淽ʽ������Ƭ
{
	m_bUseBSP = true;
}

void RayWidget::setNotUseBSP()		// ��˫���Բ�ֵ��ʽ������Ƭ
{
	m_bUseBSP = false;
}

void RayWidget::setThreadNum(int threadNum)
{
	m_commonData.m_nThreadNum = threadNum;
}
