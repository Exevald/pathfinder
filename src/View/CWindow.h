#pragma once

#include "CGridViewModel.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Window;
}
QT_END_NAMESPACE

class CWindow : public QMainWindow
{
public:
	explicit CWindow(QWidget* parent = nullptr, std::unique_ptr<CGridViewModel> viewModel = nullptr);

private slots:
	void OnOpenCADFile();
	void OnDraw3DSpace();
	void OnBackToMenu();

private:
	void ShowMainMenu();
	void Draw3DSpace();

	std::unique_ptr<QGraphicsView> m_graphicsView;
	std::unique_ptr<QGraphicsScene> m_scene;
	std::unique_ptr<CGridViewModel> m_gridViewModel;
};