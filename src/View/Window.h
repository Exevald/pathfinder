#pragma once

#include "../ViewModel/GridViewModel.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Window;
}
QT_END_NAMESPACE

class Window : public QMainWindow
{
	Q_OBJECT

public:
	explicit Window(QWidget* parent = nullptr, std::unique_ptr<GridViewModel> viewModel = nullptr);

private slots:
	void OnOpenCADFile();
	void OnDraw3DSpace();
	void OnBackToMenu();

private:
	void ShowMainMenu();
	void Draw3DSpace();

	std::unique_ptr<QGraphicsView> m_graphicsView;
	std::unique_ptr<QGraphicsScene> m_scene;
	std::unique_ptr<GridViewModel> m_gridViewModel;
};