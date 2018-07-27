#include <Game/Input/Gamepad.hpp>

#include <Xinput.h>

#pragma comment(lib,"Xinput.lib")

#include <winerror.h>
#include <climits>
#include <stdexcept>

#define MAX_THUMB_VALUE 32768.0f
#define MAX_TRIGGER_VALUE 255.0f
#define MAX_VIBRATION_VALUE 65535
#define BUTTONS_BITS (sizeof(WORD) * CHAR_BIT)
#define MAX_CONTROLLER_COUNT 4

bool Gamepad::FindNextController (DWORD _startIndex, DWORD& _iOut)
{
	if (_startIndex < 0 || _startIndex >= MAX_CONTROLLER_COUNT)
	{
		throw std::logic_error ("Start index argument out of bounds");
	}
	for (DWORD iU{ 0 }; iU < MAX_CONTROLLER_COUNT; iU++)
	{
		DWORD iCand{ (_startIndex + iU) % MAX_CONTROLLER_COUNT };

		XINPUT_STATE state;
		switch (XInputGetState (iCand, &state))
		{
			case ERROR_SUCCESS:
				_iOut = iCand;
				return true;
			case ERROR_DEVICE_NOT_CONNECTED:
				break;
		}
	}
	return false;
}

void Gamepad::Listener::OnConnected (const Gamepad& _ctrl) {}
void Gamepad::Listener::OnDisconnected (const Gamepad& _ctrl) {}
void Gamepad::Listener::OnButtonChanged (const Gamepad& _ctrl, WORD _buttons, bool _pressed) {}
void Gamepad::Listener::OnThumbChanged (const Gamepad& _ctrl, Gamepad::Thumb _thumb, Gamepad::Side _side) {}
void Gamepad::Listener::OnTriggerChanged (const Gamepad& _ctrl, float _value, Gamepad::Side _side) {}

bool Gamepad::Update ()
{
	const bool bWasStateValid{ m_bStateValid };
	const bool bWasConnected{ m_bConnected };
	const XINPUT_STATE prevState{ m_State };

	XINPUT_STATE state;
	switch (XInputGetState (m_iUser, &state))
	{
		case ERROR_SUCCESS:
			m_bConnected = true;
			m_bStateValid = true;
			m_State = state;
			if ((!bWasStateValid || !bWasConnected) && pListener)
			{
				pListener->OnConnected (*this);
			}
			if (!bWasStateValid)
			{
				FireAllEvents ();
				return true;
			}
			else
			{
				return ProcessEvents (prevState);
			}
		case ERROR_DEVICE_NOT_CONNECTED:
			m_bConnected = false;
			m_bStateValid = false;
			if (!bWasStateValid || bWasConnected)
			{
				if (pListener)
				{
					pListener->OnDisconnected (*this);
				}
				return true;
			}
			else
			{
				return false;
			}
		default:
			throw std::runtime_error ("XInput query error");
	}
}

bool Gamepad::IsConnected () const
{
	return m_bConnected;
}

Gamepad::Thumb Gamepad::GetThumb (Side _side) const
{
	if (!IsConnected ())
	{
		throw std::logic_error ("Not connected");
	}
	Thumb thumb;
	switch (_side)
	{
		case Side::LEFT:
			thumb.x = m_State.Gamepad.sThumbLX / MAX_THUMB_VALUE;
			thumb.y = m_State.Gamepad.sThumbLY / MAX_THUMB_VALUE;
			break;
		case Side::RIGHT:
			thumb.x = m_State.Gamepad.sThumbRX / MAX_THUMB_VALUE;
			thumb.y = m_State.Gamepad.sThumbRY / MAX_THUMB_VALUE;
			break;
		default:
			throw std::logic_error ("Unknown side");
	}
	return thumb;
}

Gamepad::Thumb Gamepad::GetThumb (Side _side, Thumb _def) const
{
	return IsConnected () ? GetThumb (_side) : _def;
}

bool Gamepad::AreButtonsPressed (WORD _msk) const
{
	if (!IsConnected ())
	{
		throw std::logic_error ("Not connected");
	}
	return (m_State.Gamepad.wButtons & _msk) == _msk;
}

