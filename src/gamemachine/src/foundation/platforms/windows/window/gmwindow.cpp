﻿#include "stdafx.h"
#include "gmengine/ui/gmwindow.h"
#include "foundation/platforms/windows/window/gminput.h"
#include "foundation/gamemachine.h"
#include "gmengine/ui/gmwidget.h"
#include "gmengine/ui/gmwindow_p.h"

#ifndef GetWindowOwner
#	define GetWindowOwner(hwnd) GetWindow(hwnd, GW_OWNER)
#endif

#ifndef GetWindowStyle
#	define GetWindowStyle(hwnd) ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#endif

#define DefaultWindowWidth 1024
#define DefaultWindowHeight 768

BEGIN_NS

namespace
{
	Map<HWND, GMWindow*> s_hwndMap;

	GMKey translateKey(GMWParam wParam)
	{
		static HashMap<GMWParam, GMKey> s_keyMap;
		if (s_keyMap.empty())
		{
			s_keyMap[VK_LBUTTON] = GMKey_Lbutton;
			s_keyMap[VK_RBUTTON] = GMKey_Rbutton;
			s_keyMap[VK_CANCEL] = GMKey_Cancel;
			s_keyMap[VK_MBUTTON] = GMKey_Mbutton;
			s_keyMap[VK_XBUTTON1] = GMKey_Xbutton1;
			s_keyMap[VK_XBUTTON2] = GMKey_Xbutton2;
			s_keyMap[VK_BACK] = GMKey_Back;
			s_keyMap[VK_TAB] = GMKey_Tab;
			s_keyMap[VK_CLEAR] = GMKey_Clear;
			s_keyMap[VK_RETURN] = GMKey_Return;
			s_keyMap[VK_SHIFT] = GMKey_Shift;
			s_keyMap[VK_CONTROL] = GMKey_Control;
			s_keyMap[VK_MENU] = GMKey_Menu;
			s_keyMap[VK_PAUSE] = GMKey_Pause;
			s_keyMap[VK_CAPITAL] = GMKey_Capital;
			s_keyMap[VK_KANA] = GMKey_Kana;
			s_keyMap[VK_HANGUL] = GMKey_Hangul;
			s_keyMap[VK_JUNJA] = GMKey_Junja;
			s_keyMap[VK_FINAL] = GMKey_Final;
			s_keyMap[VK_HANJA] = GMKey_Hanja;
			s_keyMap[VK_KANJI] = GMKey_Kanji;
			s_keyMap[VK_ESCAPE] = GMKey_Escape;
			s_keyMap[VK_CONVERT] = GMKey_Convert;
			s_keyMap[VK_NONCONVERT] = GMKey_Nonconvert;
			s_keyMap[VK_ACCEPT] = GMKey_Accept;
			s_keyMap[VK_MODECHANGE] = GMKey_Modechange;
			s_keyMap[VK_SPACE] = GMKey_Space;
			s_keyMap[VK_PRIOR] = GMKey_Prior;
			s_keyMap[VK_NEXT] = GMKey_Next;
			s_keyMap[VK_END] = GMKey_End;
			s_keyMap[VK_HOME] = GMKey_Home;
			s_keyMap[VK_LEFT] = GMKey_Left;
			s_keyMap[VK_UP] = GMKey_Up;
			s_keyMap[VK_RIGHT] = GMKey_Right;
			s_keyMap[VK_DOWN] = GMKey_Down;
			s_keyMap[VK_SELECT] = GMKey_Select;
			s_keyMap[VK_PRINT] = GMKey_Print;
			s_keyMap[VK_EXECUTE] = GMKey_Execute;
			s_keyMap[VK_SNAPSHOT] = GMKey_Snapshot;
			s_keyMap[VK_INSERT] = GMKey_Insert;
			s_keyMap[VK_DELETE] = GMKey_Delete;
			s_keyMap[VK_HELP] = GMKey_Help;
			s_keyMap[VK_LWIN] = GMKey_Lwin;
			s_keyMap[VK_RWIN] = GMKey_Rwin;
			s_keyMap[VK_APPS] = GMKey_Apps;
			s_keyMap[VK_SLEEP] = GMKey_Sleep;
			s_keyMap[VK_NUMPAD0] = GMKey_Numpad0;
			s_keyMap[VK_NUMPAD1] = GMKey_Numpad1;
			s_keyMap[VK_NUMPAD2] = GMKey_Numpad2;
			s_keyMap[VK_NUMPAD3] = GMKey_Numpad3;
			s_keyMap[VK_NUMPAD4] = GMKey_Numpad4;
			s_keyMap[VK_NUMPAD5] = GMKey_Numpad5;
			s_keyMap[VK_NUMPAD6] = GMKey_Numpad6;
			s_keyMap[VK_NUMPAD7] = GMKey_Numpad7;
			s_keyMap[VK_NUMPAD8] = GMKey_Numpad8;
			s_keyMap[VK_NUMPAD9] = GMKey_Numpad9;
			s_keyMap[VK_MULTIPLY] = GMKey_Multiply;
			s_keyMap[VK_ADD] = GMKey_Add;
			s_keyMap[VK_SEPARATOR] = GMKey_Separator;
			s_keyMap[VK_SUBTRACT] = GMKey_Subtract;
			s_keyMap[VK_DECIMAL] = GMKey_Decimal;
			s_keyMap[VK_DIVIDE] = GMKey_Divide;
			s_keyMap[VK_F1] = GMKey_F1;
			s_keyMap[VK_F2] = GMKey_F2;
			s_keyMap[VK_F3] = GMKey_F3;
			s_keyMap[VK_F4] = GMKey_F4;
			s_keyMap[VK_F5] = GMKey_F5;
			s_keyMap[VK_F6] = GMKey_F6;
			s_keyMap[VK_F7] = GMKey_F7;
			s_keyMap[VK_F8] = GMKey_F8;
			s_keyMap[VK_F9] = GMKey_F9;
			s_keyMap[VK_F10] = GMKey_F10;
			s_keyMap[VK_F11] = GMKey_F11;
			s_keyMap[VK_F12] = GMKey_F12;
			s_keyMap[VK_F13] = GMKey_F13;
			s_keyMap[VK_F14] = GMKey_F14;
			s_keyMap[VK_F15] = GMKey_F15;
			s_keyMap[VK_F16] = GMKey_F16;
			s_keyMap[VK_F17] = GMKey_F17;
			s_keyMap[VK_F18] = GMKey_F18;
			s_keyMap[VK_F19] = GMKey_F19;
			s_keyMap[VK_F20] = GMKey_F20;
			s_keyMap[VK_F21] = GMKey_F21;
			s_keyMap[VK_F22] = GMKey_F22;
			s_keyMap[VK_F23] = GMKey_F23;
			s_keyMap[VK_F24] = GMKey_F24;
			s_keyMap[VK_NUMLOCK] = GMKey_Numlock;
			s_keyMap[VK_SCROLL] = GMKey_Scroll;
			s_keyMap[VK_LSHIFT] = GMKey_Lshift;
			s_keyMap[VK_RSHIFT] = GMKey_Rshift;
			s_keyMap[VK_LCONTROL] = GMKey_Lcontrol;
			s_keyMap[VK_RCONTROL] = GMKey_Rcontrol;
			s_keyMap[VK_LMENU] = GMKey_Lmenu;
			s_keyMap[VK_RMENU] = GMKey_Rmenu;
			s_keyMap[VK_BROWSER_BACK] = GMKey_Browser_back;
			s_keyMap[VK_BROWSER_FORWARD] = GMKey_Browser_forward;
			s_keyMap[VK_BROWSER_REFRESH] = GMKey_Browser_refresh;
			s_keyMap[VK_BROWSER_STOP] = GMKey_Browser_stop;
			s_keyMap[VK_BROWSER_SEARCH] = GMKey_Browser_search;
			s_keyMap[VK_BROWSER_FAVORITES] = GMKey_Browser_favorites;
			s_keyMap[VK_BROWSER_HOME] = GMKey_Browser_home;
			s_keyMap[VK_VOLUME_MUTE] = GMKey_Volume_mute;
			s_keyMap[VK_VOLUME_DOWN] = GMKey_Volume_down;
			s_keyMap[VK_VOLUME_UP] = GMKey_Volume_up;
			s_keyMap[VK_MEDIA_NEXT_TRACK] = GMKey_Media_next_track;
			s_keyMap[VK_MEDIA_PREV_TRACK] = GMKey_Media_prev_track;
			s_keyMap[VK_MEDIA_STOP] = GMKey_Media_stop;
			s_keyMap[VK_MEDIA_PLAY_PAUSE] = GMKey_Media_play_pause;
			s_keyMap[VK_LAUNCH_MAIL] = GMKey_Launch_mail;
			s_keyMap[VK_LAUNCH_MEDIA_SELECT] = GMKey_Launch_media_select;
			s_keyMap[VK_LAUNCH_APP1] = GMKey_Launch_app1;
			s_keyMap[VK_LAUNCH_APP2] = GMKey_Launch_app2;
			s_keyMap[VK_OEM_1] = GMKey_Oem_1;
			s_keyMap[VK_OEM_PLUS] = GMKey_Oem_plus;
			s_keyMap[VK_OEM_COMMA] = GMKey_Oem_comma;
			s_keyMap[VK_OEM_MINUS] = GMKey_Oem_minus;
			s_keyMap[VK_OEM_PERIOD] = GMKey_Oem_period;
			s_keyMap[VK_OEM_2] = GMKey_Oem_2;
			s_keyMap[VK_OEM_3] = GMKey_Oem_3;
			s_keyMap[VK_OEM_4] = GMKey_Oem_4;
			s_keyMap[VK_OEM_5] = GMKey_Oem_5;
			s_keyMap[VK_OEM_6] = GMKey_Oem_6;
			s_keyMap[VK_OEM_7] = GMKey_Oem_7;
			s_keyMap[VK_OEM_8] = GMKey_Oem_8;
			s_keyMap[VK_OEM_102] = GMKey_Oem_102;
			s_keyMap[VK_PROCESSKEY] = GMKey_Processkey;
			s_keyMap[VK_PACKET] = GMKey_Packet;
			s_keyMap[VK_ATTN] = GMKey_Attn;
			s_keyMap[VK_CRSEL] = GMKey_Crsel;
			s_keyMap[VK_EXSEL] = GMKey_Exsel;
			s_keyMap[VK_EREOF] = GMKey_Ereof;
			s_keyMap[VK_PLAY] = GMKey_Play;
			s_keyMap[VK_ZOOM] = GMKey_Zoom;
			s_keyMap[VK_NONAME] = GMKey_Noname;
			s_keyMap[VK_PA1] = GMKey_Pa1;
			s_keyMap[VK_OEM_CLEAR] = GMKey_Oem_Clear;
		}

		auto iter = s_keyMap.find(wParam);
		if (iter != s_keyMap.end())
			return iter->second;
		return GM_ASCIIToKey((GMKey)wParam);
	}

