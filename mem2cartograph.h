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

	// mem�t�@�C��������W-�W���z��̎擾
	std::vector<ca::Point >readFromFiles(std::vector<std::string> const& files);

	// ���W-�W���z�񂩂琶�t�@�C���o��
	void writeToFile(std::vector<ca::Point>const& points, std::string const& file);

	// �P��t�@�C��(�L���b�V��)����ǂݍ���
	std::vector<ca::Point> readFromFile(std::string const& file);

	// �P��t�@�C���܂���mem�t�@�C������ǂݍ���
	std::vector<ca::Point> readFromFileOrFiles(std::string const& file, std::vector<std::string> const& files);


	// ���W-�W���z�񂩂�A�W���n2�i�K�n�}�̎擾
	ca::LandCa2 pointsToLandCa2(std::vector<ca::Point > const& points);



}