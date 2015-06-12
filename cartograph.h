#pragma once
#include "types.h"
//#include <array>
#include <memory>
//#include <vector>
#include <boost/filesystem.hpp>
//#include <boost/shared_array.hpp>

namespace filesystem = boost::filesystem;

namespace ca
{
	//===================================================================
	// ï‚èïíËêî
	// 1 : 4141.1
	// 2 : 4046.8
	// 4 : 4013.0
	// 8 : 4009.5
	// 16 : 4091.4
	//===================================================================
	int const LEN_H = 256*4;
	int const LEN_L = 256/4;
	//===================================================================
	// ï‚èïä÷êî
	//===================================================================
	inline int iHigh(types::IYX const& iPlace)
	{
		return (iPlace.iY / LEN_L)*LEN_H + (iPlace.iX / LEN_L);
	}
	// í·à îzóÒindex
	inline int iLow(types::IYX const& iPlace)
	{
		return (iPlace.iY % LEN_L)*LEN_L + (iPlace.iX % LEN_L);
	}
	//---------------------------------------------------------------
	// ínê}óvëf
	//---------------------------------------------------------------
	struct Point
	{
		//types::IYX iPlace;
		unsigned short iY;
		unsigned short iX;
		short height;

		types::IYX iPlace()const{ return types::IYX(iY, iX); }
	};
	inline bool operator== (Point const& lhs, Point const& rhs)
	{
		return lhs.iY == rhs.iY && lhs.iX == rhs.iX && lhs.height == rhs.height;
	}
	inline bool operator!= (Point const& lhs, Point const& rhs)
	{
		return !(lhs == rhs);
	}
	//---------------------------------------------------------------
	// 2íiç\ë¢ínê}
	//---------------------------------------------------------------
	template<typename T>
	class Cartograph2
	{
		T defaultValue_;
		std::shared_ptr<T*>ptr2_;
		int nPoints_;

		types::IYX iMin_;
		types::IYX iMax_;
	public:
		// ê∂ê¨
		explicit Cartograph2(T defaultValue);
		// óvëfê›íË
		void setValue(types::IYX const& iPlace, T value);
		// óvëféÊìæ
		T operator()(types::IYX const& iPlace)const;
		bool isValid(types::IYX const& iPlace)const;

		T defaultValue()const{ return defaultValue_; }
		int nPoints()const{ return nPoints_; }
		types::IYX const& iMin()const{ return iMin_; }
		types::IYX const& iMax()const{ return iMax_; }
	};

	//---------------------------------------------------------------
	// ínê}ñºèÃ
	//---------------------------------------------------------------
	//using HeightCa2old = ca::Cartograph2 < short >;
	using LandCa2 = ca::Cartograph2 < __int32 >;
}
#include "cartograph.imp.h"


