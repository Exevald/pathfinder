#include "View/CWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	auto viewModel = std::make_unique<CGridViewModel>();

	CWindow window(nullptr, std::move(viewModel));
	window.resize(1920, 1080);
	window.show();

	return QApplication::exec();
}