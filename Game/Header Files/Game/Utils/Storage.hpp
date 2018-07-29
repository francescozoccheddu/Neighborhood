#pragma once

#include <string>

namespace Storage
{

	char * LoadBinaryFile (const std::string& filename, int& size);

	std::string LoadTextFile (const std::string& filename);

}