#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "resource.h"
#include <stdexcept>
#include <strsafe.h>
#include <Framework/View.h>
#include <Neighborhood/GameView.h>

View::Listener * listener;

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

	listener = new GameView ();

	DWORD style{ WS_OVERLAPPEDWINDOW | WS_VISIBLE };
	HWND hWnd = CreateWindow (MAKEINTATOM (classAtom), title, style, CW_USEDEFAULT, CW_USEDEFAULT, 600, 480, NULL, NULL, _hInstance, NULL);
	if (!hWnd)
	{
		PrintError (NULL, _hInstance, title, IDS_ERRD_WINDOW_NOT_CREATED, GetLastError ());
		delete listener;
		return -1;
	}

	ShowWindow (hWnd, _nCmdShow);
	UpdateWindow (hWnd);

	MSG msg;
	BOOL bRes;
	while ((bRes = GetMessage (&msg, NULL, 0, 0)) != 0)
	{
		if (bRes == -1)
		{
			PrintError (hWnd, _hInstance, title, IDS_ERRD_ERROR_IN_MESSAGE_LOOP, GetLastError ());
			break;
		}
		else
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	delete listener;

	return static_cast<int>(msg.wParam);
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

LRESULT CALLBACK MainWinProc (HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		case WM_CREATE:
		{
			listener->Create ();
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow (_hwnd);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage (0);
			listener->Destroy ();
			break;
		}
		default:
		{
			return DefWindowProc (_hwnd, _msg, _wparam, _lparam);
		}
	}
	return 0;
}