	GMMouseButton translateButton(GMWParam wParam)
	{
		GMMouseButton button = GMMouseButton_None;
		if (wParam & MK_LBUTTON)
			button |= GMMouseButton_Left;
		if (wParam & MK_MBUTTON)
			button |= GMMouseButton_Right;
		if (wParam & MK_RBUTTON)
			button |= GMMouseButton_Middle;
		return button;
	}

	GMMouseButton translateWheelButton(GMWParam wParam)
	{
		GMMouseButton button = GMMouseButton_None;
		GMshort keystate = GET_KEYSTATE_WPARAM(wParam);
		if (keystate & MK_LBUTTON)
			button |= GMMouseButton_Left;
		if (keystate & MK_MBUTTON)
			button |= GMMouseButton_Right;
		if (keystate & MK_RBUTTON)
			button |= GMMouseButton_Middle;
		return button;
	}

	GMModifier translateModifier(GMWParam wParam)
	{
		GMModifier modifier = GMModifier_None;
		if (wParam & MK_CONTROL)
			modifier |= GMModifier_Ctrl;
		if (wParam & MK_SHIFT)
			modifier |= GMModifier_Shift;
		return modifier;
	}

	GMModifier getModifierState()
	{
		GMModifier modifier = GMModifier_None;
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			modifier |= GMModifier_Shift;
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
			modifier |= GMModifier_Ctrl;
		return modifier;
	}

