#pragma once

#include <Game/Utils/Exceptions.hpp>
#include <comdef.h>

#if GAME_THROW_FILE_ENABLE && GAME_THROW_FUNCTION_ENABLE
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, __FUNCTION__, __FILE__, __LINE__, msg }; }
#elif GAME_THROW_FILE_ENABLE
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, __FILE__, __LINE__, msg }; }
#elif GAME_THROW_FUNCTION_ENABLE
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, __FUNCTION__, msg }; }
#else
#define GAME_COM_THROW_MSG(hr,msg) { throw GameCOMFailure{ hr, msg }; }
#endif

#define GAME_COM_THROW(hr) GAME_COM_THROW_MSG(hr, nullptr)

#if GAME_COMC_ENABLE
#define GAME_COMC_MSG(res,msg) { _ThrowIfCOMFailed(res,msg); }
#else
#define GAME_COMC_MSG(res,msg) { res; }
#endif

#define GAME_COMC(res) GAME_COMC_MSG(res,nullptr)

class GameCOMFailure : public GameException
{
public:

	const HRESULT result;
	const std::string comMessage;

	GameCOMFailure (HRESULT result, const char * function, const char * message);
	GameCOMFailure (HRESULT result, const char * file, int line, const char * message);
	GameCOMFailure (HRESULT result, const char * function, const char * file, int line, const char * message);
	GameCOMFailure (HRESULT result, const char * message);

	const char* type () const override;

protected:
	void makeMessage (std::stringstream& message) const override;

private:
	static const std::string getCOMMessage (HRESULT result);

};

inline void _ThrowIfCOMFailed (HRESULT _result, const char * _message)
{
	if (FAILED (_result))
	{
		GAME_COM_THROW_MSG (_result, _message);
	}
}