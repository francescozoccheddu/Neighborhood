#include <Game/Utils/Storage.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <string>
#include <fstream>

char * Storage::LoadBinaryFile (const std::string& _filename, int& _size)
{
	std::ifstream file{ _filename,std::ios::ate | std::ios::binary | std::ios::in };
	GAME_ASSERT_MSG (file.is_open (), "File not opened");
	std::streampos size{ file.tellg () };
	file.seekg (std::ios::beg);
	char * pBuf{ new char[static_cast<unsigned int>(size)] };
	file.read (pBuf, size);
	file.close ();
	_size = size;
	return pBuf;
}

std::string Storage::LoadTextFile (const std::string& _filename)
{
	std::ifstream file{ _filename, std::ios::in };
	GAME_ASSERT_MSG (file.is_open (), "File not opened");
	return std::string{ std::istreambuf_iterator<char> (file), std::istreambuf_iterator<char> () };
}