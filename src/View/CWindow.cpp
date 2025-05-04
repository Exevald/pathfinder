#include "CWindow.h"
#include "COpenGLWidget.h"
#include <QFileDialog>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

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
	QPixmap originalPixmap("../../Compass.svg");
	QPixmap scaledPixmap = originalPixmap.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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