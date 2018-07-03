#pragma once

#include <windef.h>
#include <string>
#include <stdexcept>

class Window
{

private:

	static ATOM classAtom;

	static LRESULT CALLBACK MainWinProc (HWND, UINT, WPARAM, LPARAM);

	HWND hwnd{ nullptr };
	bool running{ false };

	void EnsureCreated () const;

public:

	static const int defaultPos;

	static void Register ();

	static void Unregister ();

	class WindowError : std::runtime_error
	{

	public:

		enum class Cause
		{
			NOT_CREATED, NOT_RUNNING, ALREADY_CREATED, ALREADY_RUNNING
		};

	private:

		const Window * const window;
		const Cause cause;

	public:

		WindowError (const Window * window, const Cause cause);

		const Window * GetWindow () const;

		Cause GetCause () const;

		static const char * GetCauseMessage (Cause cause);

	};

	int minW, minH, maxW, maxH;

	Window ();

	void SetTitle (const std::string& title);

	void SetSize (int w, int h);

	void SetPosition (int x, int y);

	void SetResizable (bool resizable);

	void SetMinimized (bool minimized);

	void SetSystemButtons (bool enabled, bool minimize, bool maximize);

	void SetVisible (bool visible);

	void SetIcon (HICON icon);

	std::string GetTitle () const;

	void GetSize (int& w, int& h) const;

	void GetPosition (int& x, int& y) const;

	bool IsResizable () const;

	bool IsMinimized () const;

	void GetSystemButtons (bool& enabled, bool& minimize, bool& maximize) const;

	bool IsVisible () const;

	HICON GetIcon () const;

	void Create (const std::string& title, int x, int y, int w, int h, bool resizable, bool buttons, bool minimizable, bool maximizable);

	void Create ();

	void Destroy ();

	void Run ();

	void Stop ();

	bool IsCreated ();

	bool IsRunning ();

	virtual ~Window ();

};

