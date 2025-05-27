#include "View/Window.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	auto viewModel = std::make_unique<GridViewModel>();

	Window window(nullptr, std::move(viewModel));
	window.resize(1600, 900);
	window.show();

	return QApplication::exec();
}