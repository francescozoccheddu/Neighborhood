#include <Game/Resources/FileResource.hpp>

const std::string& FileResource::GetFileName () const
{
	return m_FileName;
}

FileResource::FileResource (const std::string& _fileName) : m_FileName { _fileName } {}