#pragma once

#include <Game/Windows.hpp>
#include <string>

std::string GetErrorMessage (DWORD error);

void PostError (const char* message);

void PostError (const char * message, DWORD error);

void PostLastError (const char * message);