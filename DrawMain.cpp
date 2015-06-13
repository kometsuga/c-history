#include "DrawMain.h"
//#include "constant.h"
#include <Windows.h>
#include <random>
#include <sstream>

#undef RGB
#undef min
#undef max
#include <boost/format.hpp>

namespace
{
	int const MARGIN = 1; // �}�[�W��

	int nNextSea(ca::Point const& point, ca::LandCa2 const& landCa2)
	{
		int nSea = 0;
		if (landCa2(point.iPlace() + types::IYX(0, 1)) < 0)++nSea;
		if (landCa2(point.iPlace() + types::IYX(0, -1)) < 0)++nSea;
		if (landCa2(point.iPlace() + types::IYX(1, 0)) < 0)++nSea;
		if (landCa2(point.iPlace() + types::IYX(-1, 0)) < 0)++nSea;
		return nSea;
	}
	// ���̒��S�_(�J�[�\���ʒu)�̐���
	// �ׂɊC�Ɨ��̗���������
	types::IYX firstPlace(std::vector<ca::Point > const& points, ca::LandCa2 const& landCa2)
	{
		if (points.empty())
			throw std::runtime_error("points����Ȃ̂Ōv�Z�ł��Ȃ�");


		std::random_device rnd;

		for (;;)
		{
			auto const& point = points[rnd() % points.size()];
			int const n = nNextSea(point, landCa2);
			if (1 <= n && n <= 3)
				return point.iPlace();
		}
	}
	//--------------------------------------------------------------------------
	// �摜�̐����A�_�P�ʂňڂ����߂̊֐�
	//--------------------------------------------------------------------------
	class ToPoint
	{
		types::IYX posLeftUp_;
		std::vector<ca::Point > const& points_;
		ca::LandCa2 const& landCa2_;

		ToPoint& operator=(ToPoint const&) = delete;
	public:
		ToPoint(types::IYX posLeftUp,
			std::vector<ca::Point > const& points,ca::LandCa2 const& landCa2): 
				posLeftUp_(posLeftUp),points_(points),landCa2_(landCa2)
		{
		}


		draw::RGBA operator()(int iYt, int iXt)const
		{
			int const iY = iYt + posLeftUp_.iY;
			int const iX = iXt + posLeftUp_.iX;

			if (!landCa2_.isValid({ iY, iX }))
				return draw::RGBA(0, 128, 255, 255);

			auto const h = std::max<short>(0, points_[landCa2_({ iY, iX })].height);
			auto const r = static_cast<unsigned __int8>(std::min(255, h / 4 + 1));
			auto const g = static_cast<unsigned __int8>(
				h < 6 * 255 ? std::max(0, std::min(255, 255 - h / 2))
				: std::min(255, (h - 6 * 255) / 6));
			auto const b = static_cast<unsigned __int8>(std::min(255, h / 8 + 1));
			return draw::RGBA(r, g, b, 255);
		}
	};
	//--------------------------------------------------------------------------
	// �摜�̐���
	//--------------------------------------------------------------------------
	draw::Texture makeTexture(
		types::IYX posLeftUp,// cursor�ł͂Ȃ��AleftUpPos������ɂ�����
		types::Size2 size,
		std::vector<ca::Point > const& points,
		ca::LandCa2 const& landCa2)
	{

		ToPoint const toPoint(posLeftUp, points, landCa2);
		return draw::makeTexture1(size, toPoint);
	}
	//-----------------------------------------------------------------------
	// [textureWidth*textureWidth]�̉摜�f�[�^���`
	//-----------------------------------------------------------------------
	void drawTexture(draw::TextureInfo const&  tex)//GLuint texID, draw::Texture const& texture)
	{
		// �摜�ƃe�N�X�`��ID�����т���B
		glBindTexture(GL_TEXTURE_2D, tex.id_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			tex.texture_.size().x,
			tex.texture_.size().y,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex.texture_.ptr());
		// �e�N�X�`���̊e��ݒ�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	//-----------------------------------------------------------------------
	// texture�̍ĕ`��
	//-----------------------------------------------------------------------
	void redrawTexture(draw::TextureInfo const&  tex)//GLuint texID, draw::Texture const& texture)
	{
		glBindTexture(GL_TEXTURE_2D, tex.id_);
		glTexSubImage2D(
			GL_TEXTURE_2D, // target
			0,    // level
			0, 0, // xoffset, yoffset
			tex.texture_.size().x,
			tex.texture_.size().y,
			GL_RGBA,// format,
			GL_UNSIGNED_BYTE,//type
			tex.texture_.ptr());
	}

}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
DrawMain::DrawMain(
	std::vector<ca::Point > const& points,
	ca::LandCa2 const& landCa2,
	std::string const& title) :
	points_(points), landCa2_(landCa2), counter_(0)
{
	// firstPlace(points, landCa2);

	leftUpPos_ = firstPlace(points, landCa2);


	//int const iY = iYt - size2.y / 2 + cursor.iY;
	//int const iX = iXt - size2.x / 2 + cursor.iX;

//	texture_ = makeTexture(leftUpPos_, { 730, 1330 },points, landCa2);
	title_ = title;
	counter_ = 0;
}


//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
DrawMain::~DrawMain()
{
}
//---------------------------------------------------------------------------
//
// glTexSubImage2D�g����������?
//---------------------------------------------------------------------------
void DrawMain::keyCall(int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(win(), GL_TRUE);
		return;
	}
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		bool directionKeyPressed = false;
		int const step = 8;
		if (key == GLFW_KEY_LEFT){ leftUpPos_.iX -= step; directionKeyPressed = true; }
		if (key == GLFW_KEY_RIGHT){ leftUpPos_.iX += step; directionKeyPressed = true; }
		if (key == GLFW_KEY_UP){ leftUpPos_.iY -= step; directionKeyPressed = true; }
		if (key == GLFW_KEY_DOWN){ leftUpPos_.iY += step; directionKeyPressed = true; }
		if (!directionKeyPressed)return;
		if (!backTex_)return;