	void translateSystemEvent(GMuint32 uMsg, GMWParam wParam, GMLParam lParam, OUT GMSystemEvent** event)
	{
		GM_ASSERT(event);
		GMKey key;
		GMSystemEvent* newSystemEvent = nullptr;

		switch (uMsg)
		{
		case WM_DESTROY:
			newSystemEvent = new GMSystemEvent(GMSystemEventType::WindowAboutToDestroy);
			break;
		case WM_CLOSE:
			newSystemEvent = new GMSystemEvent(GMSystemEventType::WindowAboutToClose);
			break;
		case WM_SIZE:
			newSystemEvent = new GMSystemEvent(GMSystemEventType::WindowSizeChanged);
			break;
		case WM_SETCURSOR:
			newSystemEvent = new GMSystemEvent(GMSystemEventType::SetCursor);
			break;
			// Keyboard:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			key = translateKey(wParam);
			newSystemEvent = new GMSystemKeyEvent(GMSystemEventType::KeyDown, key, getModifierState());
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			key = translateKey(wParam);
			newSystemEvent = new GMSystemKeyEvent(GMSystemEventType::KeyUp, key, getModifierState());
			break;
		}
		case WM_CHAR:
		{
			key = translateKey(wParam);
			newSystemEvent = new GMSystemCharEvent(GMSystemEventType::Char, key, (GMwchar)wParam, getModifierState());
			break;
		}
		// Mouse:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		{
			GMPoint mousePoint =
			{
				(GMint32)LOWORD(lParam), (GMint32)HIWORD(lParam)
			};

			GMSystemEventType type = GMSystemEventType::Unknown;
			GMMouseButton triggeredButton = GMMouseButton_None;
			switch (uMsg)
			{
			case WM_MOUSEMOVE:
				type = GMSystemEventType::MouseMove;
				break;
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				type = GMSystemEventType::MouseDown;
				break;
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				type = GMSystemEventType::MouseUp;
				break;
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
				type = GMSystemEventType::MouseDblClick;
				break;
			}

			if (uMsg == WM_LBUTTONDOWN ||
				uMsg == WM_LBUTTONUP ||
				uMsg == WM_LBUTTONDBLCLK)
			{
				triggeredButton = GMMouseButton_Left;
			}
			else if (uMsg == WM_RBUTTONDOWN ||
				uMsg == WM_RBUTTONUP ||
				uMsg == WM_RBUTTONDBLCLK)
			{
				triggeredButton = GMMouseButton_Right;
			}
			else if (uMsg == WM_MBUTTONDOWN ||
				uMsg == WM_MBUTTONUP ||
				uMsg == WM_MBUTTONDBLCLK)
			{
				triggeredButton = GMMouseButton_Middle;
			}

			GM_ASSERT(type != GMSystemEventType::Unknown);
			newSystemEvent = new GMSystemMouseEvent(type, mousePoint, triggeredButton, translateButton(wParam), translateModifier(wParam));
			break;
		}
		case WM_MOUSEWHEEL:
		{
			GMPoint mousePoint =
			{
				(GMint32)LOWORD(lParam), (GMint32)HIWORD(lParam)
			};
			GMshort delta = GET_WHEEL_DELTA_WPARAM(wParam);
			newSystemEvent = new GMSystemMouseWheelEvent(
				mousePoint,
				GMMouseButton_None,
				translateWheelButton(wParam),
				translateModifier(wParam),
				delta
			);
			break;
		}
		case WM_CAPTURECHANGED:
		{
			newSystemEvent = new GMSystemCaptureChangedEvent((GMWindowHandle)lParam);
			break;
		}
		case WM_PAINT:
		{
			newSystemEvent = new GMSystemPaintEvent();
			break;
		}
		}

		*event = newSystemEvent;
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		GMWindow* pGMWindow = nullptr;
		if (uMsg == WM_NCCREATE)
		{
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pGMWindow = static_cast<GMWindow*>(lpcs->lpCreateParams);
			pGMWindow->setWindowHandle(hWnd, true);
			s_hwndMap[hWnd] = pGMWindow;
		}
		else
		{
			auto iter = s_hwndMap.find(hWnd);
			if (iter != s_hwndMap.end())
				pGMWindow = (*iter).second;
		}

		if (pGMWindow)
		{
			LRESULT lRes = 0;
			GMSystemEvent* sysEvent = nullptr;
			translateSystemEvent(uMsg, wParam, lParam, &sysEvent);
			GMScopedPtr<GMSystemEvent> guard(sysEvent);
			if (!pGMWindow->handleSystemEvent(sysEvent, lRes))
				return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
			return lRes;
		}
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	bool registerClass(const GMWindowDesc& wndAttrs, GMWindow* window, const GMwchar* className)
	{
		WNDCLASS wc = { 0 };
		wc.style = 0;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = NULL;
		wc.lpfnWndProc = window->getProcHandler();
		wc.hInstance = wndAttrs.instance;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = className;
		ATOM ret = ::RegisterClass(&wc);
		GM_ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

	HCURSOR getCursor(GMCursorType cursorType)
	{
		constexpr GMuint32 sz = (GMuint32)GMCursorType::EndOfEnum;
		static HCURSOR cursors[sz] = { NULL };
		if (!cursors[(GMuint32)cursorType])
		{
			LPCWSTR cursor = NULL;
			switch (cursorType)
			{
			case GMCursorType::Arrow:
				cursor = IDC_ARROW;
				break;
			case GMCursorType::IBeam:
				cursor = IDC_IBEAM;
				break;
			case GMCursorType::Wait:
				cursor = IDC_WAIT;
				break;
			case GMCursorType::Cross:
				cursor = IDC_CROSS;
				break;
			case GMCursorType::UpArrow:
				cursor = IDC_UPARROW;
				break;
			case GMCursorType::Hand:
				cursor = IDC_HAND;
				break;
			default:
				GM_ASSERT(false);
				break;
			}

			cursors[(GMuint32)cursorType] = ::LoadCursor(NULL, cursor);
		}
		return cursors[(GMuint32)cursorType];
	}
}

