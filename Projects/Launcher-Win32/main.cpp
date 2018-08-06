#pragma once

#include <Game/pch.hpp>

#include "Resource.h"
#include "ErrorLogger.hpp"

#include <Game/Engine/Game.hpp>
#include <Game/Engine/ResourceHandler.hpp>
#include <Game/Utils/Exceptions.hpp>

#define INITIAL_WIDTH 640
#define INITIAL_HEIGHT 480

#define MIN_WIDTH 320
#define MIN_HEIGHT 240

#define HANDLE_SIZING_ON_DRAG_RESIZING 1

#define GAME_TRY(x) { try { x; } catch(const GameException& ex) { PostError(ex.what()); } }

#ifdef _DEBUG
#define PGAME_DO(x) { if (pGame) GAME_TRY(pGame->x) }
#else
#define PGAME_DO(x) { if (pGame) pGame->x; }
#endif

#if NEIGHBORHOOD_WIN32_REQUEST_HP_GPU
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

LRESULT CALLBACK MainWinProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

Game * pGame { nullptr };

int APIENTRY wWinMain (_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	UNREFERENCED_PARAMETER (_hPrevInstance);
	UNREFERENCED_PARAMETER (_lpCmdLine);

	//init_apartment ();

	if (!DirectX::XMVerifyCPUSupport ())
	{
		PostError ("Math not supported on this CPU");
	}

	GAME_TRY (ResourceHandler::InitializeTimer ());

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
	mainClass.lpszMenuName = nullptr;
	mainClass.lpszClassName = TEXT ("Main");

	ATOM classAtom = RegisterClassEx (&mainClass);
	if (!classAtom)
	{
		PostLastError ("Unable to register window class");
	}

	pGame = new Game ();

	DWORD style { WS_OVERLAPPEDWINDOW | WS_VISIBLE };
	HWND hWnd = CreateWindow (MAKEINTATOM (classAtom), title, style, CW_USEDEFAULT, CW_USEDEFAULT, INITIAL_WIDTH, INITIAL_HEIGHT, NULL, NULL, _hInstance, NULL);

	if (!hWnd)
	{
		PostLastError ("Unable to create window");
	}

	ShowWindow (hWnd, _nCmdShow);
	UpdateWindow (hWnd);


	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
		else
		{
			PGAME_DO (Tick ());
		}
	}

	delete pGame;
	pGame = nullptr;

	//uninit_apartment ();

	return 0;
}

LRESULT CALLBACK MainWinProc (HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static bool s_bSizing = false;
	static bool s_bSuspended = false;
	static bool s_bMinimized = false;

	switch (_msg)
	{
		case WM_CREATE:
		{
			RECT rc;
			GetClientRect (_hWnd, &rc);
			PGAME_DO (SetWindow (_hWnd, { rc.right - rc.left, rc.bottom - rc.top }, DXGI_MODE_ROTATION_IDENTITY));
		}
		break;

		case WM_CLOSE:
		{
			DestroyWindow (_hWnd);
		}
		break;

		case WM_PAINT:
		{
			if (s_bSizing && pGame)
			{
				PGAME_DO (Tick ());
			}
			else
			{
				hdc = BeginPaint (_hWnd, &ps);
				EndPaint (_hWnd, &ps);
			}
		}
		break;

		case WM_SIZE:
		{
			if (_wParam == SIZE_MINIMIZED)
			{
				if (!s_bMinimized)
				{
					s_bMinimized = true;
					if (!s_bSuspended && pGame)
					{
						PGAME_DO (Suspend ());
					}
					s_bSuspended = true;
				}
			}
			else if (s_bMinimized)
			{
				s_bMinimized = false;
				if (s_bSuspended && pGame)
				{
					PGAME_DO (Resume ());
				}
				s_bSuspended = false;
			}
			else
			{
#if HANDLE_SIZING_ON_DRAG_RESIZING
				if (pGame)
#else
				if (!s_bSizing && pGame)
#endif
				{
					PGAME_DO (Size ({ LOWORD (_lParam), HIWORD (_lParam) }, DXGI_MODE_ROTATION_IDENTITY));
			}
		}
	}
		break;

		case WM_ENTERSIZEMOVE:
		{
			s_bSizing = true;
		}
		break;

		case WM_EXITSIZEMOVE:
		{
			s_bSizing = false;
#if !HANDLE_SIZING_ON_DRAG_RESIZING
			if (pGame)
			{
				RECT rc;
				GetClientRect (_hWnd, &rc);
				GAME_DO (Size ({ rc.right - rc.left, rc.bottom - rc.top }));
			}
#endif
		}
		break;

		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* pInfo = reinterpret_cast<MINMAXINFO*>(_lParam);
			pInfo->ptMinTrackSize.x = MIN_WIDTH;
			pInfo->ptMinTrackSize.y = MIN_HEIGHT;
		}
		break;

		case WM_POWERBROADCAST:
		{
			switch (_wParam)
			{
				case PBT_APMQUERYSUSPEND:
					if (!s_bSuspended && pGame)
					{
						PGAME_DO (Suspend ());
					}
					s_bSuspended = true;
					return TRUE;

				case PBT_APMRESUMESUSPEND:
					if (!s_bMinimized)
					{
						if (s_bSuspended && pGame)
						{
							PGAME_DO (Resume ());
						}
						s_bSuspended = false;
					}
					return TRUE;
			}
		}
		break;

		case WM_DESTROY:
		{

			PGAME_DO (Destroy ());
			PostQuitMessage (0);
		}
		break;

		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
		{
			DirectX::Mouse::ProcessMessage (_msg, _wParam, _lParam);
		}
		break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			DirectX::Keyboard::ProcessMessage (_msg, _wParam, _lParam);
		}
		break;

		case WM_SYSKEYDOWN:
		{
			DirectX::Keyboard::ProcessMessage (_msg, _wParam, _lParam);
		}
		break;

		case WM_MENUCHAR:
		{
			return MAKELRESULT (0, MNC_CLOSE);
		}
		}

	return DefWindowProc (_hWnd, _msg, _wParam, _lParam);
}