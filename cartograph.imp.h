#pragma once

//#include "cartograph.h"
//===================================================================
// 2íiç\ë¢ínê}
//===================================================================
//-------------------------------------------------------------------
// ê∂ê¨
//-------------------------------------------------------------------
template<typename T>
ca::Cartograph2<T>::Cartograph2(T defaultValue) : 
	defaultValue_(defaultValue),
	nPoints_(0), iMin_({ 0, 0 }), iMax_({ 0, 0 })
{
	auto deleteArray = [](T** ptr2)
	{
		for (int i = 0; i < LEN_H*LEN_H; ++i)
			delete[] ptr2[i];
		delete[] ptr2;
	};
	ptr2_.reset(new T*[LEN_H*LEN_H], deleteArray);
	std::fill_n(ptr2_.get(), LEN_H*LEN_H, nullptr);
}

//-------------------------------------------------------------------
// óvëfê›íË
//-------------------------------------------------------------------
template<typename T>
void ca::Cartograph2<T>::setValue(types::IYX const& iPlace, T value)
{
//	if (!cn::isLand(value))return;
	if (value == defaultValue())return;

	T*& lowArea = ptr2_.get()[iHigh(iPlace)];
	if (!lowArea)
	{
		lowArea = new T[LEN_L*LEN_L];
		std::fill_n(lowArea, LEN_L*LEN_L, defaultValue());// cn::SEA_HEIGHT);
	}
	lowArea[iLow(iPlace)] = value;

	if (nPoints() == 0)
	{
		iMin_ = iPlace;// types::IYX({ iY, iX });
		iMax_ = iPlace; // types::IYX(iY, iX);
	}
	else
	{
		iMin_ = types::min(iPlace, iMin_);
		iMax_ = types::max(iPlace, iMax_);
	}
	++nPoints_;
	// 2èÊê›íËÇÕåüç∏ÇµÇ»Ç¢
}
//-------------------------------------------------------------------
// óvëféÊìæ
//-------------------------------------------------------------------
template<typename T>
T ca::Cartograph2<T>::operator()(types::IYX const& iPlace)const
{
	int const iHigh1 = iHigh(iPlace);
	T const* lowArea = 0 <= iHigh1 ? ptr2_.get()[iHigh1] : nullptr;
	return lowArea ? lowArea[iLow(iPlace)] : defaultValue();// cn::SEA_HEIGHT;
}
template<typename T>
bool ca::Cartograph2<T>::isValid(types::IYX const& iPlace)const
{
	T const* lowArea = ptr2_.get()[iHigh(iPlace)];
	if (!lowArea)return false;
	return lowArea[iLow(iPlace)] != defaultValue();
}
