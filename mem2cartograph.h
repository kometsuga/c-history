#pragma once

#include "types.h"
//#include <array>
//#include <memory>
//#include <vector>
#include <boost/filesystem.hpp>
//#include <boost/shared_array.hpp>
#include "cartograph.h"

namespace filesystem = boost::filesystem;

namespace mem2ca
{

	// memファイルから座標-標高配列の取得
	std::vector<ca::Point >readFromFiles(std::vector<std::string> const& files);

	// 座標-標高配列から生ファイル出力
	void writeToFile(std::vector<ca::Point>const& points, std::string const& file);

	// 単一ファイル(キャッシュ)から読み込む
	std::vector<ca::Point> readFromFile(std::string const& file);

	// 単一ファイルまたはmemファイルから読み込む
	std::vector<ca::Point> readFromFileOrFiles(std::string const& file, std::vector<std::string> const& files);


	// 座標-標高配列から、標高系2段階地図の取得
	ca::LandCa2 pointsToLandCa2(std::vector<ca::Point > const& points);



}