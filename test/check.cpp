
#include "check.h"
//#include"util.h"
#include <iostream>
#include <vector>
#include <Windows.h>
namespace
{
	int s_checkCount=0;
	int s_errorCount=0;

	// �}���`�o�C�g�ւ̕ϊ�
	//  (�z�Q�Ƃ�h�����߁A������`)
	std::string toMultiByte(std::wstring str)
	{
		int const sz = WideCharToMultiByte(932, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
		std::vector<char>out(sz);
		WideCharToMultiByte(932, 0, str.c_str(), -1, &out[0], sz, NULL, NULL);
		return &out[0];
	}

}

void chk::isTrue(bool flag)
{
	isTrue(flag, "any error.");
}
void chk::isTrue(bool flag, std::wstring message)
{
	isTrue(flag, toMultiByte(message));
}
void chk::isTrue(bool flag, std::string message)
{
	++s_checkCount;
	if (!flag)
	{
		std::cout << message << '\n';
		++s_errorCount;
	}

}




