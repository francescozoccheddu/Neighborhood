#include "Exceptions.hpp"

#include <string>
#include <exception>
#include <sstream>
#include <comdef.h>

GameException::GameException (const char * _file, int _line, const char * _message) : file{ _file }, line{ _line }, message{ _message } {}
GameException::GameException (const char * _function, const char * _message) : function{ _function }, message{ _message } {}
GameException::GameException (const char * _function, const char * _file, int _line, const char * _message) : function{ _function }, file{ _file }, line{ _line }, message{ _message } {}
GameException::GameException (const char * _message) : message{ _message } {}

const char* GameException::what () const throw()
{
	if (!m_cached)
	{
		m_cached = true;
		std::stringstream ss;
		makeMessage (ss);
		m_what = ss.str ();
	}
	return m_what.c_str ();
}

const char* GameException::type () const
{
	return "GameError";
}

void GameException::makeMessage (std::stringstream& _msg) const
{
	_msg << "Type: ";
	_msg << type ();
	if (function)
	{
		_msg << "\nFunction: ";
		_msg << function;
	}
	if (file)
	{
		_msg << "\nFile: ";
		_msg << file;
		_msg << "\nLine: ";
		_msg << line;
	}
	if (message)
	{
		_msg << "\nMessage: ";
		_msg << message;
	}
	_msg << '\n';
}

GameCOMFailure::GameCOMFailure (HRESULT _result, const char * _function, const char * _message) : GameException{ _function,_message }, result{ _result }, comMessage{ getCOMMessage (_result) } {}
GameCOMFailure::GameCOMFailure (HRESULT _result, const char * _file, int _line, const char * _message) : GameException{ _file,_line,_message }, result{ _result }, comMessage{ getCOMMessage (_result) } {}
GameCOMFailure::GameCOMFailure (HRESULT _result, const char * _function, const char * _file, int _line, const char * _message) : GameException{ _function, _file,_line,_message }, result{ _result }, comMessage{ getCOMMessage (_result) } {}
GameCOMFailure::GameCOMFailure (HRESULT _result, const char * _message) : GameException {_message }, result{ _result }, comMessage{ getCOMMessage (_result) } {}

const char* GameCOMFailure::type () const
{
	return "GameCOMFailure";
}

void GameCOMFailure::makeMessage (std::stringstream& _msg) const
{
	GameException::makeMessage (_msg);
	_msg << "Result: ";
	_msg << result;
	_msg << "\nCOM Message: ";
	_msg << comMessage;
	_msg << '\n';
}

#ifdef UNICODE
const std::string& GameCOMFailure::getCOMMessage (HRESULT _result)
{
	std::wstring msg{ _com_error (_result).ErrorMessage () };
	return std::string{ msg.begin (), msg.end () };
}
#else
const char * GameCOMFailure::getCOMMessage (HRESULT _result)
{
	return _com_error (_result).ErrorMessage ();
}
#endif