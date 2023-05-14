#include "wdepot.h"

int main()
{
	// поддержка ввода-вывода на кириллице
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	
	avhw14::WordDepot wd;
	wd.textProcessing();

	std::cin.get();
	return 0;
}