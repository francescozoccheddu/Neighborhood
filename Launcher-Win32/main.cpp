#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "resource.h"
#include <stdexcept>
#include <strsafe.h>
#include <Game/Game.hpp>
#include <Game/Exceptions.hpp>

#define MAX_DELTA_TIME 1.0
#ifdef _DEBUG
#define CATCH_AND_LOG(x) { try { x; } catch(const GameException& ex) { LogMessage(ex.what()); } }
#else
#define CATCH_AND_LOG(x) { x; }
#endif

void LogMessage (const char * msg);

void LogMessage (const char * _msg)
{
	if (IsDebuggerPresent ())
	{
		OutputDebugStringA (_msg);
		OutputDebugStringA ("\n");
		DebugBreak ();
	}
	else
	{
		MessageBoxA (nullptr, _msg, "LogMessage", MB_OK | MB_ICONERROR);
	}
}

Game game;

void PrintError (HWND hwnd, HINSTANCE hInstance, LPCTSTR title, UINT errorStringId, DWORD error);

LRESULT CALLBACK MainWinProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int APIENTRY wWinMain (_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	LPCTSTR title;
	if (LoadString (_hInstance, IDS_TITLE, (LPTSTR)&title, 0) <= 0)
	{
		PrintError (NULL, _hInstance, TEXT ("Unknown title"), IDS_ERRD_TITLE_NOT_LOADED, GetLastError ());
		return -1;
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
		PrintError (NULL, _hInstance, title, IDS_ERRD_CLASS_NOT_REGISTERED, GetLastError ());
		return -1;
	}

	DWORD style{ WS_OVERLAPPEDWINDOW | WS_VISIBLE };
	HWND hWnd = CreateWindow (MAKEINTATOM (classAtom), title, style, CW_USEDEFAULT, CW_USEDEFAULT, 600, 480, NULL, NULL, _hInstance, NULL);

	if (!hWnd)
	{
		PrintError (NULL, _hInstance, title, IDS_ERRD_WINDOW_NOT_CREATED, GetLastError ());
		return -1;
	}

	ShowWindow (hWnd, _nCmdShow);
	UpdateWindow (hWnd);


	bool running{ true };
	int returnValue{ 0 };
	while (running)
	{
		MSG msg;
		while (PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
			if (msg.message == WM_QUIT)
			{
				returnValue = static_cast<int>(msg.wParam);
				running = false;
			}
		}
		if (running)
		{
			CATCH_AND_LOG (game.OnTick ());
		}
	}

	return returnValue;
}

void PrintError (HWND _hWnd, HINSTANCE _hInstance, LPCTSTR _title, UINT _descStringId, DWORD _error)
{
	LPCTSTR descPtr;
	size_t descLen = LoadString (_hInstance, _descStringId, (LPTSTR)&descPtr, 0);
	LPTSTR msgPtr;
	DWORD msgLen{ FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, _error, 0, (LPTSTR)&msgPtr, 0, NULL) };
	int bufLen = descLen + msgLen + 2;
	LPTSTR buf = static_cast<LPTSTR>(HeapAlloc (GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, sizeof (TCHAR) * bufLen));
	StringCchPrintf (buf, bufLen, TEXT ("%s:\n%s"), descPtr, msgPtr);
	LocalFree (msgPtr);
	MessageBox (_hWnd, buf, _title, MB_OK | MB_ICONERROR);
	LocalFree (buf);
}

LRESULT CALLBACK MainWinProc (HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_msg)
	{
		case WM_CREATE:
		{
			CATCH_AND_LOG (game.OnWindowChanged (_hWnd));
			break;
		}
		case WM_SHOWWINDOW:
		{
			BOOL shown{ static_cast<BOOL>(_wParam) };
			if (shown)
			{
				CATCH_AND_LOG (game.OnShow ());
			}
			else
			{
				CATCH_AND_LOG (game.OnHide ());
			}
			break;
		}
		case WM_SIZE:
		{
			WindowSize size;
			size.width = static_cast<int>(LOWORD (_lParam));
			size.height = static_cast<int>(HIWORD (_lParam));
			if (size.width > 0 && size.height > 0)
			{
				CATCH_AND_LOG (game.OnSize (size));
			}
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow (_hWnd);
			break;
		}
		case WM_DESTROY:
		{
			CATCH_AND_LOG (game.OnDestroy ());
			PostQuitMessage (0);
			break;
		}
		default:
		{
			return DefWindowProc (_hWnd, _msg, _wParam, _lParam);
		}
	}
	return 0;
}