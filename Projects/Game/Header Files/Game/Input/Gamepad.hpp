#pragma once

#include <Game/XInput.hpp>

class Gamepad
{

public:

	static bool FindNextController (DWORD iFirst, DWORD& iOut);

	struct Thumb
	{
		float x, y;
	};

	enum class Side
	{
		LEFT, RIGHT
	};

	class Listener
	{

	protected:

		Listener () = default;

		friend class Gamepad;

		virtual void OnConnected (const Gamepad& controller);

		virtual void OnDisconnected (const Gamepad& controller);

		virtual void OnButtonChanged (const Gamepad& controller, WORD button, bool pressed);

		virtual void OnTriggerChanged (const Gamepad& controller, float value, Side side);

		virtual void OnThumbChanged (const Gamepad& controller, Thumb thumb, Side side);

	};

	Listener * pListener { nullptr };

	bool Update ();

	bool IsConnected () const;

	Thumb GetThumb (Side side) const;

	Thumb GetThumb (Side side, Thumb defaultValue) const;

	bool AreButtonsPressed (WORD buttons) const;

	bool AreButtonsPressed (WORD buttons, bool defaultValue) const;

	float GetTrigger (Side side) const;

	float GetTrigger (Side side, float defaultValue) const;

	void SetVibration (float lowFreqSpeed, float highFreqSpeed) const;

	DWORD GetUserIndex () const;

	void SetUserIndex (DWORD index);

	virtual ~Gamepad () = default;

protected:

	bool ProcessEvents (const XINPUT_STATE& state) const;

	void FireAllEvents () const;

private:

	DWORD m_iUser { 0 };
	bool m_bStateValid { false };
	bool m_bConnected { false };
	XINPUT_STATE m_State;

};