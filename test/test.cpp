// test.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
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

		std::cout << util::toMultiByte(L"多バ イト文字\n");
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
