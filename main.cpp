//// #define GLFW_DLL
//
//// #include<Windows.h> // もし必要なら、<glfw3.h>の前に入れること。

#include "draw.h"
#include "util.h"
#include "mem.h"
//#include "constant.h"
#include "cartograph.h"
#include "mem2cartograph.h"

#include <exception>
#include <fstream>
#include <Windows.h>
//#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/timer.hpp>
#include "DrawMain.h"

namespace algorithm = boost::algorithm;

#undef RGB
#undef min
#undef max
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
namespace
{
	//---------------------------------------------------------------------------
	// 
	//---------------------------------------------------------------------------
	void exec2()
	{
		boost::timer t;

		std::string const binFile = "mem.bin";
		std::vector<std::string> const files = mem::areaFiles();


		auto const tResult1 = t.elapsed();

		auto const points = mem2ca::readFromFileOrFiles(binFile, files);
		//auto const points = [&]
		//{
		//	try
		//	{
		//		return mem2ca::readFromFile(binFile);
		//	}
		//	catch (std::exception& )
		//	{
		//		auto points = mem2ca::readFromFiles(files);
		//		mem2ca::writeToFile(points, binFile);
		//		return points;

		//	}
		//}();


//		auto const points = mem2ca::readFromFiles(files);


		auto const tResult2 = t.elapsed();
		auto const landCa2 = mem2ca::pointsToLandCa2(points);

		auto const tResult3 = t.elapsed();

		//
		auto const title = boost::format("(nY,nX)=(%d,%d),[sec](%f %f %f) ")
			% landCa2.iMax().iY
			% landCa2.iMax().iX
			% tResult1 % tResult2 % tResult3 ;
		//			% sizeof std::pair < types::IYX, short > ;
		//			% heightCa2.nPoints();
		//			% sizeof areas1;
		DrawMain drawMain(points,landCa2, title.str());

//		auto const tResult4 = t.elapsed();
		// 37.05 40.86
		// 32.9  36.4 42.9
		// 6.75 8.90 13.2

		draw::execute(drawMain);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
int main(int /*argc*/, char const* /*argv*/ [])
{
	// 二重起動防止用
	auto deleteMutex = [](HANDLE hMutex){::CloseHandle(hMutex); };
	std::unique_ptr<void, decltype(deleteMutex)>
		hMutex1 (::CreateMutex(nullptr, FALSE, L"ikHistory"), deleteMutex);
//	std::unique_ptr<void, decltype(::CloseHandle)>
//		hMutex1(::CreateMutex(nullptr, FALSE, L"ikHistory"), ::CloseHandle);
	//	HANDLE hMutex = ::CreateMutex(NULL, FALSE, L"ikHistory");
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return 0;
	}
	std::string const logName = "エラーログ.txt";
	//DeleteFile(logName.c_str());
#ifndef _DEBUG
	try
	{
#endif
		exec2();
#ifndef _DEBUG
	}
	catch (std::exception& e)
	{
		MessageBox(nullptr, util::toWideChar(e.what()).c_str(), L"エラーメッセージ", 0);
		std::ofstream fp(logName);
		fp << e.what() << "\n";
	}
	catch (...)
	{
		MessageBox(nullptr, L"不明のエラー", L"エラーメッセージ", 0);
	}
#endif


}

