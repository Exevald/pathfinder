#pragma once

#include <QApplication>
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
};