﻿#include "stdafx.h"
#include "gmuiglwindow.h"
#include <gamemachine.h>
#include "gmuiinput.h"

namespace
{
	const gm::GMwchar* g_classname = L"gamemachine_MainWindow_class";
}

GMUIGLWindow::GMUIGLWindow()
{
	D(d);
	d->depthBits = 24;
	d->stencilBits = 8;
}

GMUIGLWindow::~GMUIGLWindow()
{
	dispose();
}

gm::GMWindowHandle GMUIGLWindow::create(const gm::GMWindowAttributes& wndAttrs)
{
	D(d);
	gm::GMWindowAttributes attrs = wndAttrs;
	attrs.dwExStyle |= WS_EX_CLIENTEDGE;
	attrs.dwStyle |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_BORDER | WS_CAPTION;

	// 在非全屏的时候才有效
	AdjustWindowRectEx(&attrs.rc, attrs.dwStyle, FALSE, attrs.dwExStyle);

	if (!createWindow(attrs, g_classname))
	{
		DWORD s = GetLastError();
		gm_error(L"window created failed: %i", s);
		dispose();
		return false;
	}

	const gm::GMbyte colorDepth = 32, alphaBits = 8;
	gm::GMuint pixelFormat;
	static PIXELFORMATDESCRIPTOR pfd =						//pfd tells windows how we want things to be
	{
		sizeof(PIXELFORMATDESCRIPTOR),						//size of Pixel format descriptor
		1,													//Version Number
		PFD_DRAW_TO_WINDOW |								//must support window
		PFD_SUPPORT_OPENGL |								//must support opengl
		PFD_DOUBLEBUFFER,									//must support double buffer
		PFD_TYPE_RGBA,										//request RGBA format
		colorDepth,											//select color depth
		0, 0, 0, 0, 0, 0,									//color bits ignored
		alphaBits,											//alpha buffer bits
		0,													//shift bit ignored
		0,													//no accumulation buffer
		0, 0, 0, 0,											//accumulation bits ignored
		d->depthBits,										//z buffer bits
		d->stencilBits,										//stencil buffer bits
		0,													//no auxiliary buffer
		PFD_MAIN_PLANE,										//main drawing layer
		0,													//reserved
		0, 0, 0												//layer masks ignored
	};

	gm::GMWindowHandle wnd = getWindowHandle();
	if (!(d->hDC = GetDC(wnd)))
	{
		dispose();
		gm_error(L"can't Create a GL Device context.");
		return 0;
	}

	if (!(pixelFormat = ChoosePixelFormat(d->hDC, &pfd)))	//found a matching pixel format?
	{														//if not
		dispose();
		gm_error(L"can't find a Suitable PixelFormat.");
		return 0;
	}

	if (!SetPixelFormat(d->hDC, pixelFormat, &pfd))			//are we able to set pixel format?
	{														//if not
		dispose();
		gm_error(L"can't set the pixelformat.");
		return 0;
	}

	if (!(d->hRC = wglCreateContext(d->hDC)))
	{
		dispose();
		gm_error(L"can't create a GL rendering context.");
		return 0;
	}

	if (!wglMakeCurrent(d->hDC, d->hRC))
	{
		dispose();
		gm_error(L"can't activate the GL rendering context.");
		return 0;
	}

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		GM_ASSERT(!"glew init failed");
		return 0;
	}

	return wnd;
}

void GMUIGLWindow::swapBuffers() const
{
	D(d);
	::SwapBuffers(d->hDC);
}

bool GMUIGLWindow::wndProc(gm::GMuint uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lRes)
{
	D(d);
	D_BASE(db, GMUIWindow);

	bool handled = Base::wndProc(uMsg, wParam, lParam, lRes);
	if (handled)
		return handled;

	switch (uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0L);
		break;
	case WM_SIZE:
	{
		gm::GameMachine::instance().postMessage({ gm::GameMachineMessageType::WindowSizeChanged });
		break;
	}
	case WM_MOUSEWHEEL:
	{
		GMInput* input = gm_static_cast<GMInput*>(db->input);
		if (input)
			input->recordWheel(true, GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	}
	case WM_LBUTTONDOWN:
	{
		GMInput* input = gm_static_cast<GMInput*>(db->input);
		if (input)
			input->recordMouseDown(GMMouseButton_Left);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		GMInput* input = gm_static_cast<GMInput*>(db->input);
		if (input)
			input->recordMouseDown(GMMouseButton_Right);
		break;
	}
	case WM_LBUTTONUP:
	{
		GMInput* input = gm_static_cast<GMInput*>(db->input);
		if (input)
			input->recordMouseUp(GMMouseButton_Left);
		break;
	}
	case WM_RBUTTONUP:
	{
		GMInput* input = gm_static_cast<GMInput*>(db->input);
		if (input)
			input->recordMouseUp(GMMouseButton_Right);
		break;
	}
	default:
		break;
	}

	*lRes = ::DefWindowProc(getWindowHandle(), uMsg, wParam, lParam);
	return true;
}

void GMUIGLWindow::update()
{
	swapBuffers();
	Base::update();
}

void GMUIGLWindow::dispose()
{
	D(d);
	gm::GMWindowHandle wnd = getWindowHandle();
	if (d->hRC)
	{
		if (!wglMakeCurrent(0, 0))
			gm_error(L"release of DC and RC failed.");

		if (!wglDeleteContext(d->hRC))
			gm_error(L"release Rendering Context failed.");

		d->hRC = 0;
	}

	if (d->hDC && !ReleaseDC(wnd, d->hDC))
	{
		gm_error(L"release of Device Context failed.");
		d->hDC = 0;
	}

	if (wnd && !DestroyWindow(wnd))
	{
		gm_error(L"could not release hWnd");
		wnd = 0;
	}
}

void GMUIGLWindow::showWindow()
{
	D(d);
	gm::GMWindowHandle hwnd = getWindowHandle();
	GM_ASSERT(::IsWindow(hwnd));
	if (!::IsWindow(hwnd)) return;
	::ShowWindow(hwnd, SW_SHOWNORMAL);
}