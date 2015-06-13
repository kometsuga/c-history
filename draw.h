//-----------------------------------------------------------------------
// �摜�`��n
//-----------------------------------------------------------------------
#pragma once

#include"types.h"
//#include<utility>
#include <vector>
#undef RGB
#include <glfw3.h>

namespace draw
{
	// ��s�錾
	class RGB;
	//-----------------------------------------------------------------------
	// 4�F��`
	//-----------------------------------------------------------------------
	class RGBA
	{
		unsigned __int8 r_;
		unsigned __int8 g_;
		unsigned __int8 b_;
		unsigned __int8 a_;
	public:
		// ����
		RGBA() :RGBA(0,0,0,255){}
		RGBA(unsigned __int8 r,
		     unsigned __int8 g,
		     unsigned __int8 b,
		     unsigned __int8 a) :r_(r), g_(g), b_(b), a_(a){}
		explicit RGBA(RGB const& rgb);// { *this = rgb.imp_; }
//		RGBA(RGBA const&) = default;

		// �ϊ�
		explicit operator RGB() const;
		// �v�f
		unsigned __int8 r()const{ return r_; }
		unsigned __int8 g()const{ return g_; }
		unsigned __int8 b()const{ return b_; }
		unsigned __int8 a()const{ return a_; }
	};
	//-----------------------------------------------------------------------
	// 3�F��`
	//-----------------------------------------------------------------------
	class RGB
	{
		friend RGBA;
		RGBA imp_;
	public:
		// ����
		RGB(){}
		RGB(unsigned __int8 r,
			unsigned __int8 g,
			unsigned __int8 b) : imp_(r, g, b, 255){}
		// �ϊ�
		explicit RGB(RGBA imp) :imp_(imp){}
		explicit operator RGBA() const{ return imp_; }
		// �v�f
		unsigned __int8 r()const{ return imp_.r(); }
		unsigned __int8 g()const{ return imp_.r(); }
		unsigned __int8 b()const{ return imp_.r(); }
	};
	inline RGBA::RGBA(RGB const& rgb) :RGBA(rgb.imp_){}
	inline RGBA::operator RGB() const{return RGB(*this);}
	//-----------------------------------------------------------------------
	// �摜��`(�Œ蒷)
	//-----------------------------------------------------------------------
	class Texture
	{
		types::Size2 size_;
		std::vector<RGBA>ptr_;
	public:
		// ����
		Texture(types::Size2 const& size, RGBA rgba=RGBA()) :
			size_(size),
			ptr_(size.area(), rgba)
		{
		}
		Texture(types::Size2 const& size, RGB rgb) :
			Texture(size, static_cast<RGBA>(rgb))
		{
		}
		Texture(){}
		// �萔
		types::Size2 const& size()const { return size_; }

		// �ݒ�
		void setRgba(int iY, int iX, RGBA rgba)
		{
			ptr_[iY*size().x + iX] = rgba;
		}
		void setRgb(int iY, int iX, RGB rgb)
		{
			setRgba(iY, iX, static_cast<RGBA>(rgb));
		}
		// 
		// �v�f
		bool empty()const{ return ptr_.empty(); }
		RGBA const* ptr()const{ return &ptr_[0]; }
		RGBA rgba(int iY, int iX)const{ return ptr_[iY*size().x + iX]; }
		RGB  rgb (int iY, int iX)const
		{
			return static_cast<RGB>(rgba(iY, iX));
		}
	};
	//-----------------------------------------------------------------------
	// Texture�����x���N���X
	/// toPointColor : (Int,Int) -> RGBA
	//-----------------------------------------------------------------------
	template <typename ToPoint>
	Texture makeTexture1(types::Size2 size, ToPoint const& toPoint)
//	template <1, typename ToPoint>
///	Texture buildTexture(types::Size2 size, ToPoint const& toPoint)
	{
		draw::Texture texture(size);
		for (int iY = 0; iY < size.y; ++iY)
			for (int iX = 0; iX < size.x; ++iX)
				texture.setRgba(iY, iX, toPoint(iY,iX));
		return texture;
	};

	//-----------------------------------------------------------------------
	// �`�ʗp�̊���p�ӂ���
	// �b��I�ɁA�������Ȃ��u�^�����\�b�h�v�l���ō쐬����B
	//-----------------------------------------------------------------------
	class DrawEnvironment
	{
		//-----------------------------------------------------------------------
		// �v�f
		//-----------------------------------------------------------------------
		GLFWwindow* win_ = nullptr;
		//-----------------------------------------------------------------------
		// ��������
		//-----------------------------------------------------------------------
		virtual GLFWwindow* createWindowImp() = 0;
	public:

		//-------------------------------------------------------------------------
		// ����
		//-------------------------------------------------------------------------
		virtual ~DrawEnvironment();


		//-------------------------------------------------------------------------
		// ����(�Ăяo���n)
		//-------------------------------------------------------------------------
		virtual void keyCall(int key,int scancode,int action,int mods) = 0;
		virtual void drawCall() = 0;
		virtual void winSizeCall(int width,int height) = 0;
		//-------------------------------------------------------------------------
		// ����
		//-------------------------------------------------------------------------
		void createWindow();
		void clearWindow ();
		//-------------------------------------------------------------------------
		// �v�f
		//-------------------------------------------------------------------------
		GLFWwindow* win(){ return win_; }

	};
	// �S�̎��s
	void execute(DrawEnvironment& env);
	//-----------------------------------------------------------------------
	// �摜�\���\������
	//-----------------------------------------------------------------------
	struct TextureInfo
	{
		GLuint id_;
//		types::Size2 size_;
		Texture texture_;
	};

}
