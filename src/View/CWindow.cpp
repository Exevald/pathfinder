#include "CWindow.h"
#include "COpenGLWidget.h"
#include <QFileDialog>
#include <QGraphicsRectItem>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

CWindow::CWindow(QWidget* parent, std::unique_ptr<CGridViewModel> viewModel)
	: QMainWindow(parent)
	, m_graphicsView(nullptr)
	, m_scene(nullptr)
	, m_gridViewModel(std::move(viewModel))
{
	ShowMainMenu();
}

void CWindow::ShowMainMenu()
{
	auto* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("background-color: white;");
	setCentralWidget(centralWidget);
	centralWidget->setMinimumSize(1920, 1080);

	auto* layout = new QVBoxLayout(centralWidget);
	layout->setContentsMargins(50, 100, 0, 0);

	auto* leftLayout = new QVBoxLayout();

	auto* titleLabel = new QLabel("Pathfinder", this);
	titleLabel->setStyleSheet(
		"QLabel {"
		"   font-size: 50px;"
		"   font-weight: 600;"
		"   color: black;"
		" 	margin-bottom: 100px;"
		"}");

	layout->addWidget(titleLabel, 0, Qt::AlignLeft | Qt::AlignTop);

	auto* loadCADButton = new QPushButton("Загрузить CAD-файл", this);
	connect(loadCADButton, &QPushButton::clicked, this, &CWindow::OnOpenCADFile);

	auto* exitButton = new QPushButton("Выход", this);
	connect(exitButton, &QPushButton::clicked, this, []() { QApplication::quit(); });

	loadCADButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #0E39C7;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 20px 25px;"
		"	font-size: 18px;"
		"	font-weight: 500;"
		"}"
		"QPushButton:hover {"
		"   background-color: #0E34B4;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #0C2FA1;"
		"}");

	exitButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #0E39C7;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 20px 25px;"
		"	font-size: 18px;"
		"	font-weight: 500;"
		"}"
		"QPushButton:hover {"
		"   background-color: #0E34B4;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #0C2FA1;"
		"}");

	layout->addStretch();
	layout->addWidget(loadCADButton, 0, Qt::AlignLeft);
	layout->addWidget(exitButton, 0, Qt::AlignLeft);
	layout->addStretch();

	auto iconLabel = new QLabel(this);
	QPixmap originalPixmap("../Compass.svg");
	QPixmap scaledPixmap = originalPixmap.scaled(650, 650, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	iconLabel->setPixmap(scaledPixmap);
	iconLabel->setAlignment(Qt::AlignRight);
	layout->addWidget(iconLabel);

	layout->setSpacing(20);
}

void CWindow::OnOpenCADFile()
{
	QString filePath = QFileDialog::getOpenFileName(
		this,
		"Выберите CAD-файл",
		"",
		"(*.obj);;All Files (*)");

	if (!filePath.isEmpty())
	{
		m_gridViewModel->LoadData(filePath.toStdString());
		Draw3DSpace();
	}
}

void CWindow::OnDraw3DSpace()
{
	Draw3DSpace();
}

void CWindow::Draw3DSpace()
{
	auto* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("background-color: white;");
	setCentralWidget(centralWidget);

	auto* layout = new QVBoxLayout(centralWidget);
	layout->setContentsMargins(50, 50, 50, 50);

	auto* openGLWidget = new COpenGLWidget(this);
	openGLWidget->setFixedSize(1440, 860);
	layout->addWidget(openGLWidget);

	auto* infoContainer = new QWidget(this);
	infoContainer->setFixedWidth(300);
	infoContainer->setStyleSheet(
		"QWidget {"
		"   background-color: #F5F5F5;"
		"   border-radius: 10px;"
		"   padding: 20px;"
		"	border: 1px solid black"
		"	width"
		"}");

	auto* infoLayout = new QVBoxLayout(infoContainer);
	infoLayout->setSpacing(10);

	auto* fileNameLabel = new QLabel("Файл: " + QString::fromStdString("tinker.obj"), this);
	fileNameLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(fileNameLabel);

	auto* droneSizeLabel = new QLabel("Размеры дрона: 0 x 0 x 0", this);
	droneSizeLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(droneSizeLabel);

	auto* gridSizeLabel = new QLabel("Размер сетки: 0 x 0 x 0", this);
	gridSizeLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(gridSizeLabel);

	double shortestPathLength = 0;
	auto* pathLengthLabel = new QLabel("Длина кратчайшего пути: " + (shortestPathLength > 0 ? QString::number(shortestPathLength) : "Неизвестно"), this);
	pathLengthLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(pathLengthLabel);

	infoLayout->addWidget(infoContainer, 0, Qt::AlignRight);

	auto* backButton = new QPushButton("Выход в меню", this);
	connect(backButton, &QPushButton::clicked, this, &CWindow::OnBackToMenu);
	backButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #0E39C7;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 10px 15px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #0E34B4;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #0C2FA1;"
		"}");

	layout->addSpacing(20);
	layout->addWidget(backButton, 0, Qt::AlignLeft);

	auto* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(layout);
	mainLayout->addSpacing(20);
	mainLayout->addLayout(infoLayout);

	centralWidget->setLayout(mainLayout);
}

void CWindow::OnBackToMenu()
{
	ShowMainMenu();
}