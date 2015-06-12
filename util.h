//-----------------------------------------------------------------------
// ※共通言語ランタイムを有効にする。
//-----------------------------------------------------------------------

#pragma once
//#include"files.h"
//-----------------------------------------------------------------------
// 公開定義
//-----------------------------------------------------------------------
#include <string>
#include <vector>
#include <boost\optional.hpp>
#include <boost\filesystem.hpp>

//#include <fstream> // 調査用
//#include <numeric> // 調査用

namespace util
{
	// 文字コード変換
	//MultiByteToWideChar
	std::string toMultiByte(std::wstring const& str);
	std::wstring toWideChar(std::string const& str);
	// 文字列の置換
	std::wstring replaceString(
		std::wstring const& in,
		std::wstring const& from,
		std::wstring const& to,
		bool recurse = false);


	// 自分または上位のディレクトリの下位ディレクトリで、
	// 特定の文字列と一致するディレクトリを検索する。
	boost::optional<boost::filesystem::path> 
		searchLastPath(boost::filesystem::path const& name, boost::filesystem::path const& current);
	boost::optional<boost::filesystem::path> 
		searchLastPath(boost::filesystem::path const& name);


	// HaskelのgroupByと同等の処理
	template<typename T, typename KeyFunc>
	std::vector<std::vector<T> > groupBy(std::vector<T> const& xs, KeyFunc keyFunc)
	{
		std::vector<std::vector<T> > lines;
		if (xs.empty())return lines;
		std::vector<T> line = {xs[0]};
		for (size_t iX = 1; iX != xs.size(); ++iX)
		{
			auto const x = xs[iX];
			if (keyFunc(line.front(), x))
			{
				line.push_back(x);
			}
			else
			{
				lines.push_back(line);
				line = { x };
			}
		}
		if (!line.empty())lines.push_back(line);
		return lines;
	}
	// ベクトル大きさ比較
	template<typename T>
	bool compareSize(std::vector<T> const& lhs, std::vector<T> const& rhs){ return lhs.size() < rhs.size(); }
	// チェックサム計算
	inline __int64 addSum(__int64 in, __int64 a, __int64 b)
	{
		return in + a * b;
	}
}

