//-----------------------------------------------------------------------
// 画像描画系
//-----------------------------------------------------------------------
#pragma once

#include"types.h"
//#include<utility>
#include <vector>
#undef RGB
#include <glfw3.h>

namespace draw
{
	// 先行宣言
	class RGB;
	//-----------------------------------------------------------------------
	// 4色定義
	//-----------------------------------------------------------------------
	class RGBA
	{
		unsigned __int8 r_;
		unsigned __int8 g_;
		unsigned __int8 b_;
		unsigned __int8 a_;
	public:
		// 生成
		RGBA() :RGBA(0,0,0,255){}
		RGBA(unsigned __int8 r,
		     unsigned __int8 g,
		     unsigned __int8 b,
		     unsigned __int8 a) :r_(r), g_(g), b_(b), a_(a){}
		explicit RGBA(RGB const& rgb);// { *this = rgb.imp_; }
//		RGBA(RGBA const&) = default;

		// 変換
		explicit operator RGB() const;
		// 要素
		unsigned __int8 r()const{ return r_; }
		unsigned __int8 g()const{ return g_; }
		unsigned __int8 b()const{ return b_; }
		unsigned __int8 a()const{ return a_; }
	};
	//-----------------------------------------------------------------------
	// 3色定義
	//-----------------------------------------------------------------------
	class RGB
	{
		friend RGBA;
		RGBA imp_;
	public:
		// 生成
		RGB(){}
		RGB(unsigned __int8 r,
			unsigned __int8 g,
			unsigned __int8 b) : imp_(r, g, b, 255){}
		// 変換
		explicit RGB(RGBA imp) :imp_(imp){}
		explicit operator RGBA() const{ return imp_; }
		// 要素
		unsigned __int8 r()const{ return imp_.r(); }
		unsigned __int8 g()const{ return imp_.r(); }
		unsigned __int8 b()const{ return imp_.r(); }
	};
	inline RGBA::RGBA(RGB const& rgb) :RGBA(rgb.imp_){}
	inline RGBA::operator RGB() const{return RGB(*this);}
	//-----------------------------------------------------------------------
	// 画像定義(固定長)
	//-----------------------------------------------------------------------
	class Texture
	{
		types::Size2 size_;
		std::vector<RGBA>ptr_;
	public:
		// 生成
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
		// 定数
		types::Size2 const& size()const { return size_; }

		// 設定
		void setRgba(int iY, int iX, RGBA rgba)
		{
			ptr_[iY*size().x + iX] = rgba;
		}
		void setRgb(int iY, int iX, RGB rgb)
		{
			setRgba(iY, iX, static_cast<RGBA>(rgb));
		}
		// 
		// 要素
		bool empty()const{ return ptr_.empty(); }
		RGBA const* ptr()const{ return &ptr_[0]; }
		RGBA rgba(int iY, int iX)const{ return ptr_[iY*size().x + iX]; }
		RGB  rgb (int iY, int iX)const
		{
			return static_cast<RGB>(rgba(iY, iX));
		}
	};
	//-----------------------------------------------------------------------
	// Texture生成支援クラス
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
	// 描写用の環境を用意する
	// 暫定的に、ぎこちない「型紙メソッド」様式で作成する。
	//-----------------------------------------------------------------------
	class DrawEnvironment
	{
		//-----------------------------------------------------------------------
		// 要素
		//-----------------------------------------------------------------------
		GLFWwindow* win_ = nullptr;
		//-----------------------------------------------------------------------
		// 内部操作
		//-----------------------------------------------------------------------
		virtual GLFWwindow* createWindowImp() = 0;
	public:

		//-------------------------------------------------------------------------
		// 生成
		//-------------------------------------------------------------------------
		virtual ~DrawEnvironment();


		//-------------------------------------------------------------------------
		// 操作(呼び出し系)
		//-------------------------------------------------------------------------
		virtual void keyCall(int key,int scancode,int action,int mods) = 0;
		virtual void drawCall() = 0;
		virtual void winSizeCall(int width,int height) = 0;
		//-------------------------------------------------------------------------
		// 操作
		//-------------------------------------------------------------------------
		void createWindow();
		void clearWindow ();
		//-------------------------------------------------------------------------
		// 要素
		//-------------------------------------------------------------------------
		GLFWwindow* win(){ return win_; }

	};
	// 全体実行
	void execute(DrawEnvironment& env);
	//-----------------------------------------------------------------------
	// 画像表示構造調整
	//-----------------------------------------------------------------------
	struct TextureInfo
	{
		GLuint id_;
//		types::Size2 size_;
		Texture texture_;
	};

}
