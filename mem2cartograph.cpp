#define _CRT_SECURE_NO_WARNINGS

#include "mem2cartograph.h"
#include "cartograph.h"
#include "mem.h"
#include "constant.h"
#include "util.h"

namespace
{
	//-----------------------------------------------------------------------------
	// 
	//-----------------------------------------------------------------------------
	std::vector<std::vector<mem::Tag> > getAreaTagsByY(
		std::vector<mem::Tag> tags) // あえて値で渡す。
	{

		using TagIte = std::vector < mem::Tag > ::const_iterator;

		// 中途半端に整合した並びを「たまたま」使うことを防止するための
		// 乱数並び替え
		std::random_shuffle(tags.begin(), tags.end());

		auto cmpIy = [](mem::Tag lhs, mem::Tag rhs){return lhs.iHigher().iY < rhs.iHigher().iY;  };
		auto cmpIx = [](mem::Tag lhs, mem::Tag rhs){return lhs.iHigher().iX < rhs.iHigher().iX;  };
		std::stable_sort(tags.begin(), tags.end(), cmpIx);
		std::stable_sort(tags.begin(), tags.end(), cmpIy);

		// iY値が等しいtagを仮に格納する。
		auto keyFunc = [](mem::Tag const& lhs, mem::Tag const& rhs)
		{
			return lhs.iHigher().iY == rhs.iHigher().iY;
		};
		return util::groupBy(tags, keyFunc);
	}
}
// memファイルから座標-標高配列の取得
std::vector<ca::Point >mem2ca::readFromFiles(std::vector<std::string> const& files)
{
	filesystem::path const basePath = mem::basePath(files);
	std::vector<mem::Tag> const tags = mem::getAreaTags(files);
		
	auto const extentLocation = mem::extentLocation(basePath, tags);
	auto const iBase = extentLocation.first;
	
	auto const areaTagsByY = getAreaTagsByY(tags);

	std::vector<ca::Point >points;

	for (auto const& tagsY : areaTagsByY)
	{
		auto const iYhigher = tagsY[0].iHigher().iY;
		std::vector<mem::Area>areas;
		for (auto const& tag:tagsY)
			areas.push_back(mem::Area(basePath / tag.path()));
	
		for (auto iYlower = 0; iYlower < mem::LEN; ++iYlower)
		{
			for (auto const& area : areas)
			{
				auto const iXhigher = area.tag().iHigher().iX;
				types::IYX const iHigher = { iYhigher, iXhigher };
				for (auto iXlower = 0; iXlower < mem::LEN; ++iXlower)
				{
					types::IYX const iLower = { iYlower, iXlower };
	
					short const h = area(iLower);
					if (!cn::isLand(h))continue;
	
					auto const iPlace = mem::iLocation(iHigher, iLower) - iBase;

					auto const iY = static_cast<unsigned short>(iPlace.iY);
					auto const iX = static_cast<unsigned short>(iPlace.iX);

					points.push_back({ iY,iX, h });
				}
			}
		}
	}
	points.shrink_to_fit();
	return points;
}
// 座標-標高配列から生ファイル出力
namespace
{
	char const valFirstY = -128;
	char const valFirstX =  127;
	char const valWord   = -127;
	short const limitDiffH = 126;
	auto deleteFp = [](FILE* fp){fclose(fp); };
}
void mem2ca::writeToFile(std::vector<ca::Point>const& points, std::string const& file)
{
//	// チェックサム計算
//	{
//		__int64 sum=0;
//		int count = 0;
//		std::ofstream fp("保存時sum.txt");
//		for (auto const& point : points)
//		{
//			sum = util::addSum(sum, count++, point.iY);
//			sum = util::addSum(sum, count++, point.iX);
//			sum = util::addSum(sum, count++, point.height);
//			fp << point.iY << ' ' << point.iX << ' ' << point.height << '\n';
//		}
////		std::ofstream fp("保存時sum.txt");
//		fp << "sum:" << sum << '\n';
//	}

	FILE* fp = fopen(file.c_str(), "wb");
	if (!fp)
		throw std::runtime_error(file + "を開けない");

	std::unique_ptr<FILE, decltype(deleteFp)>
		fp_holder(fp, deleteFp);
	// 全大きさを書きこむ
	{
		size_t const size = points.size();
		fwrite(&size, sizeof(size), 1, fp);
	}
	// 配列が空の時、終了
	if (points.empty())return;

	// 各値を書き込む
	for (size_t pos = 0; pos != points.size();++pos)
	{
		ca::Point const& point = points[pos];
		ca::Point const& past = points[pos - 1];

		if (pos == 0 || point.iY != past.iY)
		{
			fputc(valFirstY, fp);
			fwrite(&point.iY, sizeof(point.iY), 1, fp);// Yの初期値を書き込む
			fwrite(&point.iX, sizeof(point.iX), 1, fp);// Xの初期値を書き込む
			fwrite(&point.height, sizeof(point.height), 1, fp);// 標高を書き込む
			continue;
		}
		if (point.iX != past.iX+1)
		{
			fputc(valFirstX, fp);
			fwrite(&point.iX, sizeof(point.iX), 1, fp);// Xの初期値を書き込む
			fwrite(&point.height, sizeof(point.height), 1, fp);// 標高を書き込む
			continue;
		}
		short const h_diff = point.height - past.height;
		if (abs(h_diff) > limitDiffH)
		{
			fputc(valWord, fp);// 2バイト値フラグ
			fwrite(&point.height, sizeof(point.height), 1, fp);// 標高を書き込む
			continue;
		}
		fputc(h_diff, fp);// 標高差
	}

}
// ファイルから読み込み
std::vector<ca::Point> mem2ca::readFromFile(std::string const& file)
{
	FILE* fp = fopen(file.c_str(), "rb");
	if (!fp)
		throw std::runtime_error(file + "を開けない");
	std::unique_ptr<FILE, decltype(deleteFp)>
		fp_holder(fp, deleteFp);
	
	// 全大きさを読み込む
	size_t size;
	fread(&size, sizeof(size), 1, fp);
	std::vector<ca::Point>points(size);

	// 配列が空の時、終了
	if (points.empty())return points;
	// 読み込み用
	ca::Point point ={};
	// 各値を読み込む
	for (size_t pos = 0	;
		!feof(fp) && pos < size;
		++pos)
	{
		char const c = static_cast<char>(fgetc(fp));

		switch (c)
		{
		case valFirstY:
			fread(&point.iY, sizeof(point.iY), 1, fp);// Yの初期値を読み込む
			fread(&point.iX, sizeof(point.iX), 1, fp);// Xの初期値を読み込む
			fread(&point.height, sizeof(point.height), 1, fp);// 標高を読み込む
			break;
		case valFirstX:
			fread(&point.iX, sizeof(point.iX), 1, fp);// Xの初期値を読み込む
			fread(&point.height, sizeof(point.height), 1, fp);// 標高を読み込む
			break;
		case valWord:
			++point.iX;
			fread(&point.height, sizeof(point.height), 1, fp);// 標高を読み込む
			break;
		default:
			++point.iX;
			point.height += static_cast<short>(c);
			break;
		}
		points[pos] = point;
		//point.height;
	}
	// チェックサム計算
	//{
	//	__int64 sum = 0;
	//	int count = 0;
	//	for (auto const& point : points)
	//	{
	//		sum = util::addSum(sum, count++, point.iY);
	//		sum = util::addSum(sum, count++, point.iX);
	//		sum = util::addSum(sum, count++, point.height);
	//	}
	//	std::ofstream fp("読込時sum.txt");
	//	fp << "sum:" << sum << '\n';
	//}
	// チェックサム計算
	//{
	//	__int64 sum = 0;
	//	int count = 0;
	//	std::ofstream fp("読込時sum.txt");
	//	for (auto const& point : points)
	//	{
	//		sum = util::addSum(sum, count++, point.iY);
	//		sum = util::addSum(sum, count++, point.iX);
	//		sum = util::addSum(sum, count++, point.height);
	//		fp << point.iY << ' ' << point.iX << ' ' << point.height << '\n';
	//	}
	//	//		std::ofstream fp("読込時sum.txt");
	//	fp << "sum:" << sum << '\n';
	//}
	return points;

}
// 単一ファイルまたはmemファイルから読み込む
std::vector<ca::Point> mem2ca::readFromFileOrFiles(std::string const& file, std::vector<std::string> const& files)
{
	try
	{
		return readFromFile(file);
	}
	catch (std::exception&)
	{
		auto points = readFromFiles(files);
		writeToFile(points, file);
		//auto points2 = readFromFile(file);

		//if (points.size() != points2.size())
		//	throw std::runtime_error("おかし");
		//for (int n = 0; n < points.size();++n)
		//{
		//	if (points[n] != points2[n])
		//	{
		//		int const nFirst = std::max(0, n - 100);
		//		int const nLast = std::min<int>(points.size() - 1, n + 100);
		//		std::ofstream fp("照合.txt");
		//		for (int n2 = nFirst; n2 <= nLast;++n2)
		//		{
		//			char const flag = points[n2] == points2[n2] ? 'o' : 'x';
		//			fp << "W " << flag << ' ' << n2 << ' ' << points[n2].iY << ' ' << points[n2].iX << ' ' << points[n2].height << '\n';
		//			fp << "R " << flag << ' ' << n2 << ' ' << points2[n2].iY << ' ' << points2[n2].iX << ' ' << points2[n2].height << '\n';
		//		}
		//		return points;
		//	}
		//}
		return points;

	}
}

// 座標-標高配列から、標高系2段階地図の取得
ca::LandCa2 mem2ca::pointsToLandCa2(std::vector<ca::Point > const& points)
{
	ca::LandCa2 landCa2(-1);// cn::SEA_HEIGHT);

	for (__int32 iPoint = 0; iPoint != points.size();++iPoint)
		landCa2.setValue(points[iPoint].iPlace(), iPoint);

	return landCa2;

}

