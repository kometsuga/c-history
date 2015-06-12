#pragma once

#include "draw.h"
#include "cartograph.h"
#include "types.h"
#include <boost\optional.hpp>
#undef max
//-----------------------------------------------------------------------
// 描写用の環境を定義する
//-----------------------------------------------------------------------
class DrawMain final : public draw::DrawEnvironment
{
	// コールバック
	void keyCall(
//		GLFWwindow* win,
		int         key,
		int         scancode,
		int         action,
		int         mods)override;

	// window生成
	GLFWwindow* createWindowImp() override;
	// 主要な各サイクル描画
	void drawCall() override;
	void winSizeCall(int width, int height) override;
	// 要素
	//ca::HeightCa2old heightCa2old_;
	
	// データ系
	std::vector<ca::Point > const points_;
	ca::LandCa2 const landCa2_;

	// 状態系
	types::IYX leftUpPos_;
	std::string title_;

	boost::optional<draw::TextureInfo> backTex_;
//	GLuint backTex_ = std::numeric_limits<GLuint>::max();
	int counter_ = 0;
public:
	// 生成など
	DrawMain() = delete;
	//operator
	explicit DrawMain(
		std::vector<ca::Point > const& points,
		ca::LandCa2 const& landCa2,
		std::string const& title);
	virtual ~DrawMain();

	DrawMain(DrawMain const&) = delete;
	DrawMain& operator=(DrawMain const&) = delete;
	//

//	draw::Texture const& texture()const{ return texture_.get(); }

};




