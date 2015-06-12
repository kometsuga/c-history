#pragma once

#include "draw.h"
#include "cartograph.h"
#include "types.h"
#include <boost\optional.hpp>
#undef max
//-----------------------------------------------------------------------
// �`�ʗp�̊����`����
//-----------------------------------------------------------------------
class DrawMain final : public draw::DrawEnvironment
{
	// �R�[���o�b�N
	void keyCall(
//		GLFWwindow* win,
		int         key,
		int         scancode,
		int         action,
		int         mods)override;

	// window����
	GLFWwindow* createWindowImp() override;
	// ��v�Ȋe�T�C�N���`��
	void drawCall() override;
	void winSizeCall(int width, int height) override;
	// �v�f
	//ca::HeightCa2old heightCa2old_;
	
	// �f�[�^�n
	std::vector<ca::Point > const points_;
	ca::LandCa2 const landCa2_;

	// ��Ԍn
	types::IYX leftUpPos_;
	std::string title_;

	boost::optional<draw::TextureInfo> backTex_;
//	GLuint backTex_ = std::numeric_limits<GLuint>::max();
	int counter_ = 0;
public:
	// �����Ȃ�
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




