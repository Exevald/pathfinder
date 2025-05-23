#include "View/CWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	auto viewModel = std::make_unique<CGridViewModel>();

	CWindow window(nullptr, std::move(viewModel));
	window.resize(1600, 900);
	window.show();

	return QApplication::exec();
}