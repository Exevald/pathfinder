#include "View/CWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	qputenv("QT_QPA_PLATFORM", "wayland");
	QApplication app(argc, argv);

	CWindow window;
	window.resize(1366, 768);
	window.show();

	return QApplication::exec();
}