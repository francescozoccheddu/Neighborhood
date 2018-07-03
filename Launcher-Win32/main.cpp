#include <Windows.h>
#include "resource.h"
#include <stdexcept>

#define PRINTLASTERR(title) {PrintError (title, GetLastError ());}

LPCTSTR TryLoadString (HINSTANCE hInstance, UINT id);

void PrintError (LPCTSTR title, DWORD error);

LRESULT CALLBACK MainWinProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int APIENTRY wWinMain (_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	LPCTSTR title{ TryLoadString (_hInstance, IDS_TITLE) };
	if (!title)
	{
		PRINTLASTERR (TEXT ("Unknown title"));
	}


	WNDCLASSEX mainClass;
	mainClass.style = CS_VREDRAW | CS_HREDRAW;
	mainClass.cbSize = sizeof (WNDCLASSEX);
	mainClass.lpfnWndProc = &MainWinProc;
	mainClass.cbClsExtra = 0;
	mainClass.cbWndExtra = 0;
	mainClass.hInstance = _hInstance;
	mainClass.hIcon = LoadIcon (_hInstance, MAKEINTRESOURCE(IDI_ICON));
	mainClass.hIconSm = LoadIcon (_hInstance, MAKEINTRESOURCE(IDI_ICON_SM));
	mainClass.hCursor = LoadCursor (NULL, IDC_ARROW);
	mainClass.hbrBackground = GetSysColorBrush (COLOR_WINDOW);
	mainClass.lpszMenuName = NULL;
	mainClass.lpszClassName = TEXT ("Main");

	ATOM classAtom = RegisterClassEx (&mainClass);
	if (!classAtom)
	{
		PRINTLASTERR(title);
		return -1;
	}

	DWORD style{ WS_OVERLAPPEDWINDOW | WS_VISIBLE };
	HWND hwnd = CreateWindow (MAKEINTATOM (classAtom), title, style, CW_USEDEFAULT, CW_USEDEFAULT, 600, 480, NULL, NULL, GetModuleHandle (NULL), NULL);
	if (!hwnd)
	{
		PRINTLASTERR (title);
		return -1;
	}

	ShowWindow (hwnd, _nCmdShow);
	UpdateWindow (hwnd);

	MSG msg;
	BOOL bRes;
	while ((bRes = GetMessage (&msg, NULL, 0, 0)) != 0)
	{
		if (bRes == -1)
		{
			PRINTLASTERR (title);
			break;
		}
		else
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	return (int)msg.wParam;
}

LPCTSTR TryLoadString (HINSTANCE _hInstance, UINT _id)
{
	LPCTSTR ptr;
	if (LoadString (_hInstance, _id, (LPTSTR)&ptr, 0) <= 0)
	{
		ptr = nullptr;
	}
	return ptr;
}

void PrintError (LPCTSTR _title, DWORD _error)
{
	LPTSTR messagePtr;
	if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, _error, 0, (LPTSTR)&messagePtr, 0, NULL) > 0)
	{
		MessageBox (NULL, messagePtr, _title, MB_OK | MB_ICONERROR);
		LocalFree (messagePtr);
	}
}

LRESULT CALLBACK MainWinProc (HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		case WM_CREATE:
		{
		}
		break;
		case WM_CLOSE:
		{
			DestroyWindow (_hwnd);
		}
		break;
		case WM_DESTROY:
		{
			PostQuitMessage (0);
		}
		break;
		default:
			return DefWindowProc (_hwnd, _msg, _wparam, _lparam);
	}
	return 0;
}