#include <Game/pch.hpp>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Graphics.Display.h>

using namespace winrt::Windows;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::Graphics::Display;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::Foundation::Numerics;

#include <cmath>
#include <ppltasks.h>

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
	bool m_visible;
	bool m_windowClosed { false };

	IFrameworkView CreateView ()
	{
		return *this;
	}

	void Initialize (CoreApplicationView const& applicationView)
	{
		applicationView.Activated ({ this, &App::OnActivated });
		CoreApplication::Suspending ({ this, &App::OnSuspending });
		CoreApplication::Resuming ({ this, &App::OnResuming });
	}

	void SetWindow (CoreWindow const & window)
	{
		/*if (!m_pResourceHandler)
		{
			m_pResourceHandler = new ResourceHandler ();
			m_pResourceHandler->pListener = new GameListener (*m_pResourceHandler);
		}
		m_pResourceHandler->SetWindow (winrt::get_unknown (window), ResourceHandler::Platform::UWP);
		WindowSize size;
		size.width = static_cast<int>(std::round (window.Bounds ().Width));
		size.height = static_cast<int>(std::round (window.Bounds ().Height));
		m_pResourceHandler->Size (size);*/

		window.Activated ({ this, &App::OnWindowActivationChanged });
		window.SizeChanged ({ this, &App::OnWindowSizeChanged });
		window.VisibilityChanged ({ this, &App::OnWindowVisibilityChanged });

		window.Closed ({ this, &App::OnWindowClosed });
		DisplayInformation displayInfo { DisplayInformation::GetForCurrentView () };
		displayInfo.DpiChanged ({ this, &App::OnDpiChanged });
		displayInfo.OrientationChanged ({ this, &App::OnOrientationChanged });
		DisplayInformation::DisplayContentsInvalidated ({ this, &App::OnDisplayContentsInvalidated });
	}

	void OnActivated (CoreApplicationView const & /* applicationView */, IActivatedEventArgs const & /* args */)
	{
		CoreWindow::GetForCurrentThread ().Activate ();
	}

	void OnSuspending (IInspectable const & /*_sender*/, SuspendingEventArgs const & args)
	{
		SuspendingDeferral deferral = args.SuspendingOperation ().GetDeferral ();
		concurrency::create_task ([this, deferral]
		{
			/*if (m_pResourceHandler)
			{
				m_pResourceHandler->Trim ();
			}*/
			deferral.Complete ();
		});
	}

	void OnResuming (IInspectable const & /*_sender*/, IInspectable const &)
	{

	}

	void OnWindowActivationChanged (CoreWindow const & /*_sender*/, WindowActivatedEventArgs const &)
	{

	}

	void OnWindowSizeChanged (CoreWindow const & /*window*/, WindowSizeChangedEventArgs const & args)
	{
		/*WindowSize size;
		size.width = static_cast<int>(std::round (args.Size ().Width));
		size.height = static_cast<int>(std::round (args.Size ().Height));
		m_pResourceHandler->Size (size);*/
	}

	void OnWindowVisibilityChanged (CoreWindow const & /* sender */, VisibilityChangedEventArgs const & args)
	{
		m_visible = args.Visible ();
	}


	void OnWindowClosed (CoreWindow const &, CoreWindowEventArgs const &)
	{
		//m_pResourceHandler->Destroy ();
	}

	void OnDpiChanged (DisplayInformation const &, IInspectable const &)
	{

	}

	void OnOrientationChanged (DisplayInformation const &, IInspectable const &)
	{

	}

	void OnDisplayContentsInvalidated (DisplayInformation const &, IInspectable const &)
	{
		/*if (m_pResourceHandler)
		{
			m_pResourceHandler->Tick ();
		}*/
	}

	void Load (hstring /*entry*/)
	{
		/*if (!m_pResourceHandler)
		{
			m_pResourceHandler = new ResourceHandler ();
			m_pResourceHandler->pListener = new GameListener (*m_pResourceHandler);
		}*/
	}

	void Run ()
	{
		while (!m_windowClosed)
		{
			if (m_visible)
			{
				CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (CoreProcessEventsOption::ProcessAllIfPresent);
				//m_pResourceHandler->Tick ();
			}
			else
			{
				CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (CoreProcessEventsOption::ProcessOneAndAllPending);
			}
		}
		//m_pResourceHandler->Trim ();
	}

	void Uninitialize ()
	{
		/*if (m_pResourceHandler)
		{
			delete m_pResourceHandler->pListener;
			delete m_pResourceHandler;
			m_pResourceHandler = nullptr;
		}*/
	}

};

int WINAPI wWinMain (HINSTANCE, HINSTANCE, PWSTR, int)
{
	CoreApplication::Run (App ());
}