		std::stringstream str;
		str << boost::format("directionKeyPressed key:%d scancode:%d action:%d modes:%d")
			% key % scancode % action % mods;
		glfwSetWindowTitle(win(), str.str().c_str());

		// �� ���̂����A�f���̍������ւ�����肽��
		//draw::Texture makeTexture(
		//	types::IYX posLeftUp,// cursor�ł͂Ȃ��AleftUpPos������ɂ�����
		//	types::Size2 size,
		//	std::vector<ca::Point > const& points,
		//	ca::LandCa2 const& landCa2)
		ToPoint const toPoint(leftUpPos_, points_, landCa2_);
		backTex_->texture_ = draw::makeTexture1(backTex_->texture_.size(), toPoint);
		redrawTexture(*backTex_);
		return;
	}
	std::stringstream str;
	str << boost::format("%s key:%d scancode:%d action:%d modes:%d")
		% title_ % key % scancode % action % mods;
	glfwSetWindowTitle(win(), str.str().c_str());

}
//---------------------------------------------------------------------------
// window����
//---------------------------------------------------------------------------
GLFWwindow* DrawMain::createWindowImp()
{
	int const winSizeY = GetSystemMetrics(SM_CYSCREEN) - 50;
	int const winSizeX = GetSystemMetrics(SM_CXSCREEN) - 50;

	leftUpPos_.iY -= winSizeY / 2 - MARGIN;
	leftUpPos_.iX -= winSizeX / 2 - MARGIN;
	//	int const sizeWinY = MARGIN + texture_.get().size().y + MARGIN;
//	int const sizeWinX = MARGIN + texture_.get().size().x + MARGIN;
	return glfwCreateWindow(winSizeX, winSizeY, title_.c_str(), nullptr, nullptr);
}
//---------------------------------------------------------------------------
// �e�����`��
//---------------------------------------------------------------------------
void DrawMain::drawCall()
{
	//if (backTex_ == std::numeric_limits<GLuint>::max())
	if (!backTex_)
	{
		backTex_ = draw::TextureInfo();
		glGenTextures(1, &backTex_->id_);
		types::Size2 size;

		glfwGetFramebufferSize(win(), &size.x, &size.y);

		size.x -= 2 * MARGIN;
		size.y -= 2 * MARGIN;


		ToPoint const toPoint(leftUpPos_, points_, landCa2_);
		backTex_->texture_ = draw::makeTexture1(size, toPoint);
//		backTex_->texture_ = makeTexture(leftUpPos_, size, points_, landCa2_);
		drawTexture(*backTex_);
	}	
	GLint const minY = MARGIN;
	GLint const minX = MARGIN;
	//GLint const maxY = static_cast<GLint>(minY + texture_.get(). size().y);
	//GLint const maxX = static_cast<GLint>(minX + texture_.get(). size().x);
	GLint const maxY = static_cast<GLint>(minY + backTex_->texture_.size().y);
	GLint const maxX = static_cast<GLint>(minX + backTex_->texture_.size().x);
	glBindTexture(GL_TEXTURE_2D, backTex_->id_);// �摜�w��
	// �ȉ��`��
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDrawArrays(GL_QUADS, 0, 4);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(minX, minY);
	glTexCoord2i(1, 0); glVertex2i(maxX, minY);
	glTexCoord2i(1, 1); glVertex2i(maxX, maxY);
	glTexCoord2i(0, 1); glVertex2i(minX, maxY);
	glEnd();
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);

	//if (false)
	//{
	//	std::stringstream str;
	//	str << boost::format("%s %d") % title_ % counter_;
	//	glfwSetWindowTitle(win(), str.str().c_str());
	//	++counter_;
	//}

}
//---------------------------------------------------------------------------
// ���̑傫�����ω������Ƃ�
//---------------------------------------------------------------------------
void DrawMain::winSizeCall(int width, int height)
{
	int width1;
	int height1;
	glfwGetFramebufferSize(win(), &width1, &height1);

	std::stringstream str;
	str << boost::format("%s (%d %d) (%d %d)") 
		% title_ 
		% width % height
		% width1 % height1;
	glfwSetWindowTitle(win(), str.str().c_str());

	if (backTex_)
	{
		glDeleteTextures(1, &backTex_->id_);

		// �����ł́A�폜���đ傫���ύX�ɑΉ����Ă��邪�A
		// �ʂ̕��@��T�肽���B
		backTex_ = boost::none;


	}	
	//auto const texture = makeTexture(leftUpPos_, backTex_->size_, points_, landCa2_);
	//drawTexture(backTex_->id_, texture);


}
