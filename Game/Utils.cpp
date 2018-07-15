#include "Utils.hpp"

#include <windef.h>
#include <string>
#include <fileapi.h>
#include <comdef.h>
#include "Exceptions.hpp"

void ThrowIfCOMFailed (HRESULT _result, const char * _message)
{
	if (FAILED (_result))
	{
		GAME_COM_THROW_MSG (_result, _message);
	}
}

char * LoadBlob (const std::string & _filename, size_t& _length)
{
	HANDLE file{ CreateFileA (_filename.c_str (), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr) };
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD size{ GetFileSize (file, nullptr) };
		char * buf{ new char[size] };
		DWORD read;
		ReadFile (file, buf, size, &read, nullptr);
		CloseHandle (file);
		_length = read;
		return buf;
	}
	else
	{
		GAME_THROW ("File not opened");
	}
}
