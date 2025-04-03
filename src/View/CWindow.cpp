#include "CWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

CWindow::CWindow(QWidget* parent)
	: QMainWindow(parent)
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
		"	padding: 10px 15px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #45A049;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #3E8E41;"
		"}");

	auto* exitButton = new QPushButton("Выход", this);
	connect(exitButton, &QPushButton::clicked, this, []() { QApplication::quit(); });
	exitButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #F44336;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 5px;"
		"	padding: 10px 15px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #D32F2F;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #C62828;"
		"}");

	layout->addStretch();
	layout->addWidget(loadCADButton, 0, Qt::AlignCenter);
	layout->addWidget(exitButton, 0, Qt::AlignCenter);
	layout->addStretch();

	layout->setSpacing(20);
}

void CWindow::OnOpenCADFile()
{
	auto filePath = QFileDialog::getOpenFileName(
		this,
		"Выберите CAD-файл",
		"",
		"CAD Files (*.cad *.dwg *.dxf);;All Files (*)");

	if (!filePath.isEmpty())
	{
		QMessageBox::information(this, "Файл выбран", "Путь к файлу: " + filePath);
	}
}