IWindow* GMRenderContext::getWindow() const
{
	return const_cast<IWindow*>(window);
}

IGraphicEngine* GMRenderContext::getEngine() const
{
	return const_cast<IGraphicEngine*>(engine);
}

GMWindowProcHandler GMWindow::getProcHandler()
{
	return &WndProc;
}

GMWindow::~GMWindow()
{
	D(d);
	if (d->ownedHandle)
	{
		GMWindowHandle hwnd = getWindowHandle();
		if (hwnd)
		{
			onWindowDestroyed();
			::DestroyWindow(hwnd);
		}
	}
}

IInput* GMWindow::getInputManager()
{
	D(d);
	if (!d->input)
		d->input = gm_makeOwnedPtr<GMInput>(this);
	return d->input.get();
}

GMRect GMWindow::getWindowRect()
{
	RECT rect;
	GetWindowRect(getWindowHandle(), &rect);
	GMRect r = { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
	return r;
}

GMRect GMWindow::getRenderRect()
{
	RECT rect;
	GetClientRect(getWindowHandle(), &rect);
	GMRect r = { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
	return r;
}

void GMWindow::centerWindow()
{
	GMWindowHandle hWnd = getWindowHandle();
	GM_ASSERT(::IsWindow(hWnd));
	GM_ASSERT((GetWindowStyle(hWnd)&WS_CHILD) == 0);
	RECT rcDlg = { 0 };
	::GetWindowRect(hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWndParent = ::GetParent(hWnd);
	HWND hWndCenter = ::GetWindowOwner(hWnd);
	if (hWndCenter != NULL)
		hWnd = hWndCenter;

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if (hWndCenter == NULL || IsIconic(hWndCenter))
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if (xLeft < rcArea.left) xLeft = rcArea.left;
	else if (xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
	if (yTop < rcArea.top) yTop = rcArea.top;
	else if (yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

bool GMWindow::isWindowActivate()
{
	return ::GetForegroundWindow() == getWindowHandle();
}

void GMWindow::setWindowCapture(bool capture)
{
	if (capture)
		::SetCapture(getWindowHandle());
	else
		::ReleaseCapture();
}

GMWindowHandle GMWindow::create(const GMWindowDesc& desc)
{
	D(d);
	GMWindowHandle hwnd = NULL;
	GMWindowDesc attrs = desc;
	if (desc.createNewWindow)
	{
		if (attrs.rc.width <= 0)
			attrs.rc.width = DefaultWindowWidth;
		if (attrs.rc.height <= 0)
			attrs.rc.height = DefaultWindowHeight;

		// 在非全屏的时候才有效，计算出客户窗口（渲染窗口）大小
		RECT rc = { (LONG) attrs.rc.x, (LONG) attrs.rc.y, (LONG) (attrs.rc.x + attrs.rc.width), (LONG) (attrs.rc.y + attrs.rc.height) };
		::AdjustWindowRectEx(&rc, attrs.dwStyle, FALSE, attrs.dwExStyle);
		const GMwchar* className = getWindowClassName();
		registerClass(attrs, this, className);
		hwnd = ::CreateWindowEx(
			attrs.dwExStyle,
			className,
			attrs.windowName.toStdWString().c_str(),
			attrs.dwStyle,
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			attrs.hwndParent,
			NULL,
			attrs.instance,
			this);
		GM_ASSERT(hwnd);
	}
	else
	{
		hwnd = attrs.existWindowHandle;
		setWindowHandle(attrs.existWindowHandle, false);

		// 如果没有指定大小，则拿窗口本身大小
		if (attrs.rc.width <= 0 || attrs.rc.height <= 0)
		{
			const GMRect& rc = getRenderRect();
			attrs.rc = { (GMfloat)rc.x, (GMfloat)rc.y, (GMfloat)rc.width, (GMfloat)rc.height };
		}
	}

	onWindowCreated(attrs);
	d->windowStates.renderRect = {
		static_cast<GMint32>(attrs.rc.x),
		static_cast<GMint32>(attrs.rc.y),
		static_cast<GMint32>(attrs.rc.width),
		static_cast<GMint32>(attrs.rc.height)
	};
	d->windowStates.framebufferRect = d->windowStates.renderRect;
	return hwnd;
}

void GMWindow::changeCursor()
{
	D(d);
	::SetCursor(getCursor(d->cursor));
}

void GMWindow::showWindow()
{
	D(d);
	GMWindowHandle hwnd = getWindowHandle();
	GM_ASSERT(::IsWindow(hwnd));
	if (!::IsWindow(hwnd)) return;
	::ShowWindow(hwnd, SW_SHOWNORMAL);
}

void GMWindow::onWindowDestroyed()
{
	D(d);
	if (d->handler)
	{
		d->handler->event(GameMachineHandlerEvent::Terminate);
	}

	if (d->ownedHandle)
	{
		GMWindowHandle hwnd = getWindowHandle();
		GM_ASSERT(hwnd);
		auto s = s_hwndMap.erase(hwnd);
		GM_ASSERT(s > 0);
	}
}

void GMWindow::setMultithreadRenderingFlag(GMMultithreadRenderingFlag)
{
}

END_NS