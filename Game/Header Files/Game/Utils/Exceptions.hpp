#pragma once

#include <string>
#include <exception>
#include <sstream>

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
#elif GAME_THROW_FILE_ENABLE
#define GAME_THROW_MSG(msg) { throw GameException{ __FILE__, __LINE__, msg }; }
#elif GAME_THROW_FUNCTION_ENABLE
#define GAME_THROW_MSG(msg) { throw GameException{ __FUNCTION__, msg }; }
#else
#define GAME_THROW_MSG(msg) { throw GameException{ msg }; }
#endif

#define GAME_THROW GAME_THROW_MSG(nullptr)
#define GAME_ASSERT(x) {if (!(x)) { GAME_THROW }}
#define GAME_ASSERT_MSG(x,msg) {if (!(x)) { GAME_THROW_MSG(msg) }}


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

