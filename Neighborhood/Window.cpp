#include "Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>
#include <stdexcept>

#define SET_BIT_IF(word,bit,test) {word = (test) ? ((word) | (bit)) : ((word) & ~(bit));}

Window::WindowError::WindowError (const Window * _window, const Cause _cause) : runtime_error{ GetCauseMessage (_cause) }, window{ _window }, cause{ _cause }
{}

const Window * Window::WindowError::GetWindow () const
{
	return window;
}

Window::WindowError::Cause Window::WindowError::GetCause () const
{
	return cause;
}

const char * Window::WindowError::GetCauseMessage (Window::WindowError::Cause cause)
{
	switch (cause)
	{
		case Window::WindowError::Cause::NOT_CREATED:
			return "Window not created";
		case Window::WindowError::Cause::NOT_RUNNING:
			return "Window not running";
		case Window::WindowError::Cause::ALREADY_CREATED:
			return "Window already created";
		case Window::WindowError::Cause::ALREADY_RUNNING:
			return "Window already running";
		default:
			return "Unknown error";
	}
}


LRESULT CALLBACK Window::MainWinProc (HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI{ reinterpret_cast<LPMINMAXINFO>(_lparam) };
			const Window * window{ reinterpret_cast<const Window *>(GetWindowLongPtr (_hwnd, GWL_USERDATA)) };
			if (window)
			{
				lpMMI->ptMinTrackSize.x = window->minW;
				lpMMI->ptMinTrackSize.y = window->minH;
				lpMMI->ptMaxTrackSize.x = window->maxW;
				lpMMI->ptMaxTrackSize.y = window->maxH;
			}
		}
		break;
		case WM_CREATE:
		{
			LPCREATESTRUCT createStruct{ reinterpret_cast<LPCREATESTRUCT>(_lparam) };
			SetWindowLongPtr (_hwnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
		}
		break;
		case WM_CLOSE:
			DestroyWindow (_hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage (0);
			break;
		default:
			return DefWindowProc (_hwnd, _msg, _wparam, _lparam);
	}
	return 0;
}

void Window::EnsureCreated () const
{
	if (!hwnd)
	{
		throw WindowError (this, WindowError::Cause::NOT_CREATED);
	}
}

ATOM Window::classAtom{ 0 };

const int Window::defaultPos{ CW_USEDEFAULT };

void Window::Register ()
{
	if (!classAtom)
	{
		WNDCLASS mainClass;
		mainClass.style = CS_VREDRAW | CS_HREDRAW;
		mainClass.lpfnWndProc = &Window::MainWinProc;
		mainClass.cbClsExtra = 0;
		mainClass.cbWndExtra = 0;
		mainClass.hInstance = NULL;
		mainClass.hIcon = NULL;
		mainClass.hCursor = LoadCursor (NULL, IDC_ARROW);
		mainClass.hbrBackground = GetSysColorBrush (COLOR_WINDOW);
		mainClass.lpszMenuName = NULL;
		mainClass.lpszClassName = TEXT ("Main");
		classAtom = RegisterClass (&mainClass);
		if (!classAtom)
		{
			throw std::runtime_error ("Unable to register Win32 window class");
		}
	}
}

void Window::Unregister ()
{
	if (classAtom)
	{
		UnregisterClass (MAKEINTATOM (classAtom), NULL);
		classAtom = 0;
	}
}

Window::Window () : minW{ GetSystemMetrics (SM_CXMINTRACK) }, minH{ GetSystemMetrics (SM_CYMINTRACK) }, maxW{ GetSystemMetrics (SM_CXMAXTRACK) }, maxH{ GetSystemMetrics (SM_CYMAXTRACK) }
{}

void Window::SetTitle (const std::string & title)
{
	EnsureCreated ();
	SetWindowTextA (hwnd, title.c_str ());
}

void Window::SetSize (int w, int h)
{
	EnsureCreated ();
	if (w < 0 || h < 0)
	{
		throw std::invalid_argument ("Size components cannot be negative");
	}
	SetWindowPos (hwnd, NULL, 0, 0, w, h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREPOSITION);
}

