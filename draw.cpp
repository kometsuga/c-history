#include"draw.h"
#include"util.h"

//-----------------------------------------------------------------------
// ����J��`
//-----------------------------------------------------------------------

#include<glfw3.h>
//#include<cstdlib>
//#include<cstdio>
#define _USE_MATH_DEFINES
//#include<cmath>
#include<fstream>
#include<sstream>
#include<vector>
//#include<cassert>


#pragma warning(push)
#pragma warning(disable:4005)
#include<Windows.h>
#pragma warning(pop)

#undef RGB
#undef max
#undef min

#if defined(_MSC_VER)
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glfw3.lib")
#endif

//#include <boost/format.hpp>
//#include <sstream>
#include <unordered_map>
//<unordered_map>

//namespace draw
//{
namespace
{
	//-----------------------------------------------------------------------
	// �`��̑Ή����`
	// DrawEnvironment
	//-----------------------------------------------------------------------
	std::unordered_map < GLFWwindow const*, draw::DrawEnvironment* >
		winToEnvironment;

	//-----------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------
	draw::DrawEnvironment* getEnv(GLFWwindow const* win)
	{
		auto ite = winToEnvironment.find(win);
		return (ite != winToEnvironment.end()) ? ite->second : nullptr;
	}


	//-----------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------
	void errorCallback(int error, char const* description)
	{
		std::wstringstream str;
		str << "error:" << error << "  description:" << description;
		{
			std::wofstream fp("glflErrorLog.txt");
			fp << str.str() << '\n';
		}
		MessageBoxW(nullptr, str.str().c_str(), L"glfw error", 0);

	}
	//-----------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------
	void keyCallback(GLFWwindow* win,int key,int scancode,int action,int mods)
	{
		auto env = getEnv(win);
		if(env)env->keyCall(key, scancode, action, mods);
	}
	//-----------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------
	void winSizeCallback(GLFWwindow * win, int width, int height)
	{
		auto env = getEnv(win);
		if (env)env->winSizeCall(width, height);
	}

}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
draw::DrawEnvironment::~DrawEnvironment()
{
	clearWindow();
}
//-------------------------------------------------------------------------
// ����
//-------------------------------------------------------------------------
//-----------------------------------------------------------------------
// window�𐶐�����
//-----------------------------------------------------------------------
void draw::DrawEnvironment::createWindow()
{
	clearWindow();
	win_ = createWindowImp();
	if (!win())
		throw std::runtime_error("glfwCreateWindow�̃G���[");
	// ���̐���
	glfwMakeContextCurrent(win());
	winToEnvironment.insert({ win(), this });
//	addEnvironment(win(), this);
}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void draw::DrawEnvironment::clearWindow( )
{
	if (!win())return;
	winToEnvironment.erase(win());
	glfwDestroyWindow(win());
	win_ = nullptr;
}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void draw::execute(draw::DrawEnvironment& env)
{
	// ���ԑ傫����`
	// �{��
	glfwSetErrorCallback(errorCallback);
	if (!glfwInit())
		throw std::runtime_error("glfwInit�̏��������s");

	env.createWindow();
	glfwSwapInterval(1);
	glfwSetKeyCallback(env.win(), ::keyCallback);
	glfwSetWindowSizeCallback(env.win(), ::winSizeCallback);

	while(!glfwWindowShouldClose(env.win()))
	{
		int width = 0;
		int height = 0;

		glfwGetFramebufferSize(env.win(), &width, &height);
		glViewport(0, 0, width, height);

		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, 1, -1);

		env.drawCall();

		glfwSwapBuffers(env.win());
		glfwWaitEvents();
		//glfwPollEvents();
	}
	
	env.clearWindow();
	glfwTerminate();

}



