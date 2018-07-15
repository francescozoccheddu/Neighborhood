#pragma once

#include <string>
#include <exception>
#include <sstream>
#include <comdef.h>

#ifdef _DEBUG
#ifndef GAME_THROW_FILE_ENABLE
#define GAME_THROW_FILE_ENABLE 1
#endif
#ifndef GAME_THROW_FUNCTION_ENABLE
#define GAME_THROW_FUNCTION_ENABLE 1
#endif
#else
#ifndef GAME_THROW_FILE_ENABLE
#define GAME_THROW_FILE_ENABLE 0
#endif
#ifndef GAME_THROW_FUNCTION_ENABLE
#define GAME_THROW_FUNCTION_ENABLE 0
#endif
#endif

#if GAME_THROW_FILE_ENABLE && GAME_THROW_FUNCTION_ENABLE
#define GAME_THROW_MSG(msg) { throw GameException{ __FUNCTION__, __FILE__, __LINE__, msg }; }
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, __FUNCTION__, __FILE__, __LINE__, msg }; }
#elif GAME_THROW_FILE_ENABLE
#define GAME_THROW_MSG(msg) { throw GameException{ __FILE__, __LINE__, msg }; }
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, __FILE__, __LINE__, msg }; }
#elif GAME_THROW_FUNCTION_ENABLE
#define GAME_THROW_MSG(msg) { throw GameException{ __FUNCTION__, msg }; }
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, __FUNCTION__, msg }; }
#else
#define GAME_THROW_MSG(msg) { throw GameException{ msg }; }
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, msg }; }
#endif

#define GAME_THROW GAME_THROW_MSG(nullptr)
#define GAME_COM_THROW(hr) GAME_COM_THROW_MSG(hr,nullptr)

class GameException : public std::exception
{
public:
	const char * const file{ nullptr };
	const char * const function{ nullptr };
	const int line{ -1 };
	const char * const message{ nullptr };

	GameException (const char * function, const char * message);
	GameException (const char * file, int line, const char * message);
	GameException (const char * function, const char * file, int line, const char * message);
	GameException (const char * message);

	const char* what () const throw() override final;

	virtual const char* type () const;

protected:
	virtual void makeMessage (std::stringstream& message) const;

private:
	mutable std::string m_what;
	mutable bool m_cached{ false };

};

class GameCOMFailure : public GameException
{
public:
	const HRESULT result;
#if UNICODE
	const std::string comMessage;
#else
	const char * const comMessage;
#endif

	GameCOMFailure (HRESULT result, const char * function, const char * message);
	GameCOMFailure (HRESULT result, const char * file, int line, const char * message);
	GameCOMFailure (HRESULT result, const char * function, const char * file, int line, const char * message);
	GameCOMFailure (HRESULT result, const char * message);

	const char* type () const override;

protected:
	void makeMessage (std::stringstream& message) const override;

private:
#if UNICODE
	static const std::string& getCOMMessage (HRESULT result);
#else
	static const char * getCOMMessage (HRESULT result);
#endif

};