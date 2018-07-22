#include "Utils.hpp"

#include <string>
#include <comdef.h>
#include "Exceptions.hpp"
#include <fstream>

void ThrowIfCOMFailed (HRESULT _result, const char * _message)
{
	if (FAILED (_result))
	{
		GAME_COM_THROW_MSG (_result, _message);
	}
}

char * LoadBlob (const std::wstring & _filename, size_t& _length)
{
	CREATEFILE2_EXTENDED_PARAMETERS pars;
	pars.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	pars.hTemplateFile = nullptr;
	pars.dwSize = sizeof (pars);
	pars.dwFileFlags = 0;
	pars.lpSecurityAttributes = nullptr;
	pars.dwSecurityQosFlags = 0;
	HANDLE file{ CreateFile2 (_filename.c_str (), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &pars) };
	if (file != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER size;
		GetFileSizeEx (file, &size);
		char * buf{ new char[size.LowPart] };
		DWORD read;
		ReadFile (file, buf, size.LowPart, &read, nullptr);
		CloseHandle (file);
		_length = read;
		return buf;
	}
	else
	{
		GAME_THROW ("File not opened");
	}
}