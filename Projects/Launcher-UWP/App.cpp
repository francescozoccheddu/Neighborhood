#include <Game/Windows.hpp>

#include <Game/Engine/Dispatcher.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <cmath>
#include <ppltasks.h>
#include <stdexcept>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Popups.h>

using namespace winrt::Windows;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::Graphics::Display;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::UI::Popups;

#define GAME_TRY(x) { try { x; } catch(const GameException& ex) { PostError(ex.what()); } }

#ifdef _DEBUG
#define PGAME_DO(x) { if (pDispatcher) GAME_TRY(pDispatcher->x) }
#else
#define PGAME_DO(x) { if (pDispatcher) pDispatcher->x; }
#endif

void PostError (const char * _msg)
{
	if (IsDebuggerPresent ())
	{
		OutputDebugStringA (_msg);
		OutputDebugStringA ("\n");
		DebugBreak ();
	}
	else
	{
		CoreApplication::Exit ();
	}
}

#define HANDLE_SIZING_WHEN_DRAG_RESIZING 0
#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
#define _CAN_DETECT_DRAG_RESIZING 1
#else
#define _CAN_DETECT_DRAG_RESIZING 0
#endif

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
	bool m_visible { true };
	bool m_exit { false };
#if _CAN_DETECT_DRAG_RESIZING && !HANDLE_SIZING_WHEN_DRAG_RESIZING
	bool m_in_sizemove { false };
#endif
	float m_dpi { 96.0f };
	float m_logicalWidth { 800.0f };
	float m_logicalHeight { 600.0f };
	DisplayOrientations m_nativeOrientation { DisplayOrientations::None };
	DisplayOrientations m_currentOrientation { DisplayOrientations::None };
	Dispatcher * pDispatcher { nullptr };


	IFrameworkView CreateView ()
	{
		return *this;
	}

	void Initialize (CoreApplicationView const& applicationView)
	{
		applicationView.Activated ({ this, &App::OnActivated });
		CoreApplication::Suspending ({ this, &App::OnSuspending });
		CoreApplication::Resuming ({ this, &App::OnResuming });

		pDispatcher = new Dispatcher ();
	}

	void SetWindow (CoreWindow const & window)
	{
#if _CAN_DETECT_DRAG_RESIZING && !HANDLE_SIZING_WHEN_DRAG_RESIZING
		try
		{
			window.ResizeStarted ({ this, &App::OnWindowResizeStarted });
			window.ResizeCompleted ({ this, &App::OnWindowResizeCompleted });
		}
		catch (...)
		{
		}
#endif

		window.SizeChanged ({ this, &App::OnWindowSizeChanged });
		window.VisibilityChanged ({ this, &App::OnWindowVisibilityChanged });

		window.Closed ({ this, &App::OnWindowClosed });
		DisplayInformation displayInfo { DisplayInformation::GetForCurrentView () };
		displayInfo.DpiChanged ({ this, &App::OnDpiChanged });
		displayInfo.OrientationChanged ({ this, &App::OnOrientationChanged });
		DisplayInformation::DisplayContentsInvalidated ({ this, &App::OnDisplayContentsInvalidated });

		m_dpi = displayInfo.LogicalDpi ();
		m_logicalWidth = window.Bounds ().Width;
		m_logicalHeight = window.Bounds ().Height;
		m_nativeOrientation = displayInfo.NativeOrientation ();
		m_currentOrientation = displayInfo.CurrentOrientation ();

		int outputWidth = ConvertDipsToPixels (m_logicalWidth);
		int outputHeight = ConvertDipsToPixels (m_logicalHeight);

		WindowRotation rotation = ComputeDisplayRotation ();

		if (rotation == WindowRotation::ROTATE_90 || rotation == WindowRotation::ROTATE_270)
		{
			std::swap (outputWidth, outputHeight);
		}

		PGAME_DO (SetWindow (winrt::get_unknown (window), { outputWidth, outputHeight }, rotation));

	}

	void Load (hstring /*entry*/)
	{}

	void Run ()
	{
		while (!m_exit)
		{
			if (m_visible)
			{
				CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (CoreProcessEventsOption::ProcessAllIfPresent);
				PGAME_DO (Tick ());
			}
			else
			{
				CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (CoreProcessEventsOption::ProcessOneAndAllPending);
			}
		}
	}

	void Uninitialize ()
	{
		delete pDispatcher;
		pDispatcher = nullptr;
	}

