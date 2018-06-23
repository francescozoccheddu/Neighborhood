#pragma once

#include <string>
#include <stdexcept>

class Window
{

	void EnsureCreated () const;

public:

	class WindowError : std::runtime_error
	{

	public:
		
		enum class Cause
		{
			NOT_CREATED, NOT_RUNNING, ALREADY_CREATED, ALREADY_RUNNING
		};

	private:

		Window * const window;

		WindowError (Window * window, const Cause cause);

	public:

		Window * GetWindow () const;

		Cause GetCause () const;

		const char * GetMessage () const;

	};

	Window (const std::string& title, int w, int h, bool fullscreen, bool visible, bool resizable);

	void SetTitle (const std::string& title);

	void SetSize (int w, int h);

	void SetFullscreen (bool fullscreen);

	void SetResizable (bool resizable);

	void SetMinimized (bool minimized);

	void SetVisible (bool visible);

	std::string GetTitle () const;

	void GetSize (int& w, int& h);

	bool IsFullscreen () const;

	bool IsResizable () const;

	bool IsMinimized () const;

	bool IsVisible () const;

	void Create ();

	void Destroy ();

	void Run ();

	void Stop ();

	bool IsCreated ();

	bool IsRunning ();

	virtual ~Window ();

};