bool Gamepad::AreButtonsPressed (WORD _msk, bool _def) const
{
	return IsConnected () ? AreButtonsPressed (_msk) : _def;
}

float Gamepad::GetTrigger (Side _side) const
{
	if (!IsConnected ())
	{
		throw std::logic_error ("Not connected");
	}
	switch (_side)
	{
		case Side::LEFT:
			return m_State.Gamepad.bLeftTrigger / MAX_TRIGGER_VALUE;
		case Side::RIGHT:
			return m_State.Gamepad.bRightTrigger / MAX_TRIGGER_VALUE;
		default:
			throw std::logic_error ("Unknown side");
	}
}

float Gamepad::GetTrigger (Side _side, float _def) const
{
	return IsConnected () ? GetTrigger (_side) : _def;
}

void Gamepad::SetVibration (float _lf, float _hf) const
{
	if (!IsConnected ())
	{
		throw std::logic_error ("Not connected");
	}
	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = static_cast<WORD>(_lf * MAX_VIBRATION_VALUE);
	vibration.wRightMotorSpeed = static_cast<WORD>(_hf * MAX_VIBRATION_VALUE);
	XInputSetState (m_iUser, &vibration);
}

DWORD Gamepad::GetUserIndex () const
{
	return m_iUser;
}

void Gamepad::SetUserIndex (DWORD _ind)
{
	if (_ind < 0 || _ind >= MAX_CONTROLLER_COUNT)
	{
		throw std::logic_error ("Index argument out of bounds");
	}
	if (m_iUser != _ind)
	{
		m_bStateValid = false;
	}
	m_iUser = _ind;
}

bool Gamepad::ProcessEvents (const XINPUT_STATE& _prevState) const
{
	if (_prevState.dwPacketNumber != m_State.dwPacketNumber)
	{
		if (pListener)
		{
			const XINPUT_GAMEPAD& prevGamepad{ _prevState.Gamepad };
			const XINPUT_GAMEPAD& currGamepad{ m_State.Gamepad };
			WORD buttonDiffs{ static_cast<WORD>(prevGamepad.wButtons ^ currGamepad.wButtons) };
			for (int iB{ 0 }; iB < BUTTONS_BITS; iB++)
			{
				WORD button = 1 << iB;
				if (buttonDiffs & button)
				{
					pListener->OnButtonChanged (*this, button, currGamepad.wButtons & button);
				}
			}
			if (prevGamepad.sThumbLX != currGamepad.sThumbLX || prevGamepad.sThumbLY != currGamepad.sThumbLY)
			{
				pListener->OnThumbChanged (*this, GetThumb (Side::LEFT), Side::LEFT);
			}
			if (prevGamepad.sThumbRX != currGamepad.sThumbRX || prevGamepad.sThumbRY != currGamepad.sThumbRY)
			{
				pListener->OnThumbChanged (*this, GetThumb (Side::RIGHT), Side::RIGHT);
			}
			if (prevGamepad.bLeftTrigger != currGamepad.bLeftTrigger)
			{
				pListener->OnTriggerChanged (*this, GetTrigger (Side::LEFT), Side::LEFT);
			}
			if (prevGamepad.bRightTrigger != currGamepad.bRightTrigger)
			{
				pListener->OnTriggerChanged (*this, GetTrigger (Side::RIGHT), Side::RIGHT);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

void Gamepad::FireAllEvents () const
{
	if (pListener)
	{
		for (int iB{ 0 }; iB < BUTTONS_BITS; iB++)
		{
			WORD button = 1 << iB;
			pListener->OnButtonChanged (*this, button, m_State.Gamepad.wButtons & button);
		}
		pListener->OnThumbChanged (*this, GetThumb (Side::LEFT), Side::LEFT);
		pListener->OnThumbChanged (*this, GetThumb (Side::RIGHT), Side::RIGHT);
		pListener->OnTriggerChanged (*this, GetTrigger (Side::LEFT), Side::LEFT);
		pListener->OnTriggerChanged (*this, GetTrigger (Side::RIGHT), Side::RIGHT);
	}
}