protected:

	void OnActivated (CoreApplicationView const & /* applicationView */, IActivatedEventArgs const & args)
	{
		m_dpi = DisplayInformation::GetForCurrentView ().LogicalDpi ();
		CoreWindow::GetForCurrentThread ().Activate ();
	}

	void OnSuspending (IInspectable const & /*_sender*/, SuspendingEventArgs const & args)
	{
		SuspendingDeferral deferral = args.SuspendingOperation ().GetDeferral ();
		concurrency::create_task ([this, deferral]
		{
			PGAME_DO (Suspend ());
			deferral.Complete ();
		});
	}

	void OnResuming (IInspectable const & /*_sender*/, IInspectable const &)
	{
		PGAME_DO (Resume ());
	}

	void OnWindowSizeChanged (CoreWindow const & window, WindowSizeChangedEventArgs const & args)
	{
		m_logicalWidth = window.Bounds ().Width;
		m_logicalHeight = window.Bounds ().Height;
#if !HANDLE_SIZING_WHEN_DRAG_RESIZING
		if (!m_in_sizemove)
#endif
		{
			HandleWindowSizeChanged ();
		}
	}

#if _CAN_DETECT_DRAG_RESIZING && !HANDLE_SIZING_WHEN_DRAG_RESIZING
	void OnWindowResizeStarted (CoreWindow const &, IInspectable const &)
	{
		m_in_sizemove = true;
	}

	void OnWindowResizeCompleted (CoreWindow const &, IInspectable const &)
	{
		m_in_sizemove = false;
		HandleWindowSizeChanged ();
	}
#endif

	void OnWindowVisibilityChanged (CoreWindow const & /* sender */, VisibilityChangedEventArgs const & args)
	{
		m_visible = args.Visible ();
	}


	void OnWindowClosed (CoreWindow const &, CoreWindowEventArgs const &)
	{
		m_exit = true;
		PGAME_DO (Destroy ());
	}

	void OnDpiChanged (DisplayInformation const & sender, IInspectable const &)
	{
		m_dpi = sender.LogicalDpi ();
		HandleWindowSizeChanged ();
	}

	void OnOrientationChanged (DisplayInformation const & sender, IInspectable const &)
	{
		CoreWindowResizeManager const & resizeManager = CoreWindowResizeManager::GetForCurrentView ();
		resizeManager.ShouldWaitForLayoutCompletion (true);

		m_currentOrientation = sender.CurrentOrientation ();

		HandleWindowSizeChanged ();

		resizeManager.NotifyLayoutCompleted ();
	}

	void OnDisplayContentsInvalidated (DisplayInformation const &, IInspectable const &)
	{
		PGAME_DO (ValidateDevice ());
		PGAME_DO (Tick ());
	}

private:

	void HandleWindowSizeChanged ()
	{
		int outputWidth = ConvertDipsToPixels (m_logicalWidth);
		int outputHeight = ConvertDipsToPixels (m_logicalHeight);

		WindowRotation rotation = ComputeDisplayRotation ();

		if (rotation == WindowRotation::ROTATE_90 || rotation == WindowRotation::ROTATE_270)
		{
			std::swap (outputWidth, outputHeight);
		}

		PGAME_DO (Size ({ outputWidth, outputHeight }, rotation));
	}

	inline int ConvertDipsToPixels (float dips) const
	{
		return int (dips * m_dpi / 96.f + 0.5f);
	}

	WindowRotation ComputeDisplayRotation () const
	{
		WindowRotation rotation;

		switch (m_nativeOrientation)
		{
			case DisplayOrientations::Landscape:
				switch (m_currentOrientation)
				{
					case DisplayOrientations::Landscape:
						rotation = WindowRotation::IDENTITY;
						break;

					case DisplayOrientations::Portrait:
						rotation = WindowRotation::ROTATE_270;
						break;

					case DisplayOrientations::LandscapeFlipped:
						rotation = WindowRotation::ROTATE_180;
						break;

					case DisplayOrientations::PortraitFlipped:
						rotation = WindowRotation::ROTATE_90;
						break;
				}
				break;

			case DisplayOrientations::Portrait:
				switch (m_currentOrientation)
				{
					case DisplayOrientations::Landscape:
						rotation = WindowRotation::ROTATE_90;
						break;

					case DisplayOrientations::Portrait:
						rotation = WindowRotation::IDENTITY;
						break;

					case DisplayOrientations::LandscapeFlipped:
						rotation = WindowRotation::ROTATE_270;
						break;

					case DisplayOrientations::PortraitFlipped:
						rotation = WindowRotation::ROTATE_180;
						break;
				}
				break;
		}

		return rotation;
	}


};

int WINAPI wWinMain (HINSTANCE, HINSTANCE, PWSTR, int)
{
	GAME_TRY (Dispatcher::Initialize ());
	CoreApplication::Run (App ());
	return 0;
}

