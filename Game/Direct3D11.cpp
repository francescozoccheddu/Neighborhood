#include "Direct3D11.h"

#include <string>
#include <stdexcept>
#include <comdef.h>
#include <fileapi.h>

#pragma comment(lib, "d3d11.lib")

void HRThrow (HRESULT _hr, const char * _desc)
{
	const TCHAR * hrMsg{ _com_error (_hr).ErrorMessage () };
	std::string msg{ _desc };
	msg += "\nMessage: ";
#ifndef UNICODE
	msg += std::string{ hrMsg };
#else
	std::wstring hrWMsg{ hrMsg };
	msg += std::string{ hrWMsg.begin (), hrWMsg.end () };
#endif
	throw std::runtime_error (msg);
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
		throw std::runtime_error ("Cannot load BLOB. File not opened.");
	}
}
