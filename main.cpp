#include "wdepot.h"

int main()
{
	// ��������� �����-������ �� ���������
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	
	avhw14::WordDepot wd;
	wd.textProcessing();

	std::cin.get();
	return 0;
}