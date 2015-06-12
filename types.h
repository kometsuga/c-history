#pragma once
#include <algorithm>
#undef min
#undef max
namespace types
{
	// •½–Ê‘å‚«‚³
	struct Size2
	{
		int y;
		int x;
		int area()const{ return y*x; }
	};
	// •½–ÊÀ•W
	struct IYX
	{
		int iY;
		int iX;
		IYX() = default;
		IYX(int y, int x) :iY(y), iX(x){}
	};

	inline IYX min(IYX const& lhs, IYX const& rhs)
	{
		return{ std::min(lhs.iY, rhs.iY), std::min(lhs.iX, rhs.iX) };
	}
	inline IYX max(IYX const& lhs, IYX const& rhs)
	{
		return{ std::max(lhs.iY, rhs.iY), std::max(lhs.iX, rhs.iX) };
	}
	inline IYX operator+(IYX const& lhs, IYX const& rhs)
	{
		return { lhs.iY + rhs.iY, lhs.iX + rhs.iX };
	}
	inline IYX operator-(IYX const& lhs, IYX const& rhs)
	{
		return { lhs.iY - rhs.iY, lhs.iX - rhs.iX };
	}
	inline bool operator==(IYX const& lhs, IYX const& rhs)
	{
		return lhs.iY == rhs.iY && lhs.iX == rhs.iX;
	}
	inline bool operator!=(IYX const& lhs, IYX const& rhs)
	{
		return !(lhs == rhs);
	}
}

