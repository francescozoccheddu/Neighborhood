#pragma once

#include <Game/Resources/Resource.hpp>
#include <string>

class FileResource : public Resource
{

public:

	FileResource (const std::string& fileName);

	const std::string& GetFileName () const;

private:

	const std::string m_FileName;

};