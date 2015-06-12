#include "util.h"
//-----------------------------------------------------------------------
// 
//-----------------------------------------------------------------------
#include <Windows.h>
#include <vector>
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#pragma comment(lib, "shlwapi.lib")
//#include<sstream>
//-----------------------------------------------------------------------
// マルチバイトへの変換
//-----------------------------------------------------------------------
std::string util::toMultiByte(std::wstring const& str)
{
	int const sz = WideCharToMultiByte(932, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::vector<char>out(sz+1);
	WideCharToMultiByte(932, 0, str.c_str(), -1, &out[0], sz, nullptr, nullptr);
	return &out[0];

}
//-----------------------------------------------------------------------
// 2バイト文字への変換
//-----------------------------------------------------------------------
std::wstring util::toWideChar(std::string const& str)
{
	int const sz = MultiByteToWideChar(932, MB_PRECOMPOSED, &str[0], -1, nullptr, 0);

	std::vector<wchar_t>out(sz + 1);

	MultiByteToWideChar(932, MB_PRECOMPOSED, &str[0], -1, &out[0], sz);

	return &out[0];
}

//-----------------------------------------------------------------------
// 文字列の置換
//-----------------------------------------------------------------------
std::wstring util::replaceString(
	std::wstring const& in,
	std::wstring const& from,
	std::wstring const& to,
	bool recurse )
{
	std::wstring result = in;
	for (size_t pos = result.find(from);
		pos != std::string::npos;
		pos = result.find(from, recurse ? 0 : pos + to.length())
		)
	{
		result.replace(pos, from.length(), to);
	}
	return result;
}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
boost::optional<boost::filesystem::path> util::searchLastPath(
	boost::filesystem::path const& name, 
	boost::filesystem::path const& current)
{
	auto const next = current / name;
	if (boost::filesystem::is_directory(next))
		return next;
	auto const upper = current.parent_path();

	return upper.empty() ? boost::none : searchLastPath(name, upper);

	
}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
boost::optional<boost::filesystem::path> util::searchLastPath(
	boost::filesystem::path const& name)
{
	return searchLastPath(name, boost::filesystem::current_path());
}


