#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "resource.h"
#include <strsafe.h>
#include <string>
#include <sstream>
#include <Game/Engine/ResourceHandler.hpp>
#include <Game/Engine/GameListener.hpp>
#include <Game/Utils/Exceptions.hpp>

#define SIZE_WHEN_RESIZING 1
#define TICK_WHEN_RESIZING 1
#define MIN_WIDTH 300
#define MIN_HEIGHT 200


#ifdef _DEBUG
#define CATCH_AND_WARN(x) { try { x; } catch(const GameException& ex) { PostError(ex.what()); } }
#else
#define CATCH_AND_WARN(x) { x; }
#endif

std::string GetErrorMessage (DWORD error);

void PostError (const char* message);

void PostError (const char * message, DWORD error);

void PostLastError (const char * message);

std::string GetErrorMessage (DWORD _error)
{
	LPSTR msgPtr;
	const DWORD msgLen { FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, _error, 0, reinterpret_cast<LPSTR>(&msgPtr), 0, nullptr) };
	std::string msg { msgPtr, msgPtr + msgLen };
	LocalFree (msgPtr);
	return msg;
}

void PostError (const char * _message)
{
	if (IsDebuggerPresent ())
	{
		OutputDebugStringA (_message);
		OutputDebugStringA ("\n");
		DebugBreak ();
	}
	else
	{
		if (MessageBoxA (nullptr, _message, SOLUTIONNAME " - " "Error happened", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
		{
			ExitProcess (-1);
		}
	}
}

void PostError (const char * _message, DWORD _error)
{
	std::stringstream ss;
	ss << _message;
	ss << "\n";
	ss << GetErrorMessage (_error);
	PostError (ss.str ().c_str ());
}

void PostLastError (const char * _message)
{
	PostError (_message, GetLastError ());
}


ResourceHandler gResourceHandler;

LRESULT CALLBACK MainWinProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int APIENTRY wWinMain (_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	gResourceHandler.pListener = new GameListener (gResourceHandler);

	LPCTSTR title;
	if (LoadString (_hInstance, IDS_TITLE, (LPTSTR) &title, 0) <= 0)
	{
		PostLastError ("Unable to load window title from resources");
	}

	WNDCLASSEX mainClass;
	mainClass.style = CS_VREDRAW | CS_HREDRAW;
	mainClass.cbSize = sizeof (WNDCLASSEX);
	mainClass.lpfnWndProc = &MainWinProc;
	mainClass.cbClsExtra = 0;
	mainClass.cbWndExtra = 0;
	mainClass.hInstance = _hInstance;
	mainClass.hIcon = LoadIcon (_hInstance, MAKEINTRESOURCE (IDI_ICON));
	mainClass.hIconSm = LoadIcon (_hInstance, MAKEINTRESOURCE (IDI_ICON_SM));
	mainClass.hCursor = LoadCursor (NULL, IDC_ARROW);
	mainClass.hbrBackground = GetSysColorBrush (COLOR_WINDOW);
	mainClass.lpszMenuName = NULL;
	mainClass.lpszClassName = TEXT ("Main");

	ATOM classAtom = RegisterClassEx (&mainClass);
	if (!classAtom)
	{
		PostLastError ("Unable to register window class");
	}

	DWORD style { WS_OVERLAPPEDWINDOW | WS_VISIBLE };
	HWND hWnd = CreateWindow (MAKEINTATOM (classAtom), title, style, CW_USEDEFAULT, CW_USEDEFAULT, 600, 480, NULL, NULL, _hInstance, NULL);

	if (!hWnd)
	{
		PostLastError ("Unable to create window");
	}

	ShowWindow (hWnd, _nCmdShow);
	UpdateWindow (hWnd);


	bool bRunning { true };
	int returnValue { 0 };
	while (bRunning)
	{
		MSG msg;
		while (PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
			if (msg.message == WM_QUIT)
			{
				returnValue = static_cast<int>(msg.wParam);
				bRunning = false;
			}
		}
		if (bRunning)
		{
			CATCH_AND_WARN (gResourceHandler.Tick ());
		}
	}

	delete gResourceHandler.pListener;

	return returnValue;
}

LRESULT CALLBACK MainWinProc (HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
#if !SIZE_WHEN_RESIZING
	static bool bSizing { false };
#endif
	switch (_msg)
	{
#if !SIZE_WHEN_RESIZING
		case WM_ENTERSIZEMOVE:
		{
			bSizing = true;
		}
		break;
		case WM_EXITSIZEMOVE:
		{
			bSizing = false;
			WindowSize size;
			RECT rect;
			GetClientRect (_hWnd, &rect);
			size.width = static_cast<int>(rect.right - rect.left);
			size.height = static_cast<int>(rect.bottom - rect.top);
			CATCH_AND_WARN (resourceHandler.Size (size));
		}
		break;
#endif
#if TICK_WHEN_RESIZING
		case WM_PAINT:
			gResourceHandler.Tick ();
			break;
#endif
		case WM_SIZE:
		{
			if (_wParam == SIZE_MINIMIZED)
			{
				gResourceHandler.Suspend ();
			}
			else
			{
				WindowSize size;
				size.width = static_cast<int>(LOWORD (_lParam));
				size.height = static_cast<int>(HIWORD (_lParam));
#if !SIZE_WHEN_RESIZING
				if (!bSizing)
#endif
					CATCH_AND_WARN (gResourceHandler.Size (size));
			}
		}
		break;
		case WM_CREATE:
		{
			CATCH_AND_WARN (gResourceHandler.SetWindow (_hWnd, ResourceHandler::Platform::Win32));
		}
		break;
		case WM_CLOSE:
		{
			DestroyWindow (_hWnd);
		}
		break;
		case WM_DESTROY:
		{
			CATCH_AND_WARN (gResourceHandler.Destroy ());
			PostQuitMessage (0);
		}
		break;
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* pInfo = reinterpret_cast<MINMAXINFO*>(_lParam);
			pInfo->ptMinTrackSize.x = MIN_WIDTH;
			pInfo->ptMinTrackSize.y = MIN_HEIGHT;
		}
		break;
		default:
		{
			return DefWindowProc (_hWnd, _msg, _wParam, _lParam);
		}
	}
	return 0;
}
