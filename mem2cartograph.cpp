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
		std::vector<mem::Tag> tags) // �����Ēl�œn���B
	{

		using TagIte = std::vector < mem::Tag > ::const_iterator;

		// ���r���[�ɐ����������т��u���܂��܁v�g�����Ƃ�h�~���邽�߂�
		// �������ёւ�
		std::random_shuffle(tags.begin(), tags.end());

		auto cmpIy = [](mem::Tag lhs, mem::Tag rhs){return lhs.iHigher().iY < rhs.iHigher().iY;  };
		auto cmpIx = [](mem::Tag lhs, mem::Tag rhs){return lhs.iHigher().iX < rhs.iHigher().iX;  };
		std::stable_sort(tags.begin(), tags.end(), cmpIx);
		std::stable_sort(tags.begin(), tags.end(), cmpIy);

		// iY�l��������tag�����Ɋi�[����B
		auto keyFunc = [](mem::Tag const& lhs, mem::Tag const& rhs)
		{
			return lhs.iHigher().iY == rhs.iHigher().iY;
		};
		return util::groupBy(tags, keyFunc);
	}
}
// mem�t�@�C��������W-�W���z��̎擾
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
// ���W-�W���z�񂩂琶�t�@�C���o��
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
//	// �`�F�b�N�T���v�Z
//	{
//		__int64 sum=0;
//		int count = 0;
//		std::ofstream fp("�ۑ���sum.txt");
//		for (auto const& point : points)
//		{
//			sum = util::addSum(sum, count++, point.iY);
//			sum = util::addSum(sum, count++, point.iX);
//			sum = util::addSum(sum, count++, point.height);
//			fp << point.iY << ' ' << point.iX << ' ' << point.height << '\n';
//		}
////		std::ofstream fp("�ۑ���sum.txt");
//		fp << "sum:" << sum << '\n';
//	}

	FILE* fp = fopen(file.c_str(), "wb");
	if (!fp)
		throw std::runtime_error(file + "���J���Ȃ�");

	std::unique_ptr<FILE, decltype(deleteFp)>
		fp_holder(fp, deleteFp);
	// �S�傫������������
	{
		size_t const size = points.size();
		fwrite(&size, sizeof(size), 1, fp);
	}
	// �z�񂪋�̎��A�I��
	if (points.empty())return;

	// �e�l����������
	for (size_t pos = 0; pos != points.size();++pos)
	{
		ca::Point const& point = points[pos];
		ca::Point const& past = points[pos - 1];

		if (pos == 0 || point.iY != past.iY)
		{
			fputc(valFirstY, fp);
			fwrite(&point.iY, sizeof(point.iY), 1, fp);// Y�̏����l����������
			fwrite(&point.iX, sizeof(point.iX), 1, fp);// X�̏����l����������
			fwrite(&point.height, sizeof(point.height), 1, fp);// �W������������
			continue;
		}
		if (point.iX != past.iX+1)
		{
			fputc(valFirstX, fp);
			fwrite(&point.iX, sizeof(point.iX), 1, fp);// X�̏����l����������
			fwrite(&point.height, sizeof(point.height), 1, fp);// �W������������
			continue;
		}
		short const h_diff = point.height - past.height;
		if (abs(h_diff) > limitDiffH)
		{
			fputc(valWord, fp);// 2�o�C�g�l�t���O
			fwrite(&point.height, sizeof(point.height), 1, fp);// �W������������
			continue;
		}
		fputc(h_diff, fp);// �W����
	}

}
// �t�@�C������ǂݍ���
std::vector<ca::Point> mem2ca::readFromFile(std::string const& file)
{
	FILE* fp = fopen(file.c_str(), "rb");
	if (!fp)
		throw std::runtime_error(file + "���J���Ȃ�");
	std::unique_ptr<FILE, decltype(deleteFp)>
		fp_holder(fp, deleteFp);
	
	// �S�傫����ǂݍ���
	size_t size;
	fread(&size, sizeof(size), 1, fp);
	std::vector<ca::Point>points(size);

	// �z�񂪋�̎��A�I��
	if (points.empty())return points;
	// �ǂݍ��ݗp
	ca::Point point ={};
	// �e�l��ǂݍ���
	for (size_t pos = 0	;
		!feof(fp) && pos < size;
		++pos)
	{
		char const c = static_cast<char>(fgetc(fp));

		switch (c)
		{
		case valFirstY:
			fread(&point.iY, sizeof(point.iY), 1, fp);// Y�̏����l��ǂݍ���
			fread(&point.iX, sizeof(point.iX), 1, fp);// X�̏����l��ǂݍ���
			fread(&point.height, sizeof(point.height), 1, fp);// �W����ǂݍ���
			break;
		case valFirstX:
			fread(&point.iX, sizeof(point.iX), 1, fp);// X�̏����l��ǂݍ���
			fread(&point.height, sizeof(point.height), 1, fp);// �W����ǂݍ���
			break;
		case valWord:
			++point.iX;
			fread(&point.height, sizeof(point.height), 1, fp);// �W����ǂݍ���
			break;
		default:
			++point.iX;
			point.height += static_cast<short>(c);
			break;
		}
		points[pos] = point;
		//point.height;
	}
	// �`�F�b�N�T���v�Z
	//{
	//	__int64 sum = 0;
	//	int count = 0;
	//	for (auto const& point : points)
	//	{
	//		sum = util::addSum(sum, count++, point.iY);
	//		sum = util::addSum(sum, count++, point.iX);
	//		sum = util::addSum(sum, count++, point.height);
	//	}
	//	std::ofstream fp("�Ǎ���sum.txt");
	//	fp << "sum:" << sum << '\n';
	//}
	// �`�F�b�N�T���v�Z
	//{
	//	__int64 sum = 0;
	//	int count = 0;
	//	std::ofstream fp("�Ǎ���sum.txt");
	//	for (auto const& point : points)
	//	{
	//		sum = util::addSum(sum, count++, point.iY);
	//		sum = util::addSum(sum, count++, point.iX);
	//		sum = util::addSum(sum, count++, point.height);
	//		fp << point.iY << ' ' << point.iX << ' ' << point.height << '\n';
	//	}
	//	//		std::ofstream fp("�Ǎ���sum.txt");
	//	fp << "sum:" << sum << '\n';
	//}
	return points;

}
// �P��t�@�C���܂���mem�t�@�C������ǂݍ���
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
		//	throw std::runtime_error("������");
		//for (int n = 0; n < points.size();++n)
		//{
		//	if (points[n] != points2[n])
		//	{
		//		int const nFirst = std::max(0, n - 100);
		//		int const nLast = std::min<int>(points.size() - 1, n + 100);
		//		std::ofstream fp("�ƍ�.txt");
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

// ���W-�W���z�񂩂�A�W���n2�i�K�n�}�̎擾
ca::LandCa2 mem2ca::pointsToLandCa2(std::vector<ca::Point > const& points)
{
	ca::LandCa2 landCa2(-1);// cn::SEA_HEIGHT);

	for (__int32 iPoint = 0; iPoint != points.size();++iPoint)
		landCa2.setValue(points[iPoint].iPlace(), iPoint);

	return landCa2;

}