void Window::SetPosition (int x, int y)
{
	EnsureCreated ();
	if (x < 0 || y < 0)
	{
		throw std::invalid_argument ("Position components cannot be negative");
	}
	SetWindowPos (hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void Window::SetResizable (bool resizable)
{
	EnsureCreated ();
	LONG style{ GetWindowLong (hwnd, GWL_STYLE) };
	SET_BIT_IF (style, WS_SIZEBOX, resizable);
	SetWindowLong (hwnd, GWL_STYLE, style);
}

void Window::SetMinimized (bool minimized)
{
	EnsureCreated ();
	ShowWindow (hwnd, minimized ? SW_MINIMIZE : SW_SHOW);
}

void Window::SetSystemButtons (bool enabled, bool minimize, bool maximize)
{
	EnsureCreated ();
	LONG style{ GetWindowLong (hwnd, GWL_STYLE) };
	SET_BIT_IF (style, WS_SYSMENU, enabled);
	SET_BIT_IF (style, WS_MINIMIZEBOX, minimize);
	SET_BIT_IF (style, WS_MAXIMIZEBOX, maximize);
	SetWindowLong (hwnd, GWL_STYLE, style);
}

void Window::SetVisible (bool visible)
{
	EnsureCreated ();
	ShowWindow (hwnd, visible ? SW_SHOW : SW_HIDE);
}

void Window::SetIcon (HICON icon)
{
	EnsureCreated ();
	SendMessage (hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
	SendMessage (hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
}

std::string Window::GetTitle () const
{
	EnsureCreated ();
	std::string buf;
	buf.reserve (GetWindowTextLengthA (hwnd));
	GetWindowTextA (hwnd, &buf[0], buf.capacity ());
	return buf;
}

void Window::GetSize (int & w, int & h) const
{}

void Window::GetPosition (int & x, int & y) const
{}

bool Window::IsResizable () const
{
	return false;
}

bool Window::IsMinimized () const
{
	return false;
}

void Window::GetSystemButtons (bool& enabled, bool & minimize, bool & maximize) const
{}

bool Window::IsVisible () const
{
	return false;
}

HICON Window::GetIcon () const
{
	return HICON ();
}

void Window::Create (const std::string & _title, int _x, int _y, int _w, int _h, bool _resizable, bool _buttons, bool _minimizable, bool _maximizable)
{
	if (IsCreated ())
	{
		throw WindowError (this, WindowError::Cause::ALREADY_CREATED);
	}
	if ((_x < 0 && _x != defaultPos) || (_y < 0 && _y != defaultPos))
	{
		throw std::invalid_argument ("Position components cannot be negative");
	}
	if (_w < 0 || _h < 0)
	{
		throw std::invalid_argument ("Size components cannot be negative");
	}
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	if (_resizable)
	{
		style |= WS_SIZEBOX;
	}
	if (_minimizable)
	{
		style |= WS_MINIMIZEBOX;
	}
	if (_maximizable)
	{
		style |= WS_MAXIMIZEBOX;
	}
	Register ();
	hwnd = CreateWindowA (MAKEINTATOM (classAtom), _title.c_str (), style, _x, _y, _w, _h, NULL, NULL, GetModuleHandle (NULL), static_cast<LPVOID>(this));
	if (!hwnd)
	{
		throw std::runtime_error ("Unable to create window");
	}

}

void Window::Create ()
{
	Create ("", defaultPos, defaultPos, 360, 240, true, true, true, true);
}

void Window::Destroy ()
{
	EnsureCreated ();
	DestroyWindow (hwnd);
	hwnd = nullptr;
}

void Window::Run ()
{
	if (IsRunning ())
	{
		throw WindowError (this, WindowError::Cause::ALREADY_RUNNING);
	}
	running = true;
	MSG msg;
	BOOL bRes;
	while (running && (bRes = GetMessage (&msg, NULL, 0, 0)) != 0)
	{
		if (bRes == -1)
		{
			running = false;
			throw std::runtime_error ("Error message in window loop");
		}
		else
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}
	running = false;
}

void Window::Stop ()
{
	if (!IsRunning ())
	{
		throw WindowError (this, WindowError::Cause::NOT_RUNNING);
	}
	running = false;
}

bool Window::IsCreated ()
{
	return static_cast<bool>(hwnd);
}

bool Window::IsRunning ()
{
	return running;
}

Window::~Window ()
{
	if (IsCreated ())
	{
		Destroy ();
	}
}
