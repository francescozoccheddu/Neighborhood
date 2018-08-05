#include "ErrorLogger.hpp"

#include <sstream>

std::string GetErrorMessage (DWORD _error)
{
	LPSTR msgPtr;
	const DWORD msgLen { FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, _error, 0, reinterpret_cast<LPSTR>(&msgPtr), 0, nullptr) };
	std::string msg { msgPtr, msgPtr + msgLen };
	LocalFree (msgPtr);
	return msg;
}

void PostError (const char * _message)
{
	if (IsDebuggerPresent ())
	{
		OutputDebugStringA (_message);
		OutputDebugStringA ("\n");
		DebugBreak ();
	}
	else
	{
		if (MessageBoxA (nullptr, _message, SOLUTIONNAME " - " "Error happened", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
		{
			ExitProcess (0);
		}
	}
}

void PostError (const char * _message, DWORD _error)
{
	std::stringstream ss;
	ss << _message;
	ss << "\n";
	ss << GetErrorMessage (_error);
	PostError (ss.str ().c_str ());
}

void PostLastError (const char * _message)
{
	PostError (_message, GetLastError ());
}