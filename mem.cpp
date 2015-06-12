#define _CRT_SECURE_NO_WARNINGS
#include "mem.h"
#include "util.h"
#include "constant.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <ctype.h>

#include <boost/algorithm/cxx11/any_of.hpp>

namespace algorithm = boost::algorithm;

//===================================================================
//  定数
//===================================================================
namespace
{
}
//===================================================================
//  地図タグ関係
//===================================================================
namespace
{
	//-------------------------------------------------------------------
	// Y座標は負とする。(北を小さな値とするため)
	//-------------------------------------------------------------------
	int iY(int indexTag)
	{
		int const upper = indexTag / 10000;
		int const lower = (indexTag / 10) % 10;
		return -(upper * 8 + lower);
	}
	//-------------------------------------------------------------------
	// 
	//-------------------------------------------------------------------
	int iX(int indexTag)
	{
		int const upper = (indexTag / 100) % 100;
		int const lower = (indexTag / 1) % 10;
		return upper * 8 + lower;
	}
}
//-------------------------------------------------------------------
// 
//-------------------------------------------------------------------
mem::Tag::Tag() :Tag(0)
{
}
//-------------------------------------------------------------------
// 
//-------------------------------------------------------------------
mem::Tag::Tag(int index) :index_(index)
{
}
//-------------------------------------------------------------------
// 
//-------------------------------------------------------------------
mem::Tag::Tag(filesystem::path const& path) :Tag
	( boost::lexical_cast<int>(path.stem().string()) )
{
}
//-------------------------------------------------------------------
// "1234\123456.MEM"という感じのパスを返す
//-------------------------------------------------------------------
filesystem::path mem::Tag::path()const
{
	return boost::str(boost::format("%04d\\%06d.MEM") % (index() / 100) % index());
}
//-------------------------------------------------------------------
// 
//-------------------------------------------------------------------
types::IYX mem::Tag::iHigher() const
{
	return{ iY(index()), iX(index()) };
}

