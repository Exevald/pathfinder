#pragma once

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
	explicit CWindow(QWidget* parent = nullptr);

private slots:
	void OnOpenCADFile();
	void OnDraw3DSpace();
	void OnBackToMenu();

private:
	void ShowMainMenu();
	void Draw3DSpace();

	QGraphicsView* m_graphicsView;
	QGraphicsScene* m_scene;
};