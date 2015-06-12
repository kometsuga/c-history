// test.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include<string>
#include<iostream>
#include<cassert>

#include "..\util.h"
#include "check.h"



int main(int /*argc*/,char const* /*argv*/[])
{
	try
	{
		std::wstring const str = L"c:\\\\\\\\x\\";
		chk::isTrue(util::replaceString(str, L"\\\\", L"\\") == L"c:\\\\x\\");
		chk::isTrue(util::replaceString(str, L"\\\\", L"\\") == L"c:\\\\xx\\", L"www");

		std::cout << util::toMultiByte(L"���o �C�g����\n");
		std::wcout << L"example_end\n";

	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
	catch (...)
	{
		std::cout << "any error\n";
	}
}