////-------------------------------------------------------------------
//// Y座標は負とする。(北を小さな値とするため)
////-------------------------------------------------------------------
//int mem::Tag::iY()const
//{
//	int const upper = index() / 10000;
//	int const lower = (index() / 10) % 10;
//	return -(upper * 8 + lower);
//}
////-------------------------------------------------------------------
//// 
////-------------------------------------------------------------------
//int mem::Tag::iX()const
//{
//	int const upper = (index() / 100) % 100;
//	int const lower = (index() / 1) % 10;
//	return upper * 8 + lower;
//}
//---------------------------------------------------------------------------
// タグ取得
//---------------------------------------------------------------------------
std::vector<mem::Tag>mem::getAreaTags(std::vector<std::string> const& areaFiles)
{
	std::vector<mem::Tag> tags(areaFiles.size());
	std::transform(areaFiles.begin(), areaFiles.end(), tags.begin(), mem::Tag::fromFile);
	return tags;
}
//---------------------------------------------------------------------------
// 2次メッシュ領域座標範囲取得
//  出力: ((iYa最小,iXa最小),(iYa最大,iXa最大) )
//---------------------------------------------------------------------------
std::pair<types::IYX, types::IYX  >	mem::extentHigher(std::vector<mem::Tag> const& tags)
{
	types::IYX iBase = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
	types::IYX iLimit = { std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

	for (auto const tag : tags)
	{
		iBase = types::min(iBase, tag.iHigher());
		iLimit = types::max(iLimit, tag.iHigher());
	}

	return std::make_pair(iBase, iLimit);
}
//---------------------------------------------------------------------------
// 50m×50m領域座標範囲取得(マージン付)
//  出力: ((iYp最小,iXp最小),(iYp最大,iXp最大) )
//---------------------------------------------------------------------------
std::pair<types::IYX, types::IYX  >	mem::extentLocation(
	filesystem::path const& basePath,
	std::vector<mem::Tag> const& tags)
{
	auto const extentH = extentHigher(tags);
	auto const minH = extentH.first ;
	auto const maxH = extentH.second;

	int iYminP = mem::LEN - 1;
	int iXminP = mem::LEN - 1;
	int iYmaxP = 0;
	int iXmaxP = 0;

	for (auto tag : tags)
	{
		if (tag.iHigher().iY == minH.iY)
		{
			mem::Area const area(basePath / tag.path());
			for (int iYp = 0; iYp < mem::LEN; ++iYp)
				if (algorithm::any_of(area.lineY(iYp), cn::isLand))
				{
					iYminP = std::min(iYminP, iYp);
					break;
				}
		}
		if (tag.iHigher().iX == minH.iX)
		{
			mem::Area const area(basePath / tag.path());
			for (int iXp = 0; iXp < mem::LEN; ++iXp)
			{
				if (algorithm::any_of(area.lineX(iXp), cn::isLand))
				{
					iXminP = std::min(iXminP, iXp);
					break;
				}
			}
		}
		if (tag.iHigher().iY == maxH.iY)
		{
			mem::Area const area(basePath / tag.path());
			for (int iYp = mem::LEN - 1; 0 <= iYp; --iYp)
				if (algorithm::any_of(area.lineY(iYp), cn::isLand))
				{
					iYmaxP = std::max(iYmaxP, iYp);
					break;
				}
		}
		if (tag.iHigher().iX == maxH.iX)
		{
			mem::Area const area(basePath / tag.path());
			for (int iXp = mem::LEN - 1; 0 <= iXp; --iXp)
				if (algorithm::any_of(area.lineX(iXp), cn::isLand))
				{
					iXmaxP = std::max(iXmaxP, iXp);
					break;
				}
		}
	}

	auto const iYXmin = mem::iLocation(minH, { iYminP, iXminP });
	auto const iYXmax = mem::iLocation(maxH, { iYmaxP, iXmaxP });

	// 大きさの検査
	if ((1 << 16) <= iYXmax.iY - iYXmin.iY)throw std::runtime_error("地図が大きすぎる(y)");
	if ((1 << 16) <= iYXmax.iX - iYXmin.iX)throw std::runtime_error("地図が大きすぎる(x)");

	return std::make_pair(iYXmin, iYXmax);
}
//===================================================================
//  地図メモリ関係
//===================================================================
// コメントアウトして19秒ほど
#define NEW_NUM 
//#define WITH_LOG
namespace
{
	// 文字の適切性を判断
	bool isValidNumCp(char c){return '0' <= c && c <= '9';}
	bool isValidNumCm(char c){ return isValidNumCp(c) || c == '-'; }
	bool isValidNum(char const* str)
	{
		if (!isValidNumCm(str[0]))return false;
		if (!isValidNumCp(str[1]))return false;
		if (!isValidNumCp(str[2]))return false;
		if (!isValidNumCp(str[3]))return false;
		return true;
	}
	// 文字を選択
	char selectNumCm(char c1,char c2){return isValidNumCm(c1) ? c1 : c2;}
	char selectNumCp(char c1, char c2){return isValidNumCp(c1) ? c1 : c2;}
	// 文字を変換
	short convert(char* str1, char const* str2)
	{
		if (!isValidNum(str1))
		{
			str1[0] = selectNumCm(str1[0], str2[0]);
			str1[1] = selectNumCp(str1[1], str2[1]);
			str1[2] = selectNumCp(str1[2], str2[2]);
			str1[3] = selectNumCp(str1[3], str2[3]);
		}
		char const c0 = str1[0];
		short const n1 = str1[1] - '0';
		short const n2 = str1[2] - '0';
		short const n3 = str1[3] - '0';
		return	c0 == '-' ? -(n1 * 100 + n2 * 10 + n3) : (c0-'0') * 1000 + n1 * 100 + n2 * 10 + n3;
	}
}
mem::Area::Area(filesystem::path const& path)
{
	read(path);
}
//-------------------------------------------------------------------
//  
//-------------------------------------------------------------------
void mem::Area::read(filesystem::path const& path)
{
	int const LINE_N = 1011;
	char buffer[LINE_N];// *201];

	std::unique_ptr<FILE, decltype(&fclose)>
		fp(fopen(path.string().c_str(), "rb"), fclose);

	{
		size_t const len1 = fread_s(buffer, sizeof(buffer), 1, sizeof(buffer), fp.get());
		if (len1 != LINE_N)
			throw std::runtime_error("Wrong length");
	}
	// タグの初期化
	{
		int indexTag;
		if (!boost::conversion::try_lexical_convert<int>(buffer, 6, indexTag))
			throw std::runtime_error(buffer);
		tag_ = mem::Tag(indexTag);
	}

	char const* pre1 = "0000";
	for (int iY = 0; iY < LEN; ++iY)
	{
		{
			size_t const len1 = fread_s(buffer, sizeof(buffer), 1, sizeof(buffer), fp.get());
			if (len1 != LINE_N)
				throw std::runtime_error("Wrong length");
		}
		// 標高の初期化
		for (int iX = 0; iX < LEN; ++iX)
		{
			char* ptr = &buffer[9 + iX * 5];
			data_[iY*LEN + iX] = convert(ptr, pre1);
			pre1 = ptr;
		}
	}
}

//===================================================================
//  
//===================================================================
//-------------------------------------------------------------------
//  地図ファイルパスの一覧を取得
//-------------------------------------------------------------------
namespace
{
	bool select_mem(filesystem::directory_entry const& d)
	{
		if (d.status().type() != filesystem::file_type::regular_file)
			return false;
		filesystem::path const path = d.path();
		std::string const ext = path.extension().string();
		std::string const mem = ".MEM";
		if (!boost::iequals(ext, mem))return false;

		// digit4 : 3624\362420.MEMの、3624の部分
		// digit6 : 3624\362420.MEMの、362420の部分
		std::string const digit4 = path.parent_path().stem().string();
		std::string const digit6 = path.stem().string();
		if (digit4.length() != 4)return false;
		if (digit6.length() != 6)return false;

		if (!boost::all(digit4, boost::is_digit()))return false;
		if (!boost::all(digit6, boost::is_digit()))return false;

		if (digit6.substr(0, 4) != digit4)return false;

		return true;
	}
}
//-------------------------------------------------------------------
// 
//-------------------------------------------------------------------
std::vector<std::string> mem::areaFiles()
{
	std::vector<std::string> paths;
	auto dmap = util::searchLastPath(L"dmap");
	if (!dmap)
		return paths;

	std::string const top = dmap->string();

	for (auto d : filesystem::recursive_directory_iterator(top))
		if (select_mem(d))
			paths.push_back(d.path().string());
	return paths;
}
//-------------------------------------------------------------------
//  
//  
//-------------------------------------------------------------------
filesystem::path mem::basePath(std::vector<std::string> const& areaFiles)
{
	if (areaFiles.empty())
		return "";

	auto const basePath =
		filesystem::path(areaFiles.front()).parent_path().parent_path();

	{
		auto const basePathString = basePath.string();
		for (auto const file : areaFiles)
		{
			filesystem::path const ppPath =
				filesystem::path(file).parent_path().parent_path();
			if (!boost::iequals(basePathString, ppPath.string()))
				throw std::runtime_error("Invalid Base Path:" + basePathString);

		}
	}
	return basePath;
}
//-------------------------------------------------------------------
// 座標合成
//   領域Areaの位置を示す上位座標iUpperと、
//   領域area内の位置を示す下位座標iLowerを組み合わせて、
//   総合座標iPlaceを生成する。
//-------------------------------------------------------------------
int mem::iLocation(int iUpper, int iLower)
{
	return iUpper*LEN + iLower;
}
//-------------------------------------------------------------------
// 
//-------------------------------------------------------------------
types::IYX mem::iLocation(types::IYX iUpper, types::IYX iLower)
{
//	return types::IYX(iA.iY()*LEN + iP.iY(), iA.iX()*LEN + iP.iX());
	return{ iUpper.iY*LEN + iLower.iY, iUpper.iX*LEN + iLower.iX };
}
