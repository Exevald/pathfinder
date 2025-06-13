#include "Window.h"

#include <QFileDialog>
#include <QGraphicsRectItem>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <memory>

Window::Window(QWidget* parent, std::unique_ptr<GridViewModel> viewModel)
	: QMainWindow(parent)
	, m_gridViewModel(std::move(viewModel))
{
	setWindowTitle("Pathfinder");
	ShowMainMenu();
}

void Window::ShowMainMenu()
{
	auto* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("background-color: white;");
	centralWidget->setMinimumSize(1920, 1080);

	auto* layout = new QVBoxLayout(centralWidget);
	layout->setContentsMargins(50, 100, 0, 0);

	auto* titleLabel = new QLabel("Pathfinder", this);
	titleLabel->setStyleSheet(
		"QLabel {"
		"   font-size: 50px;"
		"   font-weight: 600;"
		"   color: black;"
		"   margin-bottom: 100px;"
		"}");

	layout->addWidget(titleLabel, 0, Qt::AlignLeft | Qt::AlignTop);

	auto* loadOBJFileButton = new QPushButton("Загрузить OBJ-файл", this);
	connect(loadOBJFileButton, &QPushButton::clicked, this, &Window::OnOpenOBJFile);

	auto* exitButton = new QPushButton("Выход", this);
	connect(exitButton, &QPushButton::clicked, this, []() { QApplication::quit(); });

	loadOBJFileButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #0E39C7;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"   padding: 20px 25px;"
		"   font-size: 18px;"
		"   font-weight: 500;"
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
		"   font-size: 18px;"
		"   font-weight: 500;"
		"}"
		"QPushButton:hover {"
		"   background-color: #0E34B4;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #0C2FA1;"
		"}");

	layout->addStretch();
	layout->addWidget(loadOBJFileButton, 0, Qt::AlignLeft);
	layout->addWidget(exitButton, 0, Qt::AlignLeft);
	layout->addStretch();

	const auto iconLabel = new QLabel(this);
	const QPixmap originalPixmap("../../Compass.svg");
	const QPixmap scaledPixmap = originalPixmap.scaled(650, 650, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	iconLabel->setPixmap(scaledPixmap);
	iconLabel->setAlignment(Qt::AlignRight);
	layout->addWidget(iconLabel);

	layout->setSpacing(20);

	setCentralWidget(centralWidget);
}

void Window::OnOpenOBJFile()
{
	const QString filePath = QFileDialog::getOpenFileName(
		this,
		"Выберите OBJ-файл",
		"",
		"(*.obj);;All Files (*)");

	if (!filePath.isEmpty())
	{
		Draw3DSpace();

		if (m_space)
		{
			if (m_space->isGLInitialized())
			{
				m_gridViewModel->LoadData(filePath.toStdString());
				m_space->SetModel(m_gridViewModel->GetModel());
				m_space->update();
			}
			else
			{
				connect(m_space.get(), &Space::glInitialized, this, [this, filePath]() {
					m_gridViewModel->LoadData(filePath.toStdString());
					if (m_space)
					{
						m_space->SetModel(m_gridViewModel->GetModel());
						m_space->update();
					}
					disconnect(m_space.get(), nullptr, this, nullptr);
				});
			}
		}
	}
}

void Window::Draw3DSpace()
{
	// Старый центральный виджет будет удалён автоматически
	auto* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("background-color: white;");
	setCentralWidget(centralWidget);

	auto* mainLayout = new QVBoxLayout(centralWidget);
	mainLayout->setContentsMargins(50, 50, 50, 50);

	m_space = std::make_unique<Space>(centralWidget);
	m_space->setMinimumSize(800, 600);
	mainLayout->addWidget(m_space.get(), 1);

	auto* infoContainer = new QWidget(centralWidget);
	infoContainer->setFixedWidth(300);
	infoContainer->setStyleSheet(
		"QWidget {"
		"   background-color: #F5F5F5;"
		"   border-radius: 10px;"
		"   padding: 20px;"
		"   border: 1px solid black;"
		"}");

	auto* infoLayout = new QVBoxLayout(infoContainer);
	infoLayout->setSpacing(10);

	auto* fileNameLabel = new QLabel("Файл: " + QString::fromStdString("tinker.obj"), centralWidget);
	fileNameLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(fileNameLabel);

	auto* droneSizeLabel = new QLabel("Размеры дрона: 0 x 0 x 0", centralWidget);
	droneSizeLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(droneSizeLabel);

	auto* gridSizeLabel = new QLabel("Размер сетки: 0 x 0 x 0", centralWidget);
	gridSizeLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(gridSizeLabel);

	double shortestPathLength = 0;
	auto* pathLengthLabel = new QLabel("Длина кратчайшего пути: " + (shortestPathLength > 0 ? QString::number(shortestPathLength) : "Неизвестно"), centralWidget);
	pathLengthLabel->setStyleSheet("font-size: 14px; color: black;");
	infoLayout->addWidget(pathLengthLabel);

	auto* backButton = new QPushButton("Выход в меню", centralWidget);
	connect(backButton, &QPushButton::clicked, this, &Window::OnBackToMenu);
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

	mainLayout->addWidget(backButton, 0, Qt::AlignLeft);
	mainLayout->addSpacing(20);
	mainLayout->addWidget(infoContainer, 0, Qt::AlignRight);
	mainLayout->addStretch();
}

void Window::OnBackToMenu()
{
	ShowMainMenu();
}