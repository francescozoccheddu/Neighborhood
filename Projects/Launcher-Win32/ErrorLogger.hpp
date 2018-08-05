#pragma once

#include <Game/pch.hpp>
#include <string>

std::string GetErrorMessage (DWORD error);

void PostError (const char* message);

void PostError (const char * message, DWORD error);

void PostLastError (const char * message);