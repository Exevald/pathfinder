#pragma once

#include "../ViewModel/GridViewModel.h"
#include "Space.h"
#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Window;
}
QT_END_NAMESPACE

class Window final : public QMainWindow
{
	Q_OBJECT

public:
	explicit Window(QWidget* parent = nullptr, std::unique_ptr<GridViewModel> viewModel = nullptr);

private slots:
	void OnOpenCADFile();
	void OnBackToMenu();

private:
	void ShowMainMenu();
	void Draw3DSpace();

	std::unique_ptr<QGraphicsView> m_graphicsView;
	std::unique_ptr<QGraphicsScene> m_scene;
	std::unique_ptr<GridViewModel> m_gridViewModel;
	std::unique_ptr<Space> m_space;
};