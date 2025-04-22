#include "CWindow.h"
#include "COpenGLWidget.h"
#include <QFileDialog>
#include <QGraphicsRectItem>
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

	auto* layout = new QVBoxLayout(centralWidget);

	auto* loadCADButton = new QPushButton("Загрузить CAD-файл", this);
	connect(loadCADButton, &QPushButton::clicked, this, &CWindow::OnOpenCADFile);
	loadCADButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #4CAF50;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 10px 15px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #45A049;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #3E8E41;"
		"}");

	auto* drawButton = new QPushButton("Отрисовать", this);
	connect(drawButton, &QPushButton::clicked, this, &CWindow::OnDraw3DSpace);
	drawButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #2196F3;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 10px 15px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #1E88E5;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #1976D2;"
		"}");

	auto* exitButton = new QPushButton("Выход", this);
	connect(exitButton, &QPushButton::clicked, this, []() { QApplication::quit(); });
	exitButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #F44336;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 10px 15px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #D32F2F;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #C62828;"
		"}");

	layout->addStretch();
	layout->addWidget(loadCADButton, 0, Qt::AlignCenter);
	layout->addWidget(drawButton, 0, Qt::AlignCenter);
	layout->addWidget(exitButton, 0, Qt::AlignCenter);
	layout->addStretch();

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
	}
}

void CWindow::OnDraw3DSpace()
{
	Draw3DSpace();
}

void CWindow::Draw3DSpace()
{
	auto* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	auto* layout = new QVBoxLayout(centralWidget);

	auto* openGLWidget = new COpenGLWidget(this);
	openGLWidget->setMinimumSize(800, 600);

	layout->addWidget(openGLWidget);

	auto* backButton = new QPushButton("Выход в меню", this);
	connect(backButton, &QPushButton::clicked, this, &CWindow::OnBackToMenu);
	backButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #FFC107;"
		"   color: black;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 10px 15px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #FFA000;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #FF8F00;"
		"}");

	layout->addWidget(backButton, 0, Qt::AlignCenter);
}

void CWindow::OnBackToMenu()
{
	ShowMainMenu();
}