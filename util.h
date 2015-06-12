//-----------------------------------------------------------------------
// �����ʌ��ꃉ���^�C����L���ɂ���B
//-----------------------------------------------------------------------

#pragma once
//#include"files.h"
//-----------------------------------------------------------------------
// ���J��`
//-----------------------------------------------------------------------
#include <string>
#include <vector>
#include <boost\optional.hpp>
#include <boost\filesystem.hpp>

//#include <fstream> // �����p
//#include <numeric> // �����p

namespace util
{
	// �����R�[�h�ϊ�
	//MultiByteToWideChar
	std::string toMultiByte(std::wstring const& str);
	std::wstring toWideChar(std::string const& str);
	// ������̒u��
	std::wstring replaceString(
		std::wstring const& in,
		std::wstring const& from,
		std::wstring const& to,
		bool recurse = false);


	// �����܂��͏�ʂ̃f�B���N�g���̉��ʃf�B���N�g���ŁA
	// ����̕�����ƈ�v����f�B���N�g������������B
	boost::optional<boost::filesystem::path> 
		searchLastPath(boost::filesystem::path const& name, boost::filesystem::path const& current);
	boost::optional<boost::filesystem::path> 
		searchLastPath(boost::filesystem::path const& name);


	// Haskel��groupBy�Ɠ����̏���
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
	// �x�N�g���傫����r
	template<typename T>
	bool compareSize(std::vector<T> const& lhs, std::vector<T> const& rhs){ return lhs.size() < rhs.size(); }
	// �`�F�b�N�T���v�Z
	inline __int64 addSum(__int64 in, __int64 a, __int64 b)
	{
		return in + a * b;
	}